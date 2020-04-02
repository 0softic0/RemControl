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
 A0
 A1
 A2
 A3
 A4	-	LCD 1602	SDA
 A5	-	LCD 1602	SCL
 A6
 A7
 D01
 D02
 D03
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

 //#include "MessageLCD.h"
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
int signalPin = 8;	//	пин инициализации джойстика
int velPunPlus = 7;
int velPinMinus = 6;

Joy2_2axis myJoy;
X_Y_data workData;
ERROMdata workSaveData;	// переменная для хранения параметров джойстика

word addresLCD1602 = 0x38;
LiquidCrystal_I2C lcd(addresLCD1602, 16, 2); // Задаем адрес и размерность дисплея.
/*void MSG_privet();
void MSG_inicialization();
void MSG_centerOK();*/



// the setup function runs once when you press reset or power the board
void setup() {
	lcd.init();                     // Инициализация lcd дисплея
	lcd.backlight();                // Включение подсветки дисплея
	MSG_privet();
	delay(2000);
	MSG_inicialization();
	delay(2000);
	MSG_centerOK();
}

// the loop function runs over and over again until power down or reset
void loop() {

}
