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

// устанавливаем точку подключения резистора
void AxisResist::setPin(int _pin)
{ pinA = _pin;}

// устанавливаем контрольные данные резистора: крайние значения, центр, дребезг
void AxisResist::setupBaseData(BaseAxisResist _data)
{	BaseData = _data;}

// чтение данных резистора оси
void AxisResist::readRAWdata()
{	RAWdata = analogRead(pinA); }

/*
Первичная обработка значений с учетом дребезга !!!
В переменной workData хнанится нормализованное значение - не меньше минимума,
не больше максимума. В случае, если значение находится в диапазоне ошибки - центр!
*/
void AxisResist::putData()
{
	readRAWdata();
	workData = RAWdata;
	if (RAWdata < BaseData.min) { workData = BaseData.min; }	//	не меньше минимума
	if (RAWdata > BaseData.max) { workData = BaseData.max; }	//	не больше максимума
	// если до центра меньше чем дребезг - то это центр
	int AAA = abs(BaseData.center - RAWdata);
	if (AAA < BaseData.error) {
		workData = BaseData.center;
	}
}

// получение обработанных данных
int AxisResist::getData()
{
	return workData;
}

// прямое чтение данных, без изменения переменных объекта
int AxisResist::rawRead()
{
	return analogRead(pinA);
}


//============================= DubleAxis =================================
DubleAxis::DubleAxis() { }
DubleAxis::~DubleAxis() { }

// устанавливаем точки подкличения прямого и обратного резисторов одной оси
void DubleAxis::setDubleAxisPin(int _pinF, int _pinB)
{
	pinFront = _pinF; pinBack = _pinB;
	frontAxis.setPin(pinFront);
	backAxis.setPin(pinBack);
}

// устанавливаем параметры прямой и обратной оси
void DubleAxis::setPram(BaseAxisResist _f, BaseAxisResist _b)
{
	frontAxis.setupBaseData(_f);
	backAxis.setupBaseData(_b);
}

// устанавливаем сумму значений прямого и обратно резисторов и допустимую 
// ошибку
void DubleAxis::setSumParam(int _sum, int _err)
{
	sumAxis = _sum;
	sumAxisError = _err;
}

// получаем данные по двум резисторам оси
void DubleAxis::readAxis()
{
	frontAxis.putData();
	backAxis.putData();
}

// утанавливаем рабочее значение по оси и уровень ошибки
void DubleAxis::calcAxis()
{
	int FRONT = frontAxis.getData(); axisData = FRONT;
	int BACK = backAxis.getData();
	int SUM = FRONT + BACK;
	int dimention = abs(SUM - sumAxis);

	// коэффициент ошибки - кратность текущего расходжения суммы и принятой в объекте ошибкой
	//	чем больше это значение - тем более некорректно работает джойстик
	if (dimention < sumAxisError) { errorSize = 0; }	//	фиксируем уровень ошибки
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

// получить значение по оси
int DubleAxis::getAxis()
{
	return axisData;
}

// получить значение ошибки
int DubleAxis::getAxisErrorSize()
{
	return errorSize;
}

// прямое чтение данных оси без изменения внутренних переменных объекта
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

//	установка подключения джойстика
void Joy2_2axis::setPinAxis(int _fX, int _bX, int _fY, int _bY)
{
	X.setDubleAxisPin(_fX, _bX);
	Y.setDubleAxisPin(_fY, _bY);
}

// установка параметров джойстика
void Joy2_2axis::setParam(
	BaseAxisResist _fX, BaseAxisResist _bX, int _sumX, int _errX, 
	BaseAxisResist _fY, BaseAxisResist _bY, int _sumY, int _errY)
{
	X.setPram(_fX, _bX);
	X.setSumParam(_sumX, _errX);
	Y.setPram(_fY, _bY);
	X.setSumParam(_sumY, _errY);
}

// считываем данные осей
void Joy2_2axis::readData()
{
	X.readAxis();
	Y.readAxis();
}

// вычисляем значения осей для нормализации данных
void Joy2_2axis::calcData()
{
	X.calcAxis(); Y.calcAxis();
}

// получаем значения осей
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

// прямое чтение осей без изменения данных объекта
F_B_2 Joy2_2axis::rawRead()
{
	F_B_2 _work;
	_work.X = X.rawRead();
	_work.Y = Y.rawRead();
	return _work;
}

ERROMdata Joy2_2axis::newInic(F_B_2 center, F_B_2 error, int signalPin)
{
	printf("ИНИЦИАЛИЗИРУЕМСЯ \n");
	F_B_2 workSearth = rawRead();
	F_B_2 minData = workSearth;
	F_B_2 maxData = workSearth;
	int sumXmin = maxData.X.F + maxData.X.B; int sumXmax = sumXmin;
	int sumYmin = maxData.Y.F + maxData.Y.B; int sumYmax = sumYmin;
	while (digitalRead(signalPin) == LOW) { //	двигаем ручкой в крайние направления и "пасем" крайние значения
		F_B_2 workSearth = rawRead();
		// выбираем крайние значения
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
	//		printf("X сумма  МАКС = %d \t  МИН = %d \n", sumXmax, sumXmin);
	//		printf("Y сумма  МАКС = %d \t  МИН = %d \n", sumYmax, sumYmin);
				//	printf("Y ****** F min = %d \t max = %d \t\t B min = %d \t max = %d \n", minCenter.Y.F, maxCenter.Y.F, minCenter.Y.B, maxCenter.Y.B);
			// готовим данные на передачу в память
	ERROMdata saveData;
	// среднее от минимальной и максимальной суммы
	saveData.SUMM_X = (sumXmax + sumXmin) / 2;
	saveData.SUMM_Y = (sumYmax + sumYmin) / 2;
	// расхождение минимального и максимального значений суммы
	saveData.ERR_X = (sumXmax - sumXmin);
	saveData.ERR_Y = (sumYmax - sumYmin);
	// границы осей по двум резисторам
	saveData.X.F.max = maxData.X.F;	saveData.X.F.min = minData.X.F;
	saveData.X.B.max = maxData.X.B;	saveData.X.B.min = minData.X.B;
	saveData.Y.F.max = maxData.Y.F;	saveData.Y.F.min = minData.Y.F;
	saveData.Y.B.max = maxData.Y.B;	saveData.Y.B.min = minData.Y.B;
	// расположение центров осей по двум резисторам
	saveData.X.F.center = center.X.F;	saveData.X.B.center = center.X.B;
	saveData.Y.F.center = center.Y.F;	saveData.Y.B.center = center.Y.B;
	// дребезг по двум резисторам
	saveData.X.F.error = error.X.F; saveData.X.B.error = error.X.B;
	saveData.Y.F.error = error.Y.F; saveData.Y.B.error = error.Y.B;
	//
	// вставить блок записи в память
	//
	// printERROM(saveData);
	EEPROM.put(0, saveData);
	return saveData;
}

void Joy2_2axis::readWorkCenter(F_B_2* _searthCenter, F_B_2* _minCenter, F_B_2* _maxCenter, F_B_2* _errAxis)
{
	for (int i = 1; i < 8; i++) {
		F_B_2 workSearthCenter = rawRead();
		//	суммируем для усреднения
		_searthCenter->X.F = _searthCenter->X.F + workSearthCenter.X.F;
		_searthCenter->X.B = _searthCenter->X.B + workSearthCenter.X.B;
		_searthCenter->Y.F = _searthCenter->Y.F + workSearthCenter.Y.F;
		_searthCenter->Y.B = _searthCenter->Y.B + workSearthCenter.Y.B;
		// выбираем крайние значения
		if (workSearthCenter.X.F > _maxCenter->X.F) { _maxCenter->X.F = workSearthCenter.X.F; }
		if (workSearthCenter.X.F < _minCenter->X.F) { _minCenter->X.F = workSearthCenter.X.F; }
		if (workSearthCenter.X.B > _maxCenter->X.B) { _maxCenter->X.B = workSearthCenter.X.B; }
		if (workSearthCenter.X.B < _minCenter->X.B) { _minCenter->X.B = workSearthCenter.X.B; }
		if (workSearthCenter.Y.F > _maxCenter->Y.F) { _maxCenter->Y.F = workSearthCenter.Y.F; }
		if (workSearthCenter.Y.F < _minCenter->Y.F) { _minCenter->Y.F = workSearthCenter.Y.F; }
		if (workSearthCenter.Y.B > _maxCenter->Y.B) { _maxCenter->Y.B = workSearthCenter.Y.B; }
		if (workSearthCenter.Y.B < _minCenter->Y.B) { _minCenter->Y.B = workSearthCenter.Y.B; }
	}
	// усредняем
	_searthCenter->X.F = _searthCenter->X.F / 8; _searthCenter->X.B = _searthCenter->X.B / 8;
	_searthCenter->Y.F = _searthCenter->Y.F / 8; _searthCenter->Y.B = _searthCenter->Y.B / 8;
	// определяем дребезг (ошибку)
	
	_errAxis->X.F = _maxCenter->X.F - _minCenter->X.F; _errAxis->X.B = _maxCenter->X.B - _minCenter->X.B;
	_errAxis->Y.F = _maxCenter->Y.F - _minCenter->Y.F; _errAxis->Y.B = _maxCenter->Y.B - _minCenter->Y.B;

}
