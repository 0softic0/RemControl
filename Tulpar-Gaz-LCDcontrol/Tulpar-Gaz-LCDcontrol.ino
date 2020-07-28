/*
 Name:		ElectricGaz.ino
 Created:	12.05.2020 16:05:38
 Author:	��������

 ���������� ����������� ����� ����� ARDUINO
*/
#include <LiquidCrystal.h>
#include "printf.h"
#include <EEPROM.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>    // ���������� ���������� LiquidCrystal_I2C

/* �������� �����������
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

*/
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
int nomTestVoltage = 1;			//	����� ���������� ����������
int kolvoTestVoltage = 8;		//	���������� ����������� ��������
int voltage = 0;				//	������� �������� ��������
int workVoltage = 0;			//	������� ������������� �������� ��������
int lowVoltage=41;				//	�������� ��� ������� �������� ������� �������


// ��������� �������
word addresLCD1602 = 0x3F;
LiquidCrystal_I2C lcd(addresLCD1602, 16, 2); // ������ ����� � ����������� �������.
int markerVoltage=0;
unsigned long lastOutVoltage;	//	�������� ����� ������ �� �������
int periodOutVoltage=1000;		//	������ ������ ���������� �� �������

/* ��������� ��� �������� ���������� � ������������ ������� �������� � �� ������� ��������� ������*/
struct LCDRusChar
{
	byte codRusCh;			//	��� ������� ����� ����� ������� 208
	byte* bykvaPoint;		//	������ �� ������������ ����������� �����
	uint8_t temNumber;	//	�������� ����������� �����
};

LCDRusChar aaa[sizeRusArray];	// ������ ������� ���������

// the setup function runs once when you press reset or power the board
void setup() {
	lcd.init();	//	������������� lcd �������
//	outInicArray();	//	������������� ������� ������� ����
//	pinMode(pinLightLCD, OUTPUT);	//	������������� ���� ���������� ����������
//	digitalWrite(pinLightLCD, LOW); delay(1000);	//	�������� ���������
//	digitalWrite(pinLightLCD, HIGH); delay(1000);	//	�������� ���������
//	digitalWrite(pinLightLCD, LOW); delay(1000);	//	�������� ���������
//	digitalWrite(pinLightLCD, HIGH); delay(1000);	//	�������� ���������
//	delay(1000);
	lcd.backlight();
	delay(1000);
	outLCD("Tulpar xIII", 0, 0); delay(500);
	outLCD("ver: 1.0.17 E", 0, 1); delay(500);
	lcd.noBacklight();


	TCCR0B = 0b00000001;  // x1
	TCCR0A = 0b00000011;  // fast pwm
	
	Serial.begin(9600);
	printf_begin();
	pinMode(pinOutVelocity, OUTPUT);
	lastTimeIncrDecr = millis();				//  ��������� ������� �����
	lastTimeVoltage = lastTimeIncrDecr;	//	� ��� ���������� ����������
	lastOutVoltage=lastTimeIncrDecr;		//	��� �������
	lastGazWrite = minGazWrite;
	
	
}

// the loop function runs over and over again until power down or reset
void loop() {
	driveGaz();
	loadVoltage();
	printf ("voltage = %d \n", voltage);
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
	printf("%d \n", normalGaz);
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
		analogWrite(5, lastGazWrite);
	}
	//  analogWrite(5, normalGaz);
}

/* ���������� �������� ����������*/
void loadVoltage() {
	if (lastTimeVoltage < (millis() - periodTimeVoltage)) {	//	���� ���� ��������� �������� ����������
		workVoltage = workVoltage + analogRead(pinVoltage);	//	�������� ��������� �������� � �������
		nomTestVoltage++;	//	��������� �������� ��������
		if (nomTestVoltage > kolvoTestVoltage) {	//	������� ���������� ���-�� ���������
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
		lcd.setCursor(0,1);	//	������� ��������
		outLCD("                ",0,1);
		outLCD("Voltage:",0,1);
		lcd.print(voltage);
		if (voltage < lowVoltage) {	//	���� ���������� ������
			if (markerVoltage == 0) {	//	���� ������� �������
				lcd.backlight(); markerVoltage=1;
			} else {
				lcd.noBacklight(); markerVoltage=0;
			}
		} else {
			lcd.noBacklight();
		}

		lastOutVoltage=millis();	//	���������� ����� ��������� �����
	}

}
