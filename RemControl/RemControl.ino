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
 A7
 D01
 D02
 D03	-	LCD 1602 Light
 D04
 D05
 D06	-	Button	Velocity-
 D07	-	Button	Velocity+
 D08	-	Button	inicJoy
 D09	-	NRF	CE
 D10	-	NRF	CSN
 D11	-	NRF	MOSI
 D12	-	NRF	MISO
 D13	-	NRF	SCK


 */

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

/// подключение кнопок
int signalPin = 8;		//	пин инициализации джойстика
int velPunPlus = 7;		//	увеличение максимальной скорости
int velPinMinus = 6;	//	снижение максимальной скорости

Joy2_2axis myJoy;
X_Y_data workData;
ERROMdata workSaveData;	// переменная для хранения параметров джойстика

// параметры дисплея
word addresLCD1602 = 0x38;
LiquidCrystal_I2C lcd(addresLCD1602, 16, 2); // Задаем адрес и размерность дисплея.
int lcdLight = 3;


// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(9600);
	printf_begin();
	
	pinMode(signalPin, INPUT_PULLUP);	//	перевели в высокоимпедансное состояние
	myJoy.setPinAxis(fX, bX, fY, bY);	//	определили рабочие пины для джойстика

	pinMode(lcdLight, OUTPUT);	//	управление подсветкой

	lcd.init();	//	Инициализация lcd дисплея
	digitalWrite(lcdLight, HIGH);
	digitalWrite(lcdLight, HIGH);
	MSG_privet(); delay(2000);
	MSG_joyCenter(); delay(1500);
	MSG_joyTest();
	// проверяем центр (8 раз считываем данные)
	F_B_2 searthCenter; F_B_2* _searthCenter = &searthCenter;
	searthCenter = myJoy.rawRead();
	F_B_2 minCenter = searthCenter; F_B_2* _minCenter = &minCenter;
	F_B_2 maxCenter = searthCenter;	F_B_2* _maxCenter = &maxCenter;
	F_B_2 errAxis; F_B_2* _errAxis = &errAxis;
	myJoy.readWorkCenter(_searthCenter, _minCenter, _maxCenter, _errAxis);	//	получили данные текущего состояния джойстика
	delay(500);
	// инициализация джойстика если включена кнопка ИНИЦИАЛИЗАЦИИ
	if (digitalRead(signalPin) == LOW) {	//	если нужно инициализировать
		MSG_inicialization(); delay(1000);	//	начинаем инициализацию джойстика
		myJoy.newInic(searthCenter, errAxis, signalPin);	// инициализировали и записали в память
	}

	EEPROM.get(0, workSaveData);	// считали из памяти
	MSG_readData(); delay(2000);
	//
	//	проверка совпадения текущего центрального расположения и ранее записанного
	//
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
		// у нас проблеммы с джойстиком!!! надо определиться, можно ли двигаться?
		// нужно определить критерии и, возможно, нужно еще и проверять как оно было до этого
		// т.к. если проведена инициализация джойстика - переменная будет равна "0"
	}

	// переходим к установке параметров
	myJoy.setParam(
		workSaveData.X.F, workSaveData.X.B, workSaveData.SUMM_X, workSaveData.ERR_X,
		workSaveData.Y.F, workSaveData.Y.B, workSaveData.SUMM_Y, workSaveData.ERR_Y);

	printf("начали \n");


	lcd.backlight();                // Включение подсветки дисплея
	MSG_inicialization();
	delay(2000);
	MSG_centerOK();
	delay(2000);
	lcd.noBacklight();                // Включение подсветки дисплея
	digitalWrite(lcdLight, LOW);
	MSG_privet();
	delay(2000);
	MSG_inicialization();
	delay(2000);
	MSG_centerOK();

}

// the loop function runs over and over again until power down or reset
void loop() {
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
	if (myLocal.X >= 0) { myLocal.X = map(myLocal.X, 0, (xMax - workSaveData.X.F.center), 0, 1024); }
	if (myLocal.X < 0) { myLocal.X = map(myLocal.X, (xMin - workSaveData.X.F.center), 0, -1024, 0); }
	if (myLocal.Y >= 0) { myLocal.Y = map(myLocal.Y, 0, (xMax - workSaveData.Y.F.center), 0, 1024); }
	if (myLocal.Y < 0) { myLocal.Y = map(myLocal.Y, (xMin - workSaveData.Y.F.center), 0, -1024, 0); }

	//	printf("Данные ось X=%d \t ERR=%d \t ось Y=%d \t ERR=%d \n", myLocal.X, myLocal.errX, myLocal.Y, myLocal.errY);

	// раскладываем на два борта борта с учетом основного рапложения джойстика по оси Y
	// при положении джойстика "назад" логика по ускорению бортов меняется

	int bortL, bortR;
	bortL = -myLocal.Y; bortR = myLocal.Y;
	bortL = bortL - myLocal.X; bortR = bortR - myLocal.X;

	// нормализуем по максимальному значению не более +/- 1024
	if (bortL > 1024) { bortL = 1024; }
	if (bortL < -1024) { bortL = -1024; }
	if (bortR > 1024) { bortR = 1024; }
	if (bortR < -1024) { bortR = -1024; }
	// printf("Левая = %d \t Правая=%d \n", bortL, bortR);

	//	workData = myLocal;

}
