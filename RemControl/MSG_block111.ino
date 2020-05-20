#define sizeRusArray 21

byte bykva_B[8] = { B11110,B10000,B10000,B11110,B10001,B10001,B11110,B00000, }; // Буква "Б"
byte bykva_G[8] = { B11111,B10001,B10000,B10000,B10000,B10000,B10000,B00000, }; // Буква "Г"
byte bykva_D[8] = { B01111,B00101,B00101,B01001,B10001,B11111,B10001,B00000, }; // Буква "Д"
byte bykva_ZH[8] = { B10101,B10101,B10101,B11111,B10101,B10101,B10101,B00000, }; // Буква "Ж"
byte bykva_Z[8] = { B01110,B10001,B00001,B00010,B00001,B10001,B01110,B00000, }; // Буква "З"
byte bykva_I[8] = { B10001,B10011,B10011,B10101,B11001,B11001,B10001,B00000, }; // Буква "И"
byte bykva_IY[8] = { B01110,B00000,B10001,B10011,B10101,B11001,B10001,B00000, }; // Буква "Й"
byte bykva_L[8] = { B00011,B00111,B00101,B00101,B01101,B01001,B11001,B00000, }; // Буква "Л"
byte bykva_P[8] = { B11111,B10001,B10001,B10001,B10001,B10001,B10001,B00000, }; // Буква "П"
byte bykva_Y[8] = { B10001,B10001,B10001,B01010,B00100,B01000,B10000,B00000, }; // Буква "У"
byte bykva_F[8] = { B00100,B11111,B10101,B10101,B11111,B00100,B00100,B00000, }; // Буква "Ф"
byte bykva_TS[8] = { B10010,B10010,B10010,B10010,B10010,B10010,B11111,B00001, }; // Буква "Ц"
byte bykva_CH[8] = { B10001,B10001,B10001,B01111,B00001,B00001,B00001,B00000, }; // Буква "Ч"
byte bykva_Sh[8] = { B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00000, }; // Буква "Ш"
byte bykva_Shch[8] = { B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00001, }; // Буква "Щ"
byte bykva_Mz[8] = { B10000,B10000,B10000,B11110,B10001,B10001,B11110,B00000, }; // Буква "Ь"
byte bykva_IYI[8] = { B10001,B10001,B10001,B11001,B10101,B10101,B11001,B00000, }; // Буква "Ы"
byte bykva_Tz[8] = { B11000,B01000,B01110,B01001,B01001,B01001,B01110,B00000, };	// Буква "Ъ"
byte bykva_Ie[8] = { B01110,B10001,B00001,B00111,B00001,B10001,B01110,B00000, };	// Буква "Э"
byte bykva_Yu[8] = { B10010,B10101,B10101,B11101,B10101,B10101,B10010,B00000, }; // Буква "Ю"
byte bykva_Ya[8] = { B01111,B10001,B10001,B01111,B00101,B01001,B10001,B00000, }; // Буква "Я" 

/* Структура для хранения информации о используемых русских символах и их текущем временном номере*/
struct LCDRusChar
{
	byte codRusCh;			//	код русской буквы после символа 208
	byte* bykvaPoint;		//	ссылка на попиксельное отображение буквы
	uint8_t temNumber;	//	временно присвоенный номер

};
LCDRusChar aaa[sizeRusArray];	// массив рабочей структуры

int i = 0;
aaa[i].codRusCh = 145; aaa[i].bykvaPoint = bykva_B; aaa[i].temNumber = 0; i++;
aaa[i].codRusCh = 147; aaa[i].bykvaPoint = bykva_G; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 148; aaa[i].bykvaPoint = bykva_D; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 150; aaa[i].bykvaPoint = bykva_ZH; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 151; aaa[i].bykvaPoint = bykva_Z; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 152; aaa[i].bykvaPoint = bykva_I; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 153; aaa[i].bykvaPoint = bykva_IY; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 155; aaa[i].bykvaPoint = bykva_L; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 159; aaa[i].bykvaPoint = bykva_P; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 163; aaa[i].bykvaPoint = bykva_Y; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 164; aaa[i].bykvaPoint = bykva_F; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 166; aaa[i].bykvaPoint = bykva_TS; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 167; aaa[i].bykvaPoint = bykva_CH; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 168; aaa[i].bykvaPoint = bykva_Sh; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 169; aaa[i].bykvaPoint = bykva_Shch; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 170; aaa[i].bykvaPoint = bykva_Tz; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 171; aaa[i].bykvaPoint = bykva_IYI; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 172; aaa[i].bykvaPoint = bykva_Mz; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 173; aaa[i].bykvaPoint = bykva_Ie; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 174; aaa[i].bykvaPoint = bykva_Yu; aaa[i].temNumber = 0;	i++;
aaa[i].codRusCh = 175; aaa[i].bykvaPoint = bykva_Ya; aaa[i].temNumber = 0;	i++;


void outLCD(String aaa, int _col, int _row) {
	String outData;
	int kolvo = aaa.length();					//	кол-во байт в строке
	for (int i = 0; i < kolvo; i++) {	//	перебираем все байты
		char _symbol = aaa[i];
		byte _bSymbol = _symbol;
		Serial.println(_bSymbol);
		if (_bSymbol < 208) {	//	латиница пришла
			//	преобразуем все в верхний регистр "a"	97; "A"	65; z 122 Z 90
			if ((_bSymbol >= 97) && (_bSymbol <= 122)) {	//	прописная буква
				_bSymbol = _bSymbol - 32;										//	вычмсляем код соответсвующей заглавной
			}
			char outSymbol = _bSymbol;
			outData = outData + outSymbol;
		}
		/*
		Особенность русского алфавита.
		1. Код буквы состоит из двух символов 1 - 208 или 209; 2 - код символа
		2. Все заглавные буквы имеют первый символ 208 от 144-175 (буква Ё - 129)
		3. Прописные делятся на три диапазона
			а-п (208) 176-191
			р-я (209) 128-143
			ё (209) 145
		ЛОГИКА: берем символ - если код 208 или 209 - русские буквы. Диапазон заглавных - не трогаем.
			Прописные обрабатываем диапазонами.
		Для LCD - проводим переконвертацию русских в латинские, а для отсутсвующих - делаем символы.
		*/
		if (_bSymbol == 208) {	// пришла кирилица с кодом 208 (А-Я, а-п?)
			outData = outData + aaa[i];
			i++;											//	сдвигаем на один символ указатель
			char _rusSymbol = aaa[i];	//	взяли код русского символа
			byte _rusSymbolCod = _rusSymbol;
			if ((_rusSymbolCod >= 176) && (_rusSymbolCod <= 191)) {	//	если символ в диапазоне а-п переводим в верхний регистр
				_rusSymbolCod = _rusSymbolCod - 32;
				_rusSymbol = _rusSymbolCod;
			}
			outData = outData + _rusSymbol;
		}
		if (_bSymbol == 209) {	// пришла кирилица с кодом 209 (р-я и ё?)
			char _upr = _bSymbol - 1;
			outData = outData + _upr;	//	перенесли в диапазон 208
			i++;											//	сдвигаем на один символ указатель
			char _rusSymbol = aaa[i];	//	взяли код русского символа
			byte _rusSymbolCod = _rusSymbol;
			if ((_rusSymbolCod >= 128) && (_rusSymbolCod <= 143)) {	//	если символ в диапазоне  р-я (без-ё) переводим в верхний регистр
				_rusSymbolCod = _rusSymbolCod + 32;
				_rusSymbol = _rusSymbolCod;
			}
			if (_rusSymbolCod == 145) {	//	если прописная "ё"
				_rusSymbolCod = _rusSymbolCod - 16;
				_rusSymbol = _rusSymbolCod;
			}
			_rusSymbol = _rusSymbolCod;
			outData = outData + _rusSymbol;
		}
		if (i == kolvo) { i--; }	//	компенсация сдвижки при конце строки
	}

	/*
	Начинаем обрабатывать нормализованную строку
	*/
	String LCDoutData;
	kolvo = outData.length();
	for (size_t i = 0; i < kolvo; i++) {
		char _symbol = outData[i];
		byte _bSymbol = _symbol;
		if (_bSymbol < 208) {	// если не русская буква
			LCDoutData = LCDoutData + _symbol;
		}
		else {	// если возможно получили русскую букву
			if (_bSymbol == 208) { // если признак того, что может быть русская буква
				i++;	//	берем следующий символ
				char _symbolRUS = outData[i];
				byte _bSymbolRus = _symbolRUS;
				switch (_bSymbolRus) {
				case 144:	LCDoutData = LCDoutData + 'A';	break;
				case 145:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 146:	LCDoutData = LCDoutData + 'B';	break;
				case 147:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 148:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 149:	LCDoutData = LCDoutData + 'E';	break;
				case 150:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 151:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 152:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 153:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 154:	LCDoutData = LCDoutData + 'K';	break;
				case 155:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 156:	LCDoutData = LCDoutData + 'M';	break;
				case 157:	LCDoutData = LCDoutData + 'H';	break;
				case 158:	LCDoutData = LCDoutData + 'O';	break;
				case 159:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 160:	LCDoutData = LCDoutData + 'P';	break;
				case 161:	LCDoutData = LCDoutData + 'C';	break;
				case 162:	LCDoutData = LCDoutData + 'T';	break;
				case 163:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 164:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 165:	LCDoutData = LCDoutData + 'X';	break;
				case 166:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 167:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 168:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 169:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 170: LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 171:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 172:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 173:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 174:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				case 175:	LCDoutData = LCDoutData + setTempRusChar(_bSymbolRus);	break;
				default:
					break;
				}
				if (i == kolvo) { i--; }
			}
		}
	}
	lcd.setCursor(_col, _row);
	lcd.print(LCDoutData);
	//	clearNumRusChar();
}

// установка временного номера символа
char setTempRusChar(byte numChar) {
	// ищем максимальный номер временного символа
	uint8_t maxNum = 0;
	uint8_t returnNum = 0;
	for (int i = 0; i < sizeRusArray; i++) {
		if (maxNum < aaa[i].temNumber) { maxNum = aaa[i].temNumber; }
	}

	// ищем элемент по номеру
	for (int i = 0; i < sizeRusArray; i++) {
		if (aaa[i].codRusCh == numChar) {	// нашли
			if (aaa[i].temNumber > 0) {	//	ранее встречался
				returnNum = aaa[i].temNumber;	//	присваиваем ранее встреченный
			}
			else {		//	встретили первый раз
				maxNum++;	//	увеличиваем номер кода на единицу
				aaa[i].temNumber = maxNum;	//	присвоили следующий
				returnNum = aaa[i].temNumber;
				lcd.createChar(returnNum, aaa[i].bykvaPoint);	//	создали символ
			}
		}
		else {
			returnNum = returnNum;
		}
	}
	char outChar = returnNum;
	return (outChar);
}

// очистка временных номеров символов
void clearNumRusChar() {
	for (int i = 0; i < sizeRusArray; i++) {
		aaa[i].temNumber = 0;
	}
}

}