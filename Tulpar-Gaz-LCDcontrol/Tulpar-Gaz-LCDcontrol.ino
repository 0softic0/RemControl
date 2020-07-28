/*
 Name:		ElectricGaz.ino
 Created:	12.05.2020 16:05:38
 Author:	Анатолий

 Управление электронным газом через ARDUINO

 ver: 1.0.18 E
*/
#include <LiquidCrystal.h>
#include "printf.h"
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>    // Подключаем библиотеку LiquidCrystal_I2C

/* Описание подключений
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

*/
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
int nomTestVoltage = 1;			//	номер считанного напряжения
int kolvoTestVoltage = 8;		//	количество считываемых значений
int voltage = 0;				//	текущее значение вольтажа
int workVoltage = 0;			//	рабочее накопительное значение вольтажа
int lowVoltage=41;				//	значение при котором начинает моргать дисплей


// параметры дисплея
word addresLCD1602 = 0x3F;
LiquidCrystal_I2C lcd(addresLCD1602, 16, 2); // Задаем адрес и размерность дисплея.
int markerVoltage=0;
unsigned long lastOutVoltage;	//	последее время вывода на дисплей
int periodOutVoltage=2000;		//	период вывода напряжения на дисплей

/* Структура для хранения информации о используемых русских символах и их текущем временном номере*/
struct LCDRusChar
{
	byte codRusCh;			//	код русской буквы после символа 208
	byte* bykvaPoint;		//	ссылка на попиксельное отображение буквы
	uint8_t temNumber;	//	временно присвоенный номер
};

LCDRusChar aaa[sizeRusArray];	// массив рабочей структуры

// the setup function runs once when you press reset or power the board
void setup() {
	lcd.init();	//	Инициализация lcd дисплея
//	outInicArray();	//	инициализация массива русских букв
//	pinMode(pinLightLCD, OUTPUT);	//	инициализация пина управления подсветкой
//	digitalWrite(pinLightLCD, LOW); delay(1000);	//	включили подсветку
//	digitalWrite(pinLightLCD, HIGH); delay(1000);	//	включили подсветку
//	digitalWrite(pinLightLCD, LOW); delay(1000);	//	включили подсветку
//	digitalWrite(pinLightLCD, HIGH); delay(1000);	//	включили подсветку
//	delay(1000);
	lcd.backlight();
	delay(1000);
	outLCD("Tulpar xIII", 0, 0); delay(500);
	outLCD("ver: 1.0.18 E", 0, 1); delay(500);
	lcd.noBacklight();


	TCCR0B = 0b00000001;  // x1
	TCCR0A = 0b00000011;  // fast pwm
	
	Serial.begin(9600);
	printf_begin();
	pinMode(pinOutVelocity, OUTPUT);
	lastTimeIncrDecr = millis();				//  запомнили текущее время
	lastTimeVoltage = lastTimeIncrDecr;	//	и для считывания напряжения
	lastOutVoltage=lastTimeIncrDecr;		//	для дисплея
	lastGazWrite = minGazWrite;
	
	
}

// the loop function runs over and over again until power down or reset
void loop() {
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
		analogWrite(5, lastGazWrite);
	}
	//  analogWrite(5, normalGaz);
}

/* Считывание значений напряжения*/
void loadVoltage() {
	if (lastTimeVoltage < (millis() - periodTimeVoltage)) {	//	если пора считывать значение напряжения
		workVoltage = workVoltage + analogRead(pinVoltage);	//	добавили считанное значение в копилку
		nomTestVoltage++;	//	увеличили значение счетчика
		if (nomTestVoltage > kolvoTestVoltage) {	//	считано достаточно кол-во измерений
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
		lcd.setCursor(0,1);	//	выводим значение
		outLCD("                ",0,1);
		outLCD("Voltage:",0,1);
		lcd.print(voltage);
		if (voltage < lowVoltage) {	//	если напряжение низкое
			if (markerVoltage == 0) {	//	если дисплей потушен
				lcd.backlight(); markerVoltage=1;
			} else {
				lcd.noBacklight(); markerVoltage=0;
			}
		} else {
			lcd.noBacklight();
		}

		lastOutVoltage=millis();	//	запоминаем новое последнее время
	}

}
