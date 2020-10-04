/*============================================================================
 Name:		ElectricGaz.ino
 Created:	12.05.2020 16:05:38
 Author:	Анатолий

 Управление электронным газом через ARDUINO

 ver: 1.0.18 E
 =============================================================================
 1.	В следующей версии - перевести выдаваемый управляющий сигнал на D9
 тогда счетчики будут работать нормально
 2.	Контроль поставить от второго резистора
 3.	Запись в постоянную память времени наработки.
 ===========================================================================
 ver 1.0.19 E
 изменена работа индикации. Теперь панель светится всегда, при включеном питании.
 Моргает, при низком заряде на аккумуляторах
 ============================================================================
 ver 1.1.0 E
 Добавлен подсчет моточасов. 
 ВНИМАНИЕ! Предварительно скинуть значения EEPROM в ноль! (программа ClearEEPROM)

 ============================================================================*/

#include <LiquidCrystal.h>
#include "printf.h"
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>    // Подключаем библиотеку LiquidCrystal_I2C

/*----------------------------------------------------------------------------
===				Описание подключений
D01
D02
D03		-	LCD1602 - подсветка
D04
D05			-	выходной (обработанный) сигнал
D06
D07
D08
D09
D10
D11
D12
D13

A0(14)
A1(15)
A2(16)
A3(17)	-	Контроль напряжения
A4(18)	-	LCD1602
A5(19)	-	LCD1602
A6(20)	-	контрольные данные резервного датчика
A7(21)	-	данные с основного датчика

-----------------------------------------------------------------------------*/
constexpr auto sizeRusArray = 21;	// кол-во русских символов


constexpr auto pinLightLCD		= 3;	//	управление подсветкой
constexpr auto pinOutVelocity	= 5;	//	выдача управляющего сигнала
constexpr auto pinVoltage		= 17;	//	входной сигнал напряжения
constexpr auto pinInputGaz		= 21;	//	основная информация
constexpr auto pinInputControl	= 20;	//	пока не используем

int minGazRead = 500;	//	минимальное значение подбирается под каждую педаль
int maxGazRead = 892;	//	полный газ под каждую педать

int minGazWrite = 40;	//	минимальное значение на выходного сигнала для нормального переключения скорости
int maxGazWrite = 180;	//	максимальное значение на выходного сигнала подбирается под каждый двигатель

/* блок для плавного трогания*/
unsigned long lastTimeIncrDecr;
int periodIncrDecr=100;					//	период отдачи команд на изменение скорости
int incrGazWrite = 1;
int lastGazWrite;

/* контроль напряжения*/
unsigned long lastTimeVoltage;	//	последнее определение напряжения
int periodTimeVoltage=100;
int nomTestVoltage = 1;					//	номер считанного напряжения
int kolvoTestVoltage = 8;				//	количество считываемых значений
int voltage = 0;								//	текущее значение вольтажа
int workVoltage = 0;						//	рабочее накопительное значение вольтажа
int lowVoltage=41;							//	значение при котором начинает моргать дисплей


// параметры дисплея
//word addresLCD1602 = 0x27; // 0x3F;
word addresLCD1602 = 0x3F;
LiquidCrystal_I2C lcd(addresLCD1602, 16, 2); // Задаем адрес и размерность дисплея.
int markerVoltage=0;
unsigned long lastOutVoltage;						//	последее время вывода на дисплей
unsigned int periodOutVoltage=62500;		//	период вывода напряжения на дисплей (в один герц)

struct LCDRusChar
{
	byte codRusCh;			//	код русской буквы после символа 208
	byte* bykvaPoint;		//	ссылка на попиксельное отображение буквы
	uint8_t temNumber;	//	временно присвоенный номер
};

LCDRusChar aaa[sizeRusArray];	// массив рабочей структуры

/* Данные для счетчика мото-часов*/
int nomLastNote = 0;						//	номер последней заполненной ячейки
unsigned int realTimeWork = 0;	//	реальное значение времени в минутах
unsigned long lastFixTime;			//	последнее зафиксированное время



void setup() {
	Wire.setClock(31000);
	Wire.begin();
	lcd.init();	//	Инициализация lcd дисплея
	lcd.backlight();
	delay(1000);
	outLCD("Tulpar xIII", 0, 0); delay(700);
	outLCD("ver: 1.1.0 E", 0, 1); delay(1500);
//	lcd.noBacklight();	// тушим панель - в 1.0.19Е - не тушим


	TCCR0B = 0b00000001;  // x1
	TCCR0A = 0b00000011;  // fast pwm
	
	Serial.begin(9600);
	printf_begin();
	printf("начали \n");
	pinMode(pinOutVelocity, OUTPUT);
	lastTimeIncrDecr = millis();				//  запомнили текущее время
	lastTimeVoltage = lastTimeIncrDecr;	//	и для считывания напряжения																									 
	lastOutVoltage=lastTimeIncrDecr;		//	для дисплея
	lastGazWrite = minGazWrite;

	SearthLastRealTimeWork();	//	поиск и инициализация наработанных часов

}

// the loop function runs over and over again until power down or reset
void loop() {
	unsigned long realTime = millis();
	if (realTime >= (64*60000)) {	//	если еще прошла одна минута
		realTime = realTime - (64*60000);	//	ставим маркер
	}
	else {
		realTime = lastFixTime - 1;	//	если минута не прошла - не даем уйти в минусовые значения
	}
	if (lastFixTime <= realTime) {	//	минута прошла
		realTimeWork++;								//	увеличили значение минут
		lastFixTime = millis();				//	зафиксировали время
		printf("Значение EEPROM ===================realTimeWork=%d \t lastFixTime=%d \n", realTimeWork, lastFixTime);
		saveEEPROM();	//	записали в память
	}

	driveGaz();
	loadVoltage();
//	printf ("voltage = %d \n", voltage);
	VoltageOutLCD();
}

/*
Управление газом
*/
void driveGaz() {
	int inputGaz = analogRead(pinInputGaz);
	int inputControl = analogRead(pinInputControl);
	//  printf("Gaz = %d == Control = %d Normal =", inputGaz, inputControl);
	  //constrain(inputGaz, 519, 830);
	if (inputGaz < minGazRead) { inputGaz = minGazRead; }
	if (inputGaz > maxGazRead) { inputGaz = maxGazRead; }
	//  printf("***Gaz = %d == Control = %d Normal =", inputGaz, inputControl);
	int normalGaz = map(inputGaz, minGazRead, maxGazRead, minGazWrite, maxGazWrite);
//	printf("%d \n", normalGaz);
	if (normalGaz > 50 && normalGaz < 75) { normalGaz = 80; }
	if (normalGaz < 50) { normalGaz = minGazWrite; }
	if (lastTimeIncrDecr < (millis() - periodIncrDecr)) { //  если пора слать новое значение скорости
		if (lastGazWrite > normalGaz) { //  надо затормозить
			lastGazWrite = lastGazWrite - incrGazWrite;
			if (lastGazWrite < normalGaz) { lastGazWrite = normalGaz; }
		}
		if (lastGazWrite < normalGaz) {
			lastGazWrite = lastGazWrite + incrGazWrite;
			if (lastGazWrite > normalGaz) { lastGazWrite = normalGaz; };
		}
		lastTimeIncrDecr = millis();
		analogWrite(pinOutVelocity, lastGazWrite);
	}
	//  analogWrite(5, normalGaz);
}

/* Считывание значений напряжения*/
void loadVoltage() {
	if (lastTimeVoltage < (millis() - periodTimeVoltage)) {	//	если пора считывать значение напряжения
		workVoltage = workVoltage + analogRead(pinVoltage);		//	добавили считанное значение в копилку
		nomTestVoltage++;																			//	увеличили значение счетчика
		if (nomTestVoltage > kolvoTestVoltage) {							//	считано достаточно кол-во измерений
			nomTestVoltage = 1;
			voltage = (workVoltage / kolvoTestVoltage) / 17;	//	17 подобрано эксперементально
			workVoltage = 0;
		}
		lastTimeVoltage = millis();
	}
}

/* Индикация напряжения */
void 	VoltageOutLCD() {
	// Проверяем пора ли выводить новое значение
	if (lastOutVoltage < (millis() - periodOutVoltage)) {
		lcd.setCursor(0,1);																	//	выводим значение
		outLCD("                ",0,1);
		outLCD("Volt:",0,1);
		lcd.print(voltage);
		lcd.print("V");
		outRealTime();	//	выводим значение отработанного времени
		if (voltage < lowVoltage) {													//	если напряжение низкое
			if (markerVoltage == 0) {													//	если дисплей потушен
				lcd.backlight(); markerVoltage=1;
			} else {
				lcd.noBacklight(); markerVoltage=0;
			}
		} else {
//			lcd.noBacklight();
			lcd.backlight();	// ver 1.0.19E - включаем дисплей
		}

		lastOutVoltage=millis();														//	запоминаем новое последнее время
	}

}

//	Поиск окончания последей записи и кол-ва минут
void SearthLastRealTimeWork() {
	int nomSeartch = 0;	//	начинаем с первой ячейки
	printf("Nachali poisk \n");
	while (nomSeartch <= 1023) {
		uint8_t readTimeWork1 = EEPROM.read(nomSeartch);	//	получаем старший байт
		nomSeartch++;																			//	переходим к следующему
		unsigned int readTimeWork = readTimeWork1 * 256 + EEPROM.read(nomSeartch);	//	формируем общее кол-во наработанных минут
		if (readTimeWork > realTimeWork) {	//	сравниваем с предыдущим значением
			nomLastNote = nomSeartch - 1;			//	если больше - запоминаем его расположение
			realTimeWork = readTimeWork;			//	делаем его ориентиром
		}
		nomSeartch++;	//	переместились 
	}
	printf("NomLastNote=%d \t realTimeWork=%u \n", nomLastNote, realTimeWork);
	lastFixTime = millis();	//	зафиксировали время для дальнейшего отсчета секунд

}

/* Запись значения моточасов*/
void saveEEPROM() {
	int nomSave = nomLastNote + 2;	//	сместились в следующую ячейку
	if (nomLastNote > 1023) {	//	если дошли до конца - начинаем с начала
		nomSave = 0;
	}
	nomLastNote = nomSave;
	uint8_t byteSt = realTimeWork / 256;
	uint8_t byteMl = realTimeWork - byteSt * 256;
	printf("nomLastNote= %d byteSt=%o \t byteMl=%o \n", nomLastNote, byteSt, byteMl);
	EEPROM.write(nomSave, byteSt);
	nomSave++;
	EEPROM.write(nomSave, byteMl);
}

void outRealTime() {
	int Chas = realTimeWork / 60;	//	полное значение часов
	int Minut = realTimeWork - Chas * 60;	//	кол-во минут
	int Chas_tsd = Chas / 10;
	int Chas_e = Chas - Chas_tsd * 10;
	int Chas_ts = Chas_tsd / 10;
	int Chas_d = Chas_tsd - Chas_ts * 10;
	int Chas_t = Chas_ts / 10;
	int Chas_s = Chas_ts - Chas_t * 10;

	int Minut_d = Minut / 10;
	int Minut_e = Minut - Minut_d * 10;
	lcd.print(" ");
	lcd.print(Chas_t); lcd.print(Chas_s); lcd.print(Chas_d); lcd.print(Chas_e);
	lcd.print(":");
	lcd.print(Minut_d); lcd.print(Minut_e);

}
