// 
// 
// 

#include "DubleAxis.h"
//#include "printf.h"
#include <EEPROM.h>

AxisResist::AxisResist()
{
}
AxisResist::~AxisResist()
{
}

// ������������� ����� ����������� ���������
void AxisResist::setPin(int _pin)
{ pinA = _pin;}

// ������������� ����������� ������ ���������: ������� ��������, �����, �������
void AxisResist::setupBaseData(BaseAxisResist _data)
{	BaseData = _data;}

// ������ ������ ��������� ���
void AxisResist::readRAWdata()
{	RAWdata = analogRead(pinA); }

/*
��������� ��������� �������� � ������ �������� !!!
� ���������� workData �������� ��������������� �������� - �� ������ ��������,
�� ������ ���������. � ������, ���� �������� ��������� � ��������� ������ - �����!
*/
void AxisResist::putData()
{
	readRAWdata();
	workData = RAWdata;
	if (RAWdata < BaseData.min) { workData = BaseData.min; }	//	�� ������ ��������
	if (RAWdata > BaseData.max) { workData = BaseData.max; }	//	�� ������ ���������
	// ���� �� ������ ������ ��� ������� - �� ��� �����
	int AAA = abs(BaseData.center - RAWdata);
	if (AAA < BaseData.error) {
		workData = BaseData.center;
	}
}

// ��������� ������������ ������
int AxisResist::getData()
{
	return workData;
}

// ������ ������ ������, ��� ��������� ���������� �������
int AxisResist::rawRead()
{
	return analogRead(pinA);
}


//============================= DubleAxis =================================
DubleAxis::DubleAxis() { }
DubleAxis::~DubleAxis() { }

// ������������� ����� ����������� ������� � ��������� ���������� ����� ���
void DubleAxis::setDubleAxisPin(int _pinF, int _pinB)
{
	pinFront = _pinF; pinBack = _pinB;
	frontAxis.setPin(pinFront);
	backAxis.setPin(pinBack);
}

// ������������� ��������� ������ � �������� ���
void DubleAxis::setPram(BaseAxisResist _f, BaseAxisResist _b)
{
	frontAxis.setupBaseData(_f);
	backAxis.setupBaseData(_b);
}

// ������������� ����� �������� ������� � ������� ���������� � ���������� 
// ������
void DubleAxis::setSumParam(int _sum, int _err)
{
	sumAxis = _sum;
	sumAxisError = _err;
}

// �������� ������ �� ���� ���������� ���
void DubleAxis::readAxis()
{
	frontAxis.putData();
	backAxis.putData();
}

// ������������ ������� �������� �� ��� � ������� ������
void DubleAxis::calcAxis()
{
	int FRONT = frontAxis.getData(); axisData = FRONT;
	int BACK = backAxis.getData();
	int SUM = FRONT + BACK;
	int dimention = abs(SUM - sumAxis);

	// ����������� ������ - ��������� �������� ����������� ����� � �������� � ������� �������
	//	��� ������ ��� �������� - ��� ����� ����������� �������� ��������
	if (dimention < sumAxisError) { errorSize = 0; }	//	��������� ������� ������
	else {
		dimention = dimention - sumAxisError;
		if (dimention < sumAxisError) { errorSize = 1; }
		else {
			dimention = dimention - sumAxisError;
			if (dimention < sumAxisError) { errorSize = 2; }
			else { errorSize = 3; }
		}
	}
}

// �������� �������� �� ���
int DubleAxis::getAxis()
{
	return axisData;
}

// �������� �������� ������
int DubleAxis::getAxisErrorSize()
{
	return errorSize;
}

// ������ ������ ������ ��� ��� ��������� ���������� ���������� �������
F_B DubleAxis::rawRead()
{
	F_B _work;
	_work.F = frontAxis.rawRead();
	_work.B = backAxis.rawRead();
	return _work;
}

//============================= Joy2_2axis =================================

Joy2_2axis::Joy2_2axis() { }
Joy2_2axis::~Joy2_2axis() { }

//	��������� ����������� ���������
void Joy2_2axis::setPinAxis(int _fX, int _bX, int _fY, int _bY)
{
	X.setDubleAxisPin(_fX, _bX);
	Y.setDubleAxisPin(_fY, _bY);
}

// ��������� ���������� ���������
void Joy2_2axis::setParam(
	BaseAxisResist _fX, BaseAxisResist _bX, int _sumX, int _errX, 
	BaseAxisResist _fY, BaseAxisResist _bY, int _sumY, int _errY)
{
	X.setPram(_fX, _bX);
	X.setSumParam(_sumX, _errX);
	Y.setPram(_fY, _bY);
	X.setSumParam(_sumY, _errY);
}

// ��������� ������ ����
void Joy2_2axis::readData()
{
	X.readAxis();
	Y.readAxis();
}

// ��������� �������� ���� ��� ������������ ������
void Joy2_2axis::calcData()
{
	X.calcAxis(); Y.calcAxis();
}

// �������� �������� ����
X_Y_data Joy2_2axis::getData()
{
	X_Y_data _work;
	_work.X = X.getAxis();
	_work.Y = Y.getAxis();
	_work.errX = X.getAxisErrorSize();
	_work.errY = Y.getAxisErrorSize();
	work = _work;

	return _work;
}

// ������ ������ ���� ��� ��������� ������ �������
F_B_2 Joy2_2axis::rawRead()
{
	F_B_2 _work;
	_work.X = X.rawRead();
	_work.Y = Y.rawRead();
	return _work;
}

ERROMdata Joy2_2axis::newInic(F_B_2 center, F_B_2 error, int signalPin)
{
	printf("���������������� \n");
	F_B_2 workSearth = rawRead();
	F_B_2 minData = workSearth;
	F_B_2 maxData = workSearth;
	int sumXmin = maxData.X.F + maxData.X.B; int sumXmax = sumXmin;
	int sumYmin = maxData.Y.F + maxData.Y.B; int sumYmax = sumYmin;
	while (digitalRead(signalPin) == LOW) { //	������� ������ � ������� ����������� � "�����" ������� ��������
		F_B_2 workSearth = rawRead();
		// �������� ������� ��������
		if ((workSearth.X.F + workSearth.X.B) > sumXmax) { sumXmax = workSearth.X.F + workSearth.X.B; }
		if ((workSearth.X.F + workSearth.X.B) < sumXmin) { sumXmin = workSearth.X.F + workSearth.X.B; }
		if ((workSearth.Y.F + workSearth.Y.B) > sumYmax) { sumYmax = workSearth.Y.F + workSearth.Y.B; }
		if ((workSearth.Y.F + workSearth.Y.B) < sumYmin) { sumYmin = workSearth.Y.F + workSearth.Y.B; }

		if (workSearth.X.F > maxData.X.F) { maxData.X.F = workSearth.X.F; }
		if (workSearth.X.F < minData.X.F) { minData.X.F = workSearth.X.F; }
		if (workSearth.X.B > maxData.X.B) { maxData.X.B = workSearth.X.B; }
		if (workSearth.X.B < minData.X.B) { minData.X.B = workSearth.X.B; }
		if (workSearth.Y.F > maxData.Y.F) { maxData.Y.F = workSearth.Y.F; }
		if (workSearth.Y.F < minData.Y.F) { minData.Y.F = workSearth.Y.F; }
		if (workSearth.Y.B > maxData.Y.B) { maxData.Y.B = workSearth.Y.B; }
		if (workSearth.Y.B < minData.Y.B) { minData.Y.B = workSearth.Y.B; }
	}
	//		printf("X �����  ���� = %d \t  ��� = %d \n", sumXmax, sumXmin);
	//		printf("Y �����  ���� = %d \t  ��� = %d \n", sumYmax, sumYmin);
				//	printf("Y ****** F min = %d \t max = %d \t\t B min = %d \t max = %d \n", minCenter.Y.F, maxCenter.Y.F, minCenter.Y.B, maxCenter.Y.B);
			// ������� ������ �� �������� � ������
	ERROMdata saveData;
	// ������� �� ����������� � ������������ �����
	saveData.SUMM_X = (sumXmax + sumXmin) / 2;
	saveData.SUMM_Y = (sumYmax + sumYmin) / 2;
	// ����������� ������������ � ������������� �������� �����
	saveData.ERR_X = (sumXmax - sumXmin);
	saveData.ERR_Y = (sumYmax - sumYmin);
	// ������� ���� �� ���� ����������
	saveData.X.F.max = maxData.X.F;	saveData.X.F.min = minData.X.F;
	saveData.X.B.max = maxData.X.B;	saveData.X.B.min = minData.X.B;
	saveData.Y.F.max = maxData.Y.F;	saveData.Y.F.min = minData.Y.F;
	saveData.Y.B.max = maxData.Y.B;	saveData.Y.B.min = minData.Y.B;
	// ������������ ������� ���� �� ���� ����������
	saveData.X.F.center = center.X.F;	saveData.X.B.center = center.X.B;
	saveData.Y.F.center = center.Y.F;	saveData.Y.B.center = center.Y.B;
	// ������� �� ���� ����������
	saveData.X.F.error = error.X.F; saveData.X.B.error = error.X.B;
	saveData.Y.F.error = error.Y.F; saveData.Y.B.error = error.Y.B;
	//
	// �������� ���� ������ � ������
	//
	// printERROM(saveData);
	EEPROM.put(0, saveData);
	return saveData;
}

void Joy2_2axis::readWorkCenter(F_B_2* _searthCenter, F_B_2* _minCenter, F_B_2* _maxCenter, F_B_2* _errAxis)
{
	for (int i = 1; i < 8; i++) {
		F_B_2 workSearthCenter = rawRead();
		//	��������� ��� ����������
		_searthCenter->X.F = _searthCenter->X.F + workSearthCenter.X.F;
		_searthCenter->X.B = _searthCenter->X.B + workSearthCenter.X.B;
		_searthCenter->Y.F = _searthCenter->Y.F + workSearthCenter.Y.F;
		_searthCenter->Y.B = _searthCenter->Y.B + workSearthCenter.Y.B;
		// �������� ������� ��������
		if (workSearthCenter.X.F > _maxCenter->X.F) { _maxCenter->X.F = workSearthCenter.X.F; }
		if (workSearthCenter.X.F < _minCenter->X.F) { _minCenter->X.F = workSearthCenter.X.F; }
		if (workSearthCenter.X.B > _maxCenter->X.B) { _maxCenter->X.B = workSearthCenter.X.B; }
		if (workSearthCenter.X.B < _minCenter->X.B) { _minCenter->X.B = workSearthCenter.X.B; }
		if (workSearthCenter.Y.F > _maxCenter->Y.F) { _maxCenter->Y.F = workSearthCenter.Y.F; }
		if (workSearthCenter.Y.F < _minCenter->Y.F) { _minCenter->Y.F = workSearthCenter.Y.F; }
		if (workSearthCenter.Y.B > _maxCenter->Y.B) { _maxCenter->Y.B = workSearthCenter.Y.B; }
		if (workSearthCenter.Y.B < _minCenter->Y.B) { _minCenter->Y.B = workSearthCenter.Y.B; }
	}
	// ���������
	_searthCenter->X.F = _searthCenter->X.F / 8; _searthCenter->X.B = _searthCenter->X.B / 8;
	_searthCenter->Y.F = _searthCenter->Y.F / 8; _searthCenter->Y.B = _searthCenter->Y.B / 8;
	// ���������� ������� (������)
	
	_errAxis->X.F = _maxCenter->X.F - _minCenter->X.F; _errAxis->X.B = _maxCenter->X.B - _minCenter->X.B;
	_errAxis->Y.F = _maxCenter->Y.F - _minCenter->Y.F; _errAxis->Y.B = _maxCenter->Y.B - _minCenter->Y.B;

}
