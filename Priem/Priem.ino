﻿/******************************************************************************
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

/// описание пинов подключения
//constexpr auto = 0;
//constexpr auto = 1;
//constexpr auto = 2;
//constexpr auto lcdLight = 3;	//	управление подсветкой дисплея
//constexpr auto = 4;
constexpr auto pinNRFce = 5;
constexpr auto pinNRFcsn = 6;
//constexpr auto velPinMinus = 7;// 6;	//	снижение максимальной скорости
//constexpr auto velPinPlus = 8;// 7;		//	увеличение максимальной скорости
//constexpr auto signalPin = 9;// 8;		//	пин инициализации джойстика
//constexpr auto = 10;
//constexpr auto = 11;	NRF
//constexpr auto = 12;	NRF
//constexpr auto = 13;	NRF
constexpr auto startDrivers = 14;	//	A0
constexpr auto startCooler = 15;	//	A1
constexpr auto stopEngine = 16;	//	A2
//constexpr auto startCooler = 17;	//	A3
	//	18-A4		19-A5		20-A6
constexpr auto pinVolt = 21;	//	A7	подключенный через делитель на основное напряжение (??? В - ??? единиц)


RF24   radio(pinNRFce , pinNRFcsn);	//	Создаём объект radio для работы с библиотекой RF24, 
																		//	указывая номера выводов nRF24L01+ (CE, CSN)
int   sendData[2];		//	Создаём массив для приёма данных
											//	0 - левая гусеница,
											//	1 - праввая гусеница;
constexpr auto leftCat = 0;
constexpr auto rightCat = 1;

int	dataVoltage;			//	данные с датчика вольтажа


ManageDriveCaterpillar CaterpL(8, 10);
ManageDriveCaterpillar CaterpR(7, 9);

//int startDrivers = 14;  //  влючение драйверов
//int startCooler = 15;   //  включение вентилятора
//int stopEngine = 16;    //  глушение двигателя генератора

unsigned long lastDataTime;  //  время последнего удачного приема
unsigned long realDataTime;  //  текущее время если нет приема
unsigned long lastGENtime;   //  время старта генератора
unsigned long realGENtime;
unsigned  long  lastIncrData; //  время последнего инкремента для генератора
//int genSost = 0;              //  состояние генератора
//int PIN_REL = 3;  // управляющий канал на генератор
//int PIN_ANALOG_READ = A7; //  обратная связь (коэффициент подбирается под делитель)
//int Voltage = 0;  //  управляющее значение
//int VoltageMAX = 850; //  к какому значению стремимся



void setup() {
  // Пишем код для инициализации устройств(а), 
  // то что надо сделать один раз при включении
// устанавливаем частоту ШИМ для Таймера1 на 62,5 КГц
  TCCR1A = TCCR1A & 0xe0 | 1;
  TCCR1B = TCCR1B & 0xe0 | 0x09;
  //    sendData[0]=0;
  //    sendData[1]=0;
  Serial.begin(9600);
  printf_begin();
  pinMode(startDrivers, OUTPUT); digitalWrite(startDrivers, LOW);
  pinMode(startCooler, OUTPUT); digitalWrite(startCooler, LOW);
  pinMode(stopEngine, OUTPUT);  digitalWrite(stopEngine, HIGH);
  //pinMode(PIN_REL, OUTPUT);
  //digitalWrite(PIN_REL, LOW);

  // инициализация радио-модуля
  radio.begin();                                // Инициируем работу nRF24L01+
  radio.setChannel(120);                        // Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
  radio.setDataRate(RF24_250KBPS);           // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel(RF24_PA_MAX);         // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
//	radio.enableAckPayload();                                  // Указываем что в пакетах подтверждения приёма есть блок с пользовательскими данными.
	radio.openReadingPipe(1, 0x1234567890LL);    // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
//    radio.openReadingPipe (1, 0x1234567890LL);
  radio.startListening();                     // Включаем приемник, начинаем прослушивать открытую трубу
//  dataVoltage=analogRead(pinVolt);	//	считали значение напряжения 490 единиц - это 50,2 вольта
//	radio.writeAckPayload(1, &dataVoltage , sizeof(dataVoltage));
	delay(100);
  CaterpL.Inic();
  CaterpR.Inic();

}

void loop()
{
	dataVoltage=analogRead(pinVolt);
  if (radio.available()) {                                // Если в буфере имеются принятые данные
//    printf("Ect data SizeOF=%d \n",sizeof(sendData) );
    radio.read(&sendData, sizeof(sendData));            // Читаем данные в массив data и указываем сколько байт читать
//		radio.writeAckPayload(1, &dataVoltage, sizeof(dataVoltage));
//    printf("Lev = %d \t Prav=%d \n", sendData[0],sendData[1]);
    lastDataTime = millis();            //  запомнили время последнего удачного приема
    digitalWrite(startDrivers, HIGH); //  включаем драйвера
		printf("данные получены \n" );
  }
  else {
    realDataTime = millis();										//	запомнили время отсутстви данных в приеме
    if ((lastDataTime + 500) < realDataTime) {	//	если прошло больше 0,5 сек
      digitalWrite(startDrivers, LOW);					//	отключаем драйвера
      digitalWrite(startCooler, LOW);						//	останавливаем вертиляторы
      digitalWrite(stopEngine, HIGH);						//	глушим двигатель
//      digitalWrite(PIN_REL, LOW);								//	даем команду на прекращение генерации
//      genSost = 0;
//      sendData[2] = 0;
      sendData[leftCat] = 0;
      sendData[rightCat] = 0;
      printf("JOPAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA \n");
    }
    //    printf("NET \n");
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


  //  printf ("Normal C_L=%d \t C_R=%d", C_L, C_R);
  //  printf("_DATA C_L=%d \t C_R=%d \n", C_L, C_R);
    // блок замены данных от джойстика для более плавного трогания
  CaterpL.SetInputReadVelocity(C_L);
  CaterpR.SetInputReadVelocity(C_R);
  //  Serial.print("L");
  CaterpL.DriveCaterpillar();
  //  Serial.print("R");
  CaterpR.DriveCaterpillar();
  CaterpL.Run_Run();
  CaterpR.Run_Run();
  //  printf("GEN_SOST=%d \t sendWorkRelay=%d \n", genSost, sendWorkRelay);
}
