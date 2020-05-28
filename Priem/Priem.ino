/******************************************************************************
 Priem - приемная часть для управления гусеницами BOT-REX
 Project			:	Приемник
 Libraries		:
 Author				:	SoftIC (0softic0@gmail.com)
 Description	:	Прием данных, управление драйверами двигателей,
								вентиляторами и генератором (если получится)
								Данные должны одинаково обрабатываться на передатчике
								и приемнике.
------------------------------------------------------------------
(2020-04-09 001)
Адаптация системы приема данных от BOT-REX 005 версии
------------------------------------------------------------------
2019-12-09 - (001)
1.  Модуль реле подключен на A1-A4 (в цифровом формате).
	А1 - запуск контроллеров двигателей; (HIGH - включено)
*  А2 - реле вентилятора охолождения; (HIGH - включено)
	А3 - старт генератора;  (HIGH - включено)
	А4 - стоп генератора (LOW - включено)
	в выключенном состоянии реле находятся в состоянии LOW.
	для запуска двигателя и его работы необходимо А4 перевести в HIGH
	и удерживать это состояние до его останова.
2.  Изменение структуры передаваемых данных
	Массив состоит из 3-х элементов. Третий элемент определяет что делать
	M3(0) - команды движения M0 - скорость девой гусеницы M1-правой
	M3(1) - команды запуска и останова генератора M0(1) - запуск, M0(2) - стоп
-------------------------------------------------------------------------------
*******************************************************************************/
// Подключаем библиотеки
#include <SPI.h>      //  Подключаем библиотеку для работы с шиной SPI
#include <nRF24L01.h> //  Подключаем файл настроек из библиотеки RF24
#include <RF24.h>     //  Подключаем библиотеку для работы с nRF24L01+
#include "printf.h"
#include "MDC_V001.h" //  подключаем библиотеку управления гусеницей
#include <OneWire.h>	//	библиотека для связи с датчиком температуры

/// описание пинов подключения
//constexpr auto = 0;
//constexpr auto = 1;
constexpr auto pinTemperature = 2;	//	подключение детчика температуры DS18b20
//constexpr auto = 3;
//constexpr auto = 4;
constexpr auto pinNRFce = 5;
constexpr auto pinNRFcsn = 6;
constexpr auto naprCatR = 7;			//	направление Правой Гусеницы
constexpr auto naprCatL = 8;			//	направление Левой Гусеницы
constexpr auto velocityCatR = 9;	//	скорость Правой гусеницы
constexpr auto velocityCatL = 10;	//	скорость Левой гусеницы
//constexpr auto = 11;	NRF
//constexpr auto = 12;	NRF
//constexpr auto = 13;	NRF
constexpr auto startDrivers = 14;	//	реле включения драйверов
constexpr auto startCooler = 15;	//	реле влючения вентилятора
constexpr auto stopEngine = 16;		//	реле на глушку двигателя генератора
//constexpr auto = 17;
	//	18-A4		19-A5		20-A6
constexpr auto pinVolt = 21;	//	A7	подключенный через делитель на основное напряжение (??? В - ??? единиц)

OneWire Temperature(pinTemperature);						//	Создаем объект OneWire для шины 1-Wire, с помощью которого будет
																								//	осуществляться работа с датчиком
boolean sostTemperatureOpros;										//	состояние опроса датчика температуры (False - не опрашивали, 
																								//	True - находимся в состоянии опроса
unsigned long lastTemperatureControl;						//	время последнего контроля температуры
unsigned long repeatTemperatureControl = 3000;	//	переодичность контроля температуры
unsigned long timeOutTemperatureData = 1000;		//	таймаут от запроса до готовности данных
byte dataTemperatureRAW[2];											//	Место для значения температуры
int dataTemperature;														//	Температура


RF24   radio(pinNRFce , pinNRFcsn);	//	Создаём объект radio для работы с библиотекой RF24, 
																		//	указывая номера выводов nRF24L01+ (CE, CSN)
int   sendData[2];		//	Создаём массив для приёма данных
											//	0 - левая гусеница,
											//	1 - праввая гусеница;
unsigned long outData;
constexpr auto leftCat = 0;
constexpr auto rightCat = 1;

int	dataVoltage;			//	данные с датчика вольтажа


ManageDriveCaterpillar CaterpL(naprCatL,velocityCatL);
ManageDriveCaterpillar CaterpR(naprCatR,velocityCatR);

unsigned long lastDataTime;  //  время последнего удачного приема
unsigned long realDataTime;  //  текущее время если нет приема


void setup() {
	//	Пишем код для инициализации устройств(а), 
	//	то что надо сделать один раз при включении
	//	устанавливаем частоту ШИМ для Таймера1 на 62,5 КГц
	TCCR1A = TCCR1A & 0xe0 | 1;
	TCCR1B = TCCR1B & 0xe0 | 0x09;
	Serial.begin(9600);
	printf_begin();
	pinMode(startDrivers, OUTPUT); digitalWrite(startDrivers, LOW);
	pinMode(startCooler, OUTPUT); digitalWrite(startCooler, LOW);
	pinMode(stopEngine, OUTPUT);  digitalWrite(stopEngine, HIGH);

	// инициализация радио-модуля
	inicRadio();
	inicTemperature();
	CaterpL.Inic();
	CaterpR.Inic();

}

void loop()
{
	dataVoltage=analogRead(pinVolt);
	if (radio.available()) {																				//	Если в буфере имеются принятые данные
		radio.read(&sendData, sizeof(sendData));											//	Читаем данные в массив data и указываем сколько байт читать
		outData=millis();
		printf("voltage=%d \n", dataVoltage);
		radio.writeAckPayload(1, &dataVoltage, sizeof(dataVoltage));	//	Отправляем значение бортового напряжения
		lastDataTime = millis();																			//	запомнили время последнего удачного приема
		digitalWrite(startDrivers, HIGH);															//	включаем драйвера
		printf("данные получены \n" );
	}
	else {
		realDataTime = millis();										//	запомнили время отсутстви данных в приеме
		if ((lastDataTime + 500) < realDataTime) {	//	если прошло больше 0,5 сек
			digitalWrite(startDrivers, LOW);					//	отключаем драйвера
			digitalWrite(startCooler, LOW);						//	останавливаем вертиляторы
			digitalWrite(stopEngine, HIGH);						//	глушим двигатель
			sendData[leftCat] = 0;
			sendData[rightCat] = 0;
			printf("JOPAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA \n");
		}
	}
	int C_L = sendData[leftCat];
	int C_R = sendData[rightCat];
		printf ("C_L=%d \t C_R=%d", C_L, C_R);
	C_L = constrain(C_L, -1024, 1024);      // нормализуем до максимального значения при прямом ходе
	C_R = constrain(C_R, -1024, 1024);

	// Блок удаления пустых значений, когда контроллер не реагирует
	if (C_L > 0) { C_L = map(C_L, 0, 1024, 40, 180); }
	else { C_L = map(C_L, 0, -1024, -40, -180); }
	if (C_R > 0) { C_R = map(C_R, 0, 1024, 40, 180); }
	else { C_R = map(C_R, 0, -1024, -40, -180); }


	analiticTemperature();
		// блок замены данных от джойстика для более плавного трогания
	CaterpL.SetInputReadVelocity(C_L);
	CaterpR.SetInputReadVelocity(C_R);
	//  Serial.print("L");
	CaterpL.DriveCaterpillar();
	//  Serial.print("R");
	CaterpR.DriveCaterpillar();
	CaterpL.Run_Run();
	CaterpR.Run_Run();
	
}

/* 
Инициализация радио модуля 
*/
void inicRadio() {
	// инициализация радио-модуля
	radio.begin();														// Инициируем работу nRF24L01+
	radio.setChannel(120);										// Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
	radio.setDataRate(RF24_250KBPS);					// Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
	radio.setPALevel(RF24_PA_MAX);						// Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
	radio.enableAckPayload();									// Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
	radio.openReadingPipe(1, 0x1234567890LL);	// Открываем 1 трубу с идентификатором 0x1234567890 для приема данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
//    radio.openReadingPipe (1, 0x1234567890LL);
	radio.startListening();										// Включаем приемник, начинаем прослушивать открытую трубу
//  dataVoltage=analogRead(pinVolt);	//	считали значение напряжения 490 единиц - это 50,2 вольта
	radio.writeAckPayload(1, &outData, sizeof(outData));
	delay(200);

}
/*
Инициализация датчика температуры - формирование первого запроса
*/
void inicTemperature() {
	Temperature.reset();							//	Начинаем взаимодействие со сброса всех предыдущих команд и параметров
	Temperature.write(0xCC);					//	Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство 
	Temperature.write(0x44);					//	Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
	lastTemperatureControl=millis();	//	Запомнили когда српосили данные
	sostTemperatureOpros=true;				//	перевели в состояние ожидания готовности данных
}

/*
Получение данных от датчика температуры
*/
void readDataTemperature() {
	//printf("Зашли \n");
	if (sostTemperatureOpros) {	//	состояние ожидания готовности данных
		if ((lastTemperatureControl + timeOutTemperatureData) < millis()) {	//	если данные готовы
			//printf("данные готовы \n");
			Temperature.reset();			//	Теперь готовимся получить значение измеренной температуры
			Temperature.write(0xCC);
			Temperature.write(0xBE);	//	Просим передать нам значение регистров со значением температуры
			// Получаем и считываем ответ
			dataTemperatureRAW[0] = Temperature.read();	// Читаем младший байт значения температуры
			dataTemperatureRAW[1] = Temperature.read();	// А теперь старший
			dataTemperature = ((dataTemperatureRAW[1] << 8) | dataTemperatureRAW[0]);
			sostTemperatureOpros=false;
			lastTemperatureControl=millis();
		}
	}
	else {	//	если данные не ожидаем, проверяем сколько прошло времени от последнего запроса
		if ((lastTemperatureControl + repeatTemperatureControl) < millis()) {	//	пора запрашивать по новой
			//printf ("Пора спросить \n");
			Temperature.reset();							//	Начинаем взаимодействие со сброса всех предыдущих команд и параметров
			Temperature.write(0xCC);					// Даем датчику DS18b20 команду пропустить поиск по адресу. В нашем случае только одно устрйоство 
			Temperature.write(0x44);					// Даем датчику DS18b20 команду измерить температуру. Само значение температуры мы еще не получаем - датчик его положит во внутреннюю память
			lastTemperatureControl = millis();	//	Запомнили когда српосили данные
			sostTemperatureOpros = true;				//	перевели в состояние ожидания готовности данных
		}
	}
}
void analiticTemperature() {
	readDataTemperature();	//	проверили температуту
	if (dataTemperature > 500) {	//	пора включать вентилятор
		digitalWrite(startCooler, HIGH);
	}
	else {	//	выключаем вентилятор
		digitalWrite(startCooler, LOW);
	}
	if (dataTemperature > 1000) {	//	останавливаем драйвера
		digitalWrite(startDrivers, LOW);
		sendData[leftCat] = 0;
		sendData[rightCat] = 0;
	}
}