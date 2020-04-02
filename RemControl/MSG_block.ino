
byte bukva_B[8] = { B11110,B10000,B10000,B11110,B10001,B10001,B11110,B00000, }; // Буква "Б"
byte bukva_G[8] = { B11111,B10001,B10000,B10000,B10000,B10000,B10000,B00000, }; // Буква "Г"
byte bukva_D[8] = { B01111,B00101,B00101,B01001,B10001,B11111,B10001,B00000, }; // Буква "Д"
byte bukva_ZH[8] = { B10101,B10101,B10101,B11111,B10101,B10101,B10101,B00000, }; // Буква "Ж"
byte bukva_Z[8] = { B01110,B10001,B00001,B00010,B00001,B10001,B01110,B00000, }; // Буква "З"
byte bukva_I[8] = { B10001,B10011,B10011,B10101,B11001,B11001,B10001,B00000, }; // Буква "И"
byte bukva_IY[8] = { B01110,B00000,B10001,B10011,B10101,B11001,B10001,B00000, }; // Буква "Й"
byte bukva_L[8] = { B00011,B00111,B00101,B00101,B01101,B01001,B11001,B00000, }; // Буква "Л"
byte bukva_P[8] = { B11111,B10001,B10001,B10001,B10001,B10001,B10001,B00000, }; // Буква "П"
byte bukva_Y[8] = { B10001,B10001,B10001,B01010,B00100,B01000,B10000,B00000, }; // Буква "У"
byte bukva_F[8] = { B00100,B11111,B10101,B10101,B11111,B00100,B00100,B00000, }; // Буква "Ф"
byte bukva_TS[8] = { B10010,B10010,B10010,B10010,B10010,B10010,B11111,B00001, }; // Буква "Ц"
byte bukva_CH[8] = { B10001,B10001,B10001,B01111,B00001,B00001,B00001,B00000, }; // Буква "Ч"
byte bukva_Sh[8] = { B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00000, }; // Буква "Ш"
byte bukva_Shch[8] = { B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00001, }; // Буква "Щ"
byte bukva_Mz[8] = { B10000,B10000,B10000,B11110,B10001,B10001,B11110,B00000, }; // Буква "Ь"
byte bukva_IYI[8] = { B10001,B10001,B10001,B11001,B10101,B10101,B11001,B00000, }; // Буква "Ы"
byte bukva_Yu[8] = { B10010,B10101,B10101,B11101,B10101,B10101,B10010,B00000, }; // Буква "Ю"
byte bukva_Ya[8] = { B01111,B10001,B10001,B01111,B00101,B01001,B10001,B00000, }; // Буква "Я"  


void MSG_privet() {
	lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("CTAPT");
	lcd.print("2020-04-01-001");
}

void MSG_inicialization() {
	lcd.clear();
	lcd.createChar(1, bukva_I);
	lcd.createChar(2, bukva_TS);
	lcd.createChar(3, bukva_L);
	lcd.createChar(4, bukva_Z);
	lcd.createChar(5, bukva_Ya);
	lcd.setCursor(1, 0);
	//	lcd.print("ИНИЦИАЛИЗАЦИЯ")
	lcd.print("\1H\1\2\1A\3\1\4A\2\1\5");
}

void MSG_centerOK() {
	lcd.clear();
	lcd.createChar(1, bukva_TS);
	lcd.setCursor(0, 0);
	lcd.print("ЦEHTP");
	lcd.print("\1EHTP");
	lcd.setCursor(1, 0);
	lcd.print("OK");
}
void MSG_joyCenter() {
	lcd.clear();
	// Джойстик в центре
	lcd.createChar(1, bukva_D);
	lcd.createChar(2, bukva_ZH);
	lcd.createChar(3, bukva_IY);
	lcd.createChar(4, bukva_I);
	lcd.createChar(5, bukva_TS);
	lcd.setCursor(0, 0);
	lcd.print("\1\2O\3CT\4K");
	lcd.setCursor(0, 1);
	lcd.print("B \5EHTPE");
}

void MSG_joyTest() {
	lcd.clear();
	// ТЕСТИРОВАНИЕ
	lcd.createChar(1, bukva_I);
	lcd.setCursor(0, 0);
	lcd.print("TECT\1POBAH\1E");
}
void MSG_readData() {
	// Читаем настройки
	lcd.clear();
	lcd.createChar(1, bukva_CH);
	lcd.createChar(2, bukva_I);
	lcd.createChar(3, bukva_IY);
	lcd.setCursor(0, 0);
	lcd.print("\1\2TAEM HACTPO\3K\2");
	lcd.setCursor(0, 1);
	for (size_t i = 0; i < 16; i++)
	{
		lcd.print("*"); delay(i*30);
	}
}