/*
 Name:		testLCD1602.ino
 Created:	21.07.2020 16:50:18
 Author:	AnPo
*/
#include <Wire.h> 
#include <LiquidCrystal_I2C.h> // ����������� ����������
//#include <LiquidCrystal_PCF8574.h> // ����������� �������������� ����������
LiquidCrystal_I2C lcd(0x3F, 16, 2); // ��������� I2C ����� (�������� ���������������� ��������), � ����� ��������� ������ (� ������ LCD 1602 - 2 ������ �� 16 �������� � ������ 
//LiquidCrystal_PCF8574 lcd(0x27); // ������� ��� ���������� PCF8574 
void setup()
{
	lcd.init();                      // ������������� �������  
	lcd.backlight();                 // ����������� ���������
	lcd.setCursor(0, 0);              // ��������� ������� � ������ ������ ������
	lcd.print("Hello������");       // ����� ������ �� ������ ������
	lcd.setCursor(0, 1);              // ��������� ������� � ������ ������ ������
	lcd.print("ArduinoMaster");       // ����� ������ �� ������ ������
}
void loop()
{
}