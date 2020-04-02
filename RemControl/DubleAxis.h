/*
	���:						DubleAxis.h
	���� ��������:	22.03.2020
	�����:					WIN-J9AN17758SB\��������
	------------------------------------------------------------------------------
	(000)
	���������� ��� ������ � ���������� ���������� � ����� ����������� (������ � 
	��������) �� ������ ���.
	------------------------------------------------------------------------------
*/
// DubleAxis.h
// #include _PRINTF_h
// #include _EEPROM_h

#ifndef _DUBLEAXIS_h
#define _DUBLEAXIS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
/*
	��������� �������� �������� ���������� ������ ��������� ���
*/
struct BaseAxisResist
{
	int min;			//	�������� ��������
	int center;	//	�������� ������
	int max;			//	��������� ���������
	int error;		//	������ ��������
};

/*
	�������� ������� � ��������� ��������� �� ����� ���
*/
struct F_B
{
	int F;	//	�������� ������� ���������
	int B;	//	�������� ��������� ���������
};

/*
	�������� ���� ���� ���������
*/
struct F_B_2
{
	F_B X;	//	��� X
	F_B Y;	//	��� Y
};

/*
	�������� ������ �� ��������� � ����������� �� ������
*/
struct X_Y_data
{
	int X;		//	�������� �� X
	int errX;	//	�������� ������ X
	int Y;		//	�������� �� Y
	int errY;	//	������� ������ Y
};

/*
	��������� �������� �������� ���������� ��������� � ����������������� 
	������.
*/
struct ERROMdata {
	struct X {
		BaseAxisResist F;
		BaseAxisResist B;
	} X;
	struct Y {
		BaseAxisResist F;
		BaseAxisResist B;
	} Y;
	int SUMM_X, ERR_X, SUMM_Y, ERR_Y;
};

/*
	���� �������� ���
*/
class AxisResist
{
public:
	AxisResist();
	~AxisResist();
	void setPin(int _pin);										//	�������������� ���
	void setupBaseData(BaseAxisResist _data);	//	������������� �������� ������ ��� ��������� ���
																						//	������� �������
	void putData();	//	������� ������
	int getData();	//	�������� ��������� ��������� ������
	int rawRead();	//	������ � ������, ��� ��������� ������ �������

private:
	int pinA;									//	�������� ��������
	BaseAxisResist BaseData;	//	������� ������
	int RAWdata;							//	������ ��� �������������� ���������� ������
	int workData;							//	������� (��������������� � ���������������) ������
	void readRAWdata();				//	������ ������ � �����
};

/*
	������ ��� � ����� �����������
*/
class DubleAxis
{
public:
	DubleAxis();
	~DubleAxis();
	void setDubleAxisPin(int _pinF, int _pinB);					//	��������� ����� ���
	void setPram(BaseAxisResist _f, BaseAxisResist _b);	//	��������� ���������� ���
	void setSumParam(int _sum, int _err);								//	������������� ���������� � ��������������� ���������� �� ���
	void readAxis();				//	��������� ��������� ������
	void calcAxis();				//	�������� ������ � ��� ������������
	int getAxis();					//	�������� ����������� ������
	int getAxisErrorSize();	//	�������� ������� ������
	F_B rawRead();					//	�������� ������ ��� ��� ��������� �������

private:
	int pinFront;	//	����������� ������� ���������
	int pinBack;	//	����������� ��������� ���������
	int sumAxis;			//	����� �� ���� ����������
	int sumAxisError;	//	���������� ������ �� ����� ���� ����
	int axisData;			//	����������� ����� �� ���
	int errorSize;		//	������� ������� ������
	AxisResist frontAxis;	//	������ ������� ���������
	AxisResist backAxis;	//	������ ��������� ���������
};

/*
	���� ������, ��������������� ��������
*/
class Joy2_2axis
{
public:
	Joy2_2axis();
	~Joy2_2axis();
	void setPinAxis(	//	������������� ������ ������������� �����
		int _fX, int _bX,
		int _fY, int _bY);
	void setParam(		//	������������� ����������� ��������� ���� ���������
		BaseAxisResist _fX, BaseAxisResist _bX, int _sumX, int _errX,
		BaseAxisResist _fY, BaseAxisResist _bY, int _sumY, int _errY);
	void readData();	//	������� ������ � ����
	void calcData();	//	���������� ����������
	X_Y_data getData();	//	�������� ��������
	F_B_2 rawRead();
	ERROMdata newInic(F_B_2 center, F_B_2 error, int signalPin);	//	�������������� ���������
	void readWorkCenter(F_B_2* _searthCenter, F_B_2* _minCenter, F_B_2* _maxCenter, F_B_2* _errAxis);	//	����� ������
private:
	DubleAxis X;	// ��� X
	DubleAxis Y;	// ��� Y
	X_Y_data	work;	// ������ ���������
};

#endif

