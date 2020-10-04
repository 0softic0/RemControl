/*============================================================================
 Name:		ElectricGaz.ino
 Created:	12.05.2020 16:05:38
 Author:	��������

 ���������� ����������� ����� ����� ARDUINO

 ver: 1.0.18 E
 =============================================================================
 1.	� ��������� ������ - ��������� ���������� ����������� ������ �� D9
 ����� �������� ����� �������� ���������
 2.	�������� ��������� �� ������� ���������
 3.	������ � ���������� ������ ������� ���������.
 ===========================================================================
 ver 1.0.19 E
 �������� ������ ���������. ������ ������ �������� ������, ��� ��������� �������.
 �������, ��� ������ ������ �� �������������
 ============================================================================
 ver 1.1.0 E
 �������� ������� ���������. 
 ��������! �������������� ������� �������� EEPROM � ����! (��������� ClearEEPROM)

 ============================================================================*/

#include <LiquidCrystal.h>
#include "printf.h"
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>    // ���������� ���������� LiquidCrystal_I2C

/*----------------------------------------------------------------------------
===				�������� �����������
D01
D02
D03		-	LCD1602 - ���������
D04
D05			-	�������� (������������) ������
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
A3(17)	-	�������� ����������
A4(18)	-	LCD1602
A5(19)	-	LCD1602
A6(20)	-	����������� ������ ���������� �������
A7(21)	-	������ � ��������� �������

-----------------------------------------------------------------------------*/
constexpr auto sizeRusArray = 21;	// ���-�� ������� ��������


constexpr auto pinLightLCD		= 3;	//	���������� ����������
constexpr auto pinOutVelocity	= 5;	//	������ ������������ �������
constexpr auto pinVoltage		= 17;	//	������� ������ ����������
constexpr auto pinInputGaz		= 21;	//	�������� ����������
constexpr auto pinInputControl	= 20;	//	���� �� ����������

int minGazRead = 500;	//	����������� �������� ����������� ��� ������ ������
int maxGazRead = 892;	//	������ ��� ��� ������ ������

int minGazWrite = 40;	//	����������� �������� �� ��������� ������� ��� ����������� ������������ ��������
int maxGazWrite = 180;	//	������������ �������� �� ��������� ������� ����������� ��� ������ ���������

/* ���� ��� �������� ��������*/
unsigned long lastTimeIncrDecr;
int periodIncrDecr=100;					//	������ ������ ������ �� ��������� ��������
int incrGazWrite = 1;
int lastGazWrite;

/* �������� ����������*/
unsigned long lastTimeVoltage;	//	��������� ����������� ����������
int periodTimeVoltage=100;
int nomTestVoltage = 1;					//	����� ���������� ����������
int kolvoTestVoltage = 8;				//	���������� ����������� ��������
int voltage = 0;								//	������� �������� ��������
int workVoltage = 0;						//	������� ������������� �������� ��������
int lowVoltage=41;							//	�������� ��� ������� �������� ������� �������


// ��������� �������
//word addresLCD1602 = 0x27; // 0x3F;
word addresLCD1602 = 0x3F;
LiquidCrystal_I2C lcd(addresLCD1602, 16, 2); // ������ ����� � ����������� �������.
int markerVoltage=0;
unsigned long lastOutVoltage;						//	�������� ����� ������ �� �������
unsigned int periodOutVoltage=62500;		//	������ ������ ���������� �� ������� (� ���� ����)

struct LCDRusChar
{
	byte codRusCh;			//	��� ������� ����� ����� ������� 208
	byte* bykvaPoint;		//	������ �� ������������ ����������� �����
	uint8_t temNumber;	//	�������� ����������� �����
};

LCDRusChar aaa[sizeRusArray];	// ������ ������� ���������

/* ������ ��� �������� ����-�����*/
int nomLastNote = 0;						//	����� ��������� ����������� ������
unsigned int realTimeWork = 0;	//	�������� �������� ������� � �������
unsigned long lastFixTime;			//	��������� ��������������� �����



void setup() {
	Wire.setClock(31000);
	Wire.begin();
	lcd.init();	//	������������� lcd �������
	lcd.backlight();
	delay(1000);
	outLCD("Tulpar xIII", 0, 0); delay(700);
	outLCD("ver: 1.1.0 E", 0, 1); delay(1500);
//	lcd.noBacklight();	// ����� ������ - � 1.0.19� - �� �����


	TCCR0B = 0b00000001;  // x1
	TCCR0A = 0b00000011;  // fast pwm
	
	Serial.begin(9600);
	printf_begin();
	printf("������ \n");
	pinMode(pinOutVelocity, OUTPUT);
	lastTimeIncrDecr = millis();				//  ��������� ������� �����
	lastTimeVoltage = lastTimeIncrDecr;	//	� ��� ���������� ����������																									 
	lastOutVoltage=lastTimeIncrDecr;		//	��� �������
	lastGazWrite = minGazWrite;

	SearthLastRealTimeWork();	//	����� � ������������� ������������ �����

}

// the loop function runs over and over again until power down or reset
void loop() {
	unsigned long realTime = millis();
	if (realTime >= (64*60000)) {	//	���� ��� ������ ���� ������
		realTime = realTime - (64*60000);	//	������ ������
	}
	else {
		realTime = lastFixTime - 1;	//	���� ������ �� ������ - �� ���� ���� � ��������� ��������
	}
	if (lastFixTime <= realTime) {	//	������ ������
		realTimeWork++;								//	��������� �������� �����
		lastFixTime = millis();				//	������������� �����
		printf("�������� EEPROM ===================realTimeWork=%d \t lastFixTime=%d \n", realTimeWork, lastFixTime);
		saveEEPROM();	//	�������� � ������
	}

	driveGaz();
	loadVoltage();
//	printf ("voltage = %d \n", voltage);
	VoltageOutLCD();
}

/*
���������� �����
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
	if (lastTimeIncrDecr < (millis() - periodIncrDecr)) { //  ���� ���� ����� ����� �������� ��������
		if (lastGazWrite > normalGaz) { //  ���� �����������
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

/* ���������� �������� ����������*/
void loadVoltage() {
	if (lastTimeVoltage < (millis() - periodTimeVoltage)) {	//	���� ���� ��������� �������� ����������
		workVoltage = workVoltage + analogRead(pinVoltage);		//	�������� ��������� �������� � �������
		nomTestVoltage++;																			//	��������� �������� ��������
		if (nomTestVoltage > kolvoTestVoltage) {							//	������� ���������� ���-�� ���������
			nomTestVoltage = 1;
			voltage = (workVoltage / kolvoTestVoltage) / 17;	//	17 ��������� ����������������
			workVoltage = 0;
		}
		lastTimeVoltage = millis();
	}
}

/* ��������� ���������� */
void 	VoltageOutLCD() {
	// ��������� ���� �� �������� ����� ��������
	if (lastOutVoltage < (millis() - periodOutVoltage)) {
		lcd.setCursor(0,1);																	//	������� ��������
		outLCD("                ",0,1);
		outLCD("Volt:",0,1);
		lcd.print(voltage);
		lcd.print("V");
		outRealTime();	//	������� �������� ������������� �������
		if (voltage < lowVoltage) {													//	���� ���������� ������
			if (markerVoltage == 0) {													//	���� ������� �������
				lcd.backlight(); markerVoltage=1;
			} else {
				lcd.noBacklight(); markerVoltage=0;
			}
		} else {
//			lcd.noBacklight();
			lcd.backlight();	// ver 1.0.19E - �������� �������
		}

		lastOutVoltage=millis();														//	���������� ����� ��������� �����
	}

}

//	����� ��������� �������� ������ � ���-�� �����
void SearthLastRealTimeWork() {
	int nomSeartch = 0;	//	�������� � ������ ������
	printf("Nachali poisk \n");
	while (nomSeartch <= 1023) {
		uint8_t readTimeWork1 = EEPROM.read(nomSeartch);	//	�������� ������� ����
		nomSeartch++;																			//	��������� � ����������
		unsigned int readTimeWork = readTimeWork1 * 256 + EEPROM.read(nomSeartch);	//	��������� ����� ���-�� ������������ �����
		if (readTimeWork > realTimeWork) {	//	���������� � ���������� ���������
			nomLastNote = nomSeartch - 1;			//	���� ������ - ���������� ��� ������������
			realTimeWork = readTimeWork;			//	������ ��� ����������
		}
		nomSeartch++;	//	������������� 
	}
	printf("NomLastNote=%d \t realTimeWork=%u \n", nomLastNote, realTimeWork);
	lastFixTime = millis();	//	������������� ����� ��� ����������� ������� ������

}

/* ������ �������� ���������*/
void saveEEPROM() {
	int nomSave = nomLastNote + 2;	//	���������� � ��������� ������
	if (nomLastNote > 1023) {	//	���� ����� �� ����� - �������� � ������
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
	int Chas = realTimeWork / 60;	//	������ �������� �����
	int Minut = realTimeWork - Chas * 60;	//	���-�� �����
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
