/*
 Name:		RemoteControl.ino
 Created:	01.04.2020 7:38:01
 Author:	Анатолий
 ==============================================================================
 Пульт включает в себя следующие элекменты:
 1.	Джойстик двухосевой (по два резистора на ось)
 2.	LCD 1602 - для вывода информации
 3.	NFR24L01 - с антеной
 4.	Два тумблера: 1- питание всего пульта; 2 - превод пульта в режим
		инициализации данных от джойстика
 5.	Трехпозиционная кнопка - для выбора максимальной скорости (возможно будет и
		при работе в режиме упроавления ее как-то использовать

 */
 /// Описание подключений
 /* Описание подключений
 A0	-	fX прямой по оси Х
 A1		fY прямой по Y
 A2		bY обратный по Y
 A3		bX обратный по X
 A4	-	LCD 1602	SDA
 A5	-	LCD 1602	SCL
 A6
 A7 - battaty voltage
 D01
 D02
 D03	-	LCD 1602 Light
 D04
 D05															 CE
 D06	-	Button	Velocity-					 CSN
 D07	-	Button	Velocity+							Vel-
 D08	-	Button	inicJoy								Vel+
 D09	-	NRF	CE												inc
 D10	-	NRF	CSN
 D11	-	NRF	MOSI
 D12	-	NRF	MISO
 D13	-	NRF	SCK


 */
#define sizeRusArray 21

#include "DubleAxis.h"
#include "printf.h"
#include <EEPROM.h>

# include <Wire.h>
# include "LiquidCrystal_I2C.h"    // Подключаем библиотеку LiquidCrystal_I2C



/// Определяем подключение резиторов джойстика
int fX = A0;// A2;
int bX = A3;// A5;
int fY = A1;// A3;
int bY = A2;// A4;

uint8_t pinVolt = A7;	//	подключенный через делитель на основное напряжение (6,04 В - 630 единиц)

/// подключение кнопок
int signalPin = 9;// 8;		//	пин инициализации джойстика
uint8_t velPinPlus = 8;// 7;		//	увеличение максимальной скорости
uint8_t velPinMinus = 7;// 6;	//	снижение максимальной скорости

Joy2_2axis myJoy;
X_Y_data workData;
ERROMdata workSaveData;	// переменная для хранения параметров джойстика

// параметры дисплея
word addresLCD1602 = 0x38;
LiquidCrystal_I2C lcd(addresLCD1602, 16, 2); // Задаем адрес и размерность дисплея.
uint8_t lcdLight = 3;

/* параметры управления скоростью: 1-5
	 5 - 1024
	 4	 640
	 3	 384
	 2	 256
	 1 - 128
*/
int maxPeredacha = 5;
int maxWorkSpeed[] = { 0,128,256,384,640,1024 };
int peredacha = 3;
int maxSpeed = 384;
// the setup function runs once when you press reset or power the board
/* Структура для хранения информации о используемых русских символах и их текущем временном номере*/
struct LCDRusChar
{
	byte codRusCh;			//	код русской буквы после символа 208
	byte* bykvaPoint;		//	ссылка на попиксельное отображение буквы
	uint8_t temNumber;	//	временно присвоенный номер

};
LCDRusChar aaa[sizeRusArray];	// массив рабочей структуры

void setup() {

	// инициализируем (заполняем) массив ссылок на буквы 
	outInicArray();

	Serial.begin(9600);
	printf_begin();
	
	pinMode(signalPin, INPUT_PULLUP);		//	перевели в высокоимпедансное состояние
	pinMode(velPinMinus, INPUT_PULLUP);	//	снижение скорости
	pinMode(velPinPlus, INPUT_PULLUP);	//	увеличение скорости
	myJoy.setPinAxis(fX, bX, fY, bY);		//	определили рабочие пины для джойстика

	pinMode(lcdLight, OUTPUT);	//	управление подсветкой

	lcd.init();	//	Инициализация lcd дисплея
	digitalWrite(lcdLight, HIGH);
	outLCD("Привет!", 0, 0); delay(2000);
	outLCD("Проверка центра", 0, 1);
	// проверяем центр (8 раз считываем данные)
	F_B_2 searthCenter; F_B_2* _searthCenter = &searthCenter;
	searthCenter = myJoy.rawRead();
	F_B_2 minCenter = searthCenter; F_B_2* _minCenter = &minCenter;
	F_B_2 maxCenter = searthCenter;	F_B_2* _maxCenter = &maxCenter;
	F_B_2 errAxis; F_B_2* _errAxis = &errAxis;
	myJoy.readWorkCenter(_searthCenter, _minCenter, _maxCenter, _errAxis);	//	получили данные текущего состояния джойстика
	delay(1500);
	// инициализация джойстика если включена кнопка ИНИЦИАЛИЗАЦИИ
	if (digitalRead(signalPin) == LOW) {	//	если нужно инициализировать
		clearNumRusChar();
		lcd.clear();
		outLCD("инициализация", 0, 0);
		outLCD("джойстика...", 0, 1);
//		MSG_inicialization(); delay(1000);	//	начинаем инициализацию джойстика
		myJoy.newInic(searthCenter, errAxis, signalPin);	// инициализировали и записали в память
		clearNumRusChar();
		lcd.clear();
		outLCD("инициализация", 0, 0);
		outLCD("закончена...", 0, 1);
		delay(2000);
	}
	delay(1000);
	EEPROM.get(0, workSaveData);	// считали из памяти
	clearNumRusChar();
	lcd.clear();
	outLCD("Настраиваемся", 0, 0);
	lcd.setCursor(0, 1);
	for (int i = 0; i < 16; i++)	//	имитация загрузки параметров
	{ lcd.write('*'); delay(50 * i); }

	//	MSG_readData(); delay(2000);
	
	//	проверка совпадения текущего центрального расположения и ранее записанного

	int GoodJoy = 0;
	if (
		(workSaveData.X.F.center > (searthCenter.X.F + errAxis.X.F)) ||
		(workSaveData.X.F.center < (searthCenter.X.F - errAxis.X.F)) ||
		(workSaveData.X.B.center > (searthCenter.X.B + errAxis.X.B)) ||
		(workSaveData.X.B.center < (searthCenter.X.B - errAxis.X.B)) ||
		(workSaveData.Y.F.center > (searthCenter.Y.F + errAxis.Y.F)) ||
		(workSaveData.Y.F.center < (searthCenter.Y.F - errAxis.Y.F)) ||
		(workSaveData.Y.B.center > (searthCenter.Y.B + errAxis.Y.B)) ||
		(workSaveData.Y.B.center < (searthCenter.Y.B - errAxis.Y.B))
		) {
		GoodJoy++;
	}

	if (GoodJoy > 0) {
		int i = 0;
		lcd.clear();
		clearNumRusChar();
		outLCD("Неисправен", 0, 0);
		outLCD("джойстик", 0, 1);
		while (i < 100) {
			delay(500);
			digitalWrite(lcdLight, LOW);
			delay(500);
			digitalWrite(lcdLight, HIGH);
		}
		// у нас проблеммы с джойстиком!!! надо определиться, можно ли двигаться?
		// нужно определить критерии и, возможно, нужно еще и проверять как оно было до этого
		// т.к. если проведена инициализация джойстика - переменная будет равна "0"
//		MSG_errorJoy();
//		delay(2000);
//		MSG_changeJoy();
	} else {
 		// система в нормальном состоянии
		lcd.clear();
		clearNumRusChar();
		outLCD("Система настрена", 0, 0);
//		outLCD("джойстик", 0, 1);
	}

	// переходим к установке параметров
	myJoy.setParam(
		workSaveData.X.F, workSaveData.X.B, workSaveData.SUMM_X, workSaveData.ERR_X,
		workSaveData.Y.F, workSaveData.Y.B, workSaveData.SUMM_Y, workSaveData.ERR_Y);

	printf("начали \n");



	lcd.clear();
	lcd.setCursor(0, 1);
	lcd.print("Max speed: ");
	lcd.print(peredacha);
	delay(2000);
	digitalWrite(lcdLight, LOW);

	
}

// the loop function runs over and over again until power down or reset
void loop() {
	
	
	
	// проверяем нажатие какой либо клавиши (нужно избавиться от дребезга)
//	Serial.println(digitalRead(velPinMinus));
	if (digitalRead(velPinMinus)==LOW){
		peredacha++;
		if (peredacha > maxPeredacha) { peredacha = maxPeredacha; }
		lcd.clear();
		lcd.setCursor(0, 1);
		lcd.print("Max speed: ");
		lcd.print(peredacha);

		delay(200);
	}
	if (digitalRead(velPinPlus) == LOW) {
		peredacha--;
		if (peredacha < 1) { peredacha = 1; }
		lcd.clear();
		lcd.setCursor(0, 1);
		lcd.print("Max speed: ");
		lcd.print(peredacha);
		delay(200);
	}
	myJoy.readData();
	myJoy.calcData();
	X_Y_data myLocal = myJoy.getData();
	// проверяем изменение данных относительно предыдущих с учетом ошибки (корректируем)
	if ((workData.X > (myLocal.X + workSaveData.X.F.error)) || (workData.X < (myLocal.X - workSaveData.X.F.error))) {
		workData.X = myLocal.X; workData.errX = workSaveData.X.F.error;
	}
	if ((workData.Y > (myLocal.Y + workSaveData.Y.F.error)) || (workData.Y < (myLocal.Y - workSaveData.Y.F.error))) {
		workData.Y = myLocal.Y; workData.errY = workSaveData.Y.F.error;
	}
	myLocal = workData;
	//	рабочая область за вычитом дребезга из параметров джойстика
	int xMin = workSaveData.X.F.min + workSaveData.X.F.error;
	int xMax = workSaveData.X.F.max - workSaveData.X.F.error;
	int yMin = workSaveData.Y.F.min + workSaveData.Y.F.error;
	int yMax = workSaveData.Y.F.max - workSaveData.Y.F.error;
	// обрезаем выбросы
	constrain(myLocal.X, xMin, xMax);
	constrain(myLocal.Y, yMin, yMax);
	//	центрируем и раскладываем в +/- для получения ровного квадрата от -1024 до 1024
	myLocal.X = myLocal.X - workSaveData.X.F.center;
	myLocal.Y = myLocal.Y - workSaveData.Y.F.center;
	if (myLocal.X >= 0) { myLocal.X = map(myLocal.X, 0, (xMax - workSaveData.X.F.center), 0, maxWorkSpeed[peredacha]); }
	if (myLocal.X < 0) { myLocal.X = map(myLocal.X, (xMin - workSaveData.X.F.center), 0, -maxWorkSpeed[peredacha], 0); }
	if (myLocal.Y >= 0) { myLocal.Y = map(myLocal.Y, 0, (xMax - workSaveData.Y.F.center), 0, maxWorkSpeed[peredacha]); }
	if (myLocal.Y < 0) { myLocal.Y = map(myLocal.Y, (xMin - workSaveData.Y.F.center), 0, -maxWorkSpeed[peredacha], 0); }

	//	printf("Данные ось X=%d \t ERR=%d \t ось Y=%d \t ERR=%d \n", myLocal.X, myLocal.errX, myLocal.Y, myLocal.errY);

	// раскладываем на два борта борта с учетом основного рапложения джойстика по оси Y
	// при положении джойстика "назад" логика по ускорению бортов меняется

	int bortL, bortR;
	bortL = -myLocal.Y; bortR = myLocal.Y;
	bortL = bortL - myLocal.X; bortR = bortR - myLocal.X;

	// нормализуем по максимальному значению не более +/- 1024
	if (bortL > maxWorkSpeed[peredacha]) { bortL = maxWorkSpeed[peredacha]; }
	if (bortL < -maxWorkSpeed[peredacha]) { bortL = -maxWorkSpeed[peredacha]; }
	if (bortR > maxWorkSpeed[peredacha]) { bortR = maxWorkSpeed[peredacha]; }
	if (bortR < -maxWorkSpeed[peredacha]) { bortR = -maxWorkSpeed[peredacha]; }
	printf("Левая = %d \t Правая=%d \n", bortL, bortR);

	//	workData = myLocal;
	
	
	//	пишем данные о состоянии батареи
	lcd.setCursor(0, 0);
	lcd.print("TX->");
	int analogVolt=analogRead(pinVolt);
	if (analogVolt > 900) { lcd.print("OK"); }
	if (analogVolt < 600) { lcd.print("LOW"); }
	if ((analogVolt >= 600) && (analogVolt <= 900)) {
		int volt = analogVolt / 100;
		int decVolt = analogVolt - (volt * 100);
		decVolt = decVolt / 10;
		char symb = volt + '0';
		char dSymb = decVolt + '0';
		lcd.write(symb); lcd.write('.'); lcd.write(dSymb);
//		lcd.print("   ");
	}
}
