/*
 Name:		ASCII_LCD1620.ino
 Created:	03.04.2020 6:20:57
 Author:	Анатолий
*/

#include <Wire.h>
#include "LiquidCrystal_I2C.h"    // Подключаем библиотеку LiquidCrystal_I2C
word addresLCD1602 = 0x38;
LiquidCrystal_I2C lcd(addresLCD1602, 16, 2); // Задаем адрес и размерность дисплея.
int lcdLight = 3;
struct symbolRUS {
	byte В[8] = { B11110,B10000,B10000,B11110,B10001,B10001,B11110,B00000, }; // Буква "Б"
	byte G[8] = { B11111,B10001,B10000,B10000,B10000,B10000,B10000,B00000, }; // Буква "Г"
	byte D[8] = { B01111,B00101,B00101,B01001,B10001,B11111,B10001,B00000, }; // Буква "Д"
	byte ZH[8] = { B10101,B10101,B10101,B11111,B10101,B10101,B10101,B00000, }; // Буква "Ж"
	byte Z[8] = { B01110,B10001,B00001,B00010,B00001,B10001,B01110,B00000, }; // Буква "З"
	byte I[8] = { B10001,B10011,B10011,B10101,B11001,B11001,B10001,B00000, }; // Буква "И"
	byte IY[8] = { B01110,B00000,B10001,B10011,B10101,B11001,B10001,B00000, }; // Буква "Й"
	byte L[8] = { B00011,B00111,B00101,B00101,B01101,B01001,B11001,B00000, }; // Буква "Л"
	byte P[8] = { B11111,B10001,B10001,B10001,B10001,B10001,B10001,B00000, }; // Буква "П"
	byte Y[8] = { B10001,B10001,B10001,B01010,B00100,B01000,B10000,B00000, }; // Буква "У"
	byte F[8] = { B00100,B11111,B10101,B10101,B11111,B00100,B00100,B00000, }; // Буква "Ф"
	byte TS[8] = { B10010,B10010,B10010,B10010,B10010,B10010,B11111,B00001, }; // Буква "Ц"
	byte CH[8] = { B10001,B10001,B10001,B01111,B00001,B00001,B00001,B00000, }; // Буква "Ч"
	byte Sh[8] = { B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00000, }; // Буква "Ш"
	byte Shch[8] = { B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00001, }; // Буква "Щ"
	byte Mz[8] = { B10000,B10000,B10000,B11110,B10001,B10001,B11110,B00000, }; // Буква "Ь"
	byte IYI[8] = { B10001,B10001,B10001,B11001,B10101,B10101,B11001,B00000, }; // Буква "Ы"
	byte Yu[8] = { B10010,B10101,B10101,B11101,B10101,B10101,B10010,B00000, }; // Буква "Ю"
	byte Ya[8] = { B01111,B10001,B10001,B01111,B00101,B01001,B10001,B00000, }; // Буква "Я"  

};


int incomingByte = 0;   // переменная для хранения байта данных, 
                        //принятых по последовательному интерфейсу

int s1 = 0;
int s2 = 0;
int SSS = 0;
byte bukva_ZZZZ[8] = { B11111,B00000,B11111,B11111,B11111,B11111,B11111,B00000, }; 
byte bukva_ZZZZ1[8] = { B11111,B00000,B11111,B11111,B11111,B00000,B11111,B00000, };

void setup() {
  Serial.begin(9600);     // открываем последовательный порт
                          // задаем скорость передачи данных 9600 бод
  lcd.init();	//	Инициализация lcd дисплея
	int s1 = 0;
	int s2 = 0;
	int SSS = 0;
	pinMode(3, OUTPUT);
	digitalWrite(3, HIGH);
	lcd.createChar(2, bukva_ZZZZ);
	byte aaa = 1;
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.write(2);
	lcd.print("====");
	lcd.print(2);
	delay(5000);
	lcd.createChar(2, bukva_ZZZZ1);
	lcd.setCursor(0, 1);
	lcd.write(2);
	lcd.print("====");
	lcd.print(2);
	delay(5000);

}
void loop() {
	if (Serial.available() > 0) {
		String aaa = Serial.readString();
		int bbb = aaa.length();
		Serial.println(bbb);
		for (int i = 0; i < bbb; i++) {
			char ccc = aaa[i];
			byte iii = ccc;
			Serial.print("==");
			Serial.print(iii);
			//Serial.print(aaa[i]);
		}
		Serial.println();
		Serial.println(aaa);
	}
	/*int SSS = 0;
	lcd.print(0);
	lcd.print(' ');
  for (int i = 0; i < 256; i++) {
		char my = i;
		lcd.print(my);
		SSS++;
		if (SSS == 10) {
		 
			delay(2000);
			lcd.clear();
      lcd.setCursor(0, 0);
			lcd.print(i);
			lcd.print(' ');
			SSS = 0;
			delay(500);
    }
		delay(100);
  }	*/
}
