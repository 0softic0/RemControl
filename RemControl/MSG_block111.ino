#define sizeRusArray 21

byte bykva_B[8] = { B11110,B10000,B10000,B11110,B10001,B10001,B11110,B00000, }; // ����� "�"
byte bykva_G[8] = { B11111,B10001,B10000,B10000,B10000,B10000,B10000,B00000, }; // ����� "�"
byte bykva_D[8] = { B01111,B00101,B00101,B01001,B10001,B11111,B10001,B00000, }; // ����� "�"
byte bykva_ZH[8] = { B10101,B10101,B10101,B11111,B10101,B10101,B10101,B00000, }; // ����� "�"
byte bykva_Z[8] = { B01110,B10001,B00001,B00010,B00001,B10001,B01110,B00000, }; // ����� "�"
byte bykva_I[8] = { B10001,B10011,B10011,B10101,B11001,B11001,B10001,B00000, }; // ����� "�"
byte bykva_IY[8] = { B01110,B00000,B10001,B10011,B10101,B11001,B10001,B00000, }; // ����� "�"
byte bykva_L[8] = { B00011,B00111,B00101,B00101,B01101,B01001,B11001,B00000, }; // ����� "�"
byte bykva_P[8] = { B11111,B10001,B10001,B10001,B10001,B10001,B10001,B00000, }; // ����� "�"
byte bykva_Y[8] = { B10001,B10001,B10001,B01010,B00100,B01000,B10000,B00000, }; // ����� "�"
byte bykva_F[8] = { B00100,B11111,B10101,B10101,B11111,B00100,B00100,B00000, }; // ����� "�"
byte bykva_TS[8] = { B10010,B10010,B10010,B10010,B10010,B10010,B11111,B00001, }; // ����� "�"
byte bykva_CH[8] = { B10001,B10001,B10001,B01111,B00001,B00001,B00001,B00000, }; // ����� "�"
byte bykva_Sh[8] = { B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00000, }; // ����� "�"
byte bykva_Shch[8] = { B10101,B10101,B10101,B10101,B10101,B10101,B11111,B00001, }; // ����� "�"
byte bykva_Mz[8] = { B10000,B10000,B10000,B11110,B10001,B10001,B11110,B00000, }; // ����� "�"
byte bykva_IYI[8] = { B10001,B10001,B10001,B11001,B10101,B10101,B11001,B00000, }; // ����� "�"
byte bykva_Tz[8] = { B11000,B01000,B01110,B01001,B01001,B01001,B01110,B00000, };	// ����� "�"
byte bykva_Ie[8] = { B01110,B10001,B00001,B00111,B00001,B10001,B01110,B00000, };	// ����� "�"
byte bykva_Yu[8] = { B10010,B10101,B10101,B11101,B10101,B10101,B10010,B00000, }; // ����� "�"
byte bykva_Ya[8] = { B01111,B10001,B10001,B01111,B00101,B01001,B10001,B00000, }; // ����� "�" 

/* ��������� ��� �������� ���������� � ������������ ������� �������� � �� ������� ��������� ������*/
struct LCDRusChar
{
	byte codRusCh;			//	��� ������� ����� ����� ������� 208
	byte* bykvaPoint;		//	������ �� ������������ ����������� �����
	uint8_t temNumber;	//	�������� ����������� �����

};
LCDRusChar aaa[sizeRusArray];	// ������ ������� ���������

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
	int kolvo = aaa.length();					//	���-�� ���� � ������
	for (int i = 0; i < kolvo; i++) {	//	���������� ��� �����
		char _symbol = aaa[i];
		byte _bSymbol = _symbol;
		Serial.println(_bSymbol);
		if (_bSymbol < 208) {	//	�������� ������
			//	����������� ��� � ������� ������� "a"	97; "A"	65; z 122 Z 90
			if ((_bSymbol >= 97) && (_bSymbol <= 122)) {	//	��������� �����
				_bSymbol = _bSymbol - 32;										//	��������� ��� �������������� ���������
			}
			char outSymbol = _bSymbol;
			outData = outData + outSymbol;
		}
		/*
		����������� �������� ��������.
		1. ��� ����� ������� �� ���� �������� 1 - 208 ��� 209; 2 - ��� �������
		2. ��� ��������� ����� ����� ������ ������ 208 �� 144-175 (����� � - 129)
		3. ��������� ������� �� ��� ���������
			�-� (208) 176-191
			�-� (209) 128-143
			� (209) 145
		������: ����� ������ - ���� ��� 208 ��� 209 - ������� �����. �������� ��������� - �� �������.
			��������� ������������ �����������.
		��� LCD - �������� ��������������� ������� � ���������, � ��� ������������ - ������ �������.
		*/
		if (_bSymbol == 208) {	// ������ �������� � ����� 208 (�-�, �-�?)
			outData = outData + aaa[i];
			i++;											//	�������� �� ���� ������ ���������
			char _rusSymbol = aaa[i];	//	����� ��� �������� �������
			byte _rusSymbolCod = _rusSymbol;
			if ((_rusSymbolCod >= 176) && (_rusSymbolCod <= 191)) {	//	���� ������ � ��������� �-� ��������� � ������� �������
				_rusSymbolCod = _rusSymbolCod - 32;
				_rusSymbol = _rusSymbolCod;
			}
			outData = outData + _rusSymbol;
		}
		if (_bSymbol == 209) {	// ������ �������� � ����� 209 (�-� � �?)
			char _upr = _bSymbol - 1;
			outData = outData + _upr;	//	��������� � �������� 208
			i++;											//	�������� �� ���� ������ ���������
			char _rusSymbol = aaa[i];	//	����� ��� �������� �������
			byte _rusSymbolCod = _rusSymbol;
			if ((_rusSymbolCod >= 128) && (_rusSymbolCod <= 143)) {	//	���� ������ � ���������  �-� (���-�) ��������� � ������� �������
				_rusSymbolCod = _rusSymbolCod + 32;
				_rusSymbol = _rusSymbolCod;
			}
			if (_rusSymbolCod == 145) {	//	���� ��������� "�"
				_rusSymbolCod = _rusSymbolCod - 16;
				_rusSymbol = _rusSymbolCod;
			}
			_rusSymbol = _rusSymbolCod;
			outData = outData + _rusSymbol;
		}
		if (i == kolvo) { i--; }	//	����������� ������� ��� ����� ������
	}

	/*
	�������� ������������ ��������������� ������
	*/
	String LCDoutData;
	kolvo = outData.length();
	for (size_t i = 0; i < kolvo; i++) {
		char _symbol = outData[i];
		byte _bSymbol = _symbol;
		if (_bSymbol < 208) {	// ���� �� ������� �����
			LCDoutData = LCDoutData + _symbol;
		}
		else {	// ���� �������� �������� ������� �����
			if (_bSymbol == 208) { // ���� ������� ����, ��� ����� ���� ������� �����
				i++;	//	����� ��������� ������
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

// ��������� ���������� ������ �������
char setTempRusChar(byte numChar) {
	// ���� ������������ ����� ���������� �������
	uint8_t maxNum = 0;
	uint8_t returnNum = 0;
	for (int i = 0; i < sizeRusArray; i++) {
		if (maxNum < aaa[i].temNumber) { maxNum = aaa[i].temNumber; }
	}

	// ���� ������� �� ������
	for (int i = 0; i < sizeRusArray; i++) {
		if (aaa[i].codRusCh == numChar) {	// �����
			if (aaa[i].temNumber > 0) {	//	����� ����������
				returnNum = aaa[i].temNumber;	//	����������� ����� �����������
			}
			else {		//	��������� ������ ���
				maxNum++;	//	����������� ����� ���� �� �������
				aaa[i].temNumber = maxNum;	//	��������� ���������
				returnNum = aaa[i].temNumber;
				lcd.createChar(returnNum, aaa[i].bykvaPoint);	//	������� ������
			}
		}
		else {
			returnNum = returnNum;
		}
	}
	char outChar = returnNum;
	return (outChar);
}

// ������� ��������� ������� ��������
void clearNumRusChar() {
	for (int i = 0; i < sizeRusArray; i++) {
		aaa[i].temNumber = 0;
	}
}

}