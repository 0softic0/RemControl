/*
	Имя:						DubleAxis.h
	Дата создания:	22.03.2020
	Автор:					WIN-J9AN17758SB\Анатолий
	------------------------------------------------------------------------------
	(000)
	Библиотека для работы с двухосевым джойстиком с двумя резисторами (прямым и 
	обратным) на каждой оси.
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
	Структура хранения основных параметров одного резистора оси
*/
struct BaseAxisResist
{
	int min;			//	значение МИНИМУМА
	int center;	//	значение ЦЕНТРА
	int max;			//	значенние МАКСИМУМА
	int error;		//	размер дребезга
};

/*
	Значения прямого и обратного резистора по одной оси
*/
struct F_B
{
	int F;	//	значение ПРЯМОГО резистора
	int B;	//	значение ОБРАТНОГО резистора
};

/*
	Значение двух осей джойстика
*/
struct F_B_2
{
	F_B X;	//	ось X
	F_B Y;	//	ось Y
};

/*
	Хранение данных от джойстика с информацией об ошибке
*/
struct X_Y_data
{
	int X;		//	значение по X
	int errX;	//	значение ошибки X
	int Y;		//	значение по Y
	int errY;	//	значние ошибки Y
};

/*
	Структура хранения основных параметров джойстика в энергонезависимой 
	памяти.
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
	Один резистор оси
*/
class AxisResist
{
public:
	AxisResist();
	~AxisResist();
	void setPin(int _pin);										//	инициализируем пин
	void setupBaseData(BaseAxisResist _data);	//	устанавливаем основные данные для резистора оси
																						//	рабочие функции
	void putData();	//	считать данные
	int getData();	//	получаем последние считанные данные
	int rawRead();	//	чтение в прямую, без изменения данных объекта

private:
	int pinA;									//	значение контакта
	BaseAxisResist BaseData;	//	базовые данные
	int RAWdata;							//	данные без преобразований последнего чтения
	int workData;							//	рабочие (преобразованные и нормализованные) данные
	void readRAWdata();				//	читаем данные с порта
};

/*
	Данные оси с двумя резисторами
*/
class DubleAxis
{
public:
	DubleAxis();
	~DubleAxis();
	void setDubleAxisPin(int _pinF, int _pinB);					//	установка пинов оси
	void setPram(BaseAxisResist _f, BaseAxisResist _b);	//	установка параметров оси
	void setSumParam(int _sum, int _err);								//	устанавливаем параментры и взаимоотношение резисторов по оси
	void readAxis();				//	считываем последние данные
	void calcAxis();				//	получаем данные с оси обработанные
	int getAxis();					//	получить вычисленные данные
	int getAxisErrorSize();	//	получить уровень ошибки
	F_B rawRead();					//	получаем данные оси без изменения объекта

private:
	int pinFront;	//	подключение прямого резистора
	int pinBack;	//	подключение обратного резистора
	int sumAxis;			//	сумма от двух резисторов
	int sumAxisError;	//	допустимая ошибка по сумме двух осей
	int axisData;			//	вычисленные данны по оси
	int errorSize;		//	уровень размера ошибки
	AxisResist frontAxis;	//	объект прямого резистора
	AxisResist backAxis;	//	объект обратного резистора
};

/*
	Двух осевой, двухрезистивный джойстик
*/
class Joy2_2axis
{
public:
	Joy2_2axis();
	~Joy2_2axis();
	void setPinAxis(	//	устанавливаем данные подсоединения пинов
		int _fX, int _bX,
		int _fY, int _bY);
	void setParam(		//	устанавливаем контрольные параметры осей джойстика
		BaseAxisResist _fX, BaseAxisResist _bX, int _sumX, int _errX,
		BaseAxisResist _fY, BaseAxisResist _bY, int _sumY, int _errY);
	void readData();	//	считать данные с осей
	void calcData();	//	произвести вычисления
	X_Y_data getData();	//	получаем значения
	F_B_2 rawRead();
	ERROMdata newInic(F_B_2 center, F_B_2 error, int signalPin);	//	инисциализация джойстика
	void readWorkCenter(F_B_2* _searthCenter, F_B_2* _minCenter, F_B_2* _maxCenter, F_B_2* _errAxis);	//	поиск центра
private:
	DubleAxis X;	// ось X
	DubleAxis Y;	// ось Y
	X_Y_data	work;	// данные джойстика
};

#endif

