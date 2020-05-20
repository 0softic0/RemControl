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
RF24   radio(5, 6);   //  Создаём объект radio для работы с библиотекой RF24, 
                      //    указывая номера выводов nRF24L01+ (CE, CSN)
int   sendData[3];    //  Создаём массив для приёма данных 0 - левая гусеница,
                      //  1 - праввая гусеница; 2 - состояние двигателя (1-запущен; 0 - остановлен)


ManageDriveCaterpillar CaterpL(8, 10);
ManageDriveCaterpillar CaterpR(7, 9);

int startDrivers = 14;  //  влючение драйверов
int startCooler = 15;   //  включение вентилятора
int stopEngine = 16;    //  глушение двигателя генератора

unsigned long lastDataTime;  //  время последнего удачного приема
unsigned long realDataTime;  //  текущее время если нет приема
unsigned long lastGENtime;   //  время старта генератора
unsigned long realGENtime;
unsigned  long  lastIncrData; //  время последнего инкремента для генератора
int genSost = 0;              //  состояние генератора
int PIN_REL = 3;  // управляющий канал на генератор
int PIN_ANALOG_READ = A7; //  обратная связь (коэффициент подбирается под делитель)
int Voltage = 0;  //  управляющее значение
int VoltageMAX = 850; //  к какому значению стремимся



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
  pinMode(PIN_REL, OUTPUT);
  digitalWrite(PIN_REL, LOW);

  // инициализация радио-модуля
  radio.begin();                                // Инициируем работу nRF24L01+
  radio.setChannel(120);                        // Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
  radio.setDataRate(RF24_250KBPS);           // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
  radio.setPALevel(RF24_PA_MAX);         // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
  radio.openReadingPipe(1, 0x1234567890LL);    // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
//    radio.openReadingPipe (1, 0x1234567890LL);
  radio.startListening();                     // Включаем приемник, начинаем прослушивать открытую трубу
  delay(100);
  CaterpL.Inic();
  CaterpR.Inic();

}

void loop()
{
  if (radio.available()) {                                // Если в буфере имеются принятые данные
//    printf("Ect data SizeOF=%d \n",sizeof(sendData) );
    radio.read(&sendData, sizeof(sendData));            // Читаем данные в массив data и указываем сколько байт читать
//    printf("Lev = %d \t Prav=%d \n", sendData[0],sendData[1]);
    lastDataTime = millis();            //  запомнили время последнего удачного приема
    digitalWrite(startDrivers, HIGH); //  включаем драйвера
  }
  else {
    realDataTime = millis();										//	запомнили время отсутстви данных в приеме
    if ((lastDataTime + 500) < realDataTime) {	//	если прошло больше 0,5 сек
      digitalWrite(startDrivers, LOW);					//	отключаем драйвера
      digitalWrite(startCooler, LOW);						//	останавливаем вертиляторы
      digitalWrite(stopEngine, HIGH);						//	глушим двигатель
      digitalWrite(PIN_REL, LOW);								//	даем команду на прекращение генерации
      genSost = 0;
      sendData[2] = 0;
      sendData[1] = 0;
      sendData[0] = 0;
      printf("JOPAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA \n");
    }
    //    printf("NET \n");
  }
  int C_L = sendData[0];
  int C_R = sendData[1];
  //  printf ("C_L=%d \t C_R=%d", C_L, C_R);
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
  /*------------------------------------------------------------------------------
  Блок управления генератором в зависимости от текущего состояния
  Перечень состояний (genSost):
    0 - двигатель заглушен - тянем подсос
    1 - подсос вытянут - перестаем тянуть, включаем стартер
    2 - стартер крутит - перестаем крутить -
    3 - ждем прогрева
    4 - толкаем подсос
    5 - перестаем толкать - двигатель заведен
  ------------------------------------------------------------------------------*/
  //  printf("GEN=%d \n", sendData[2]);
  if (sendData[2] == 1) {  // генератор должен работать
    realGENtime = millis();
    //    printf("genSost=%d \t ========DDDDD=========%d",genSost, realGENtime);
    switch (genSost) {
    case 0: //  генератор в останове
      digitalWrite(startCooler, HIGH);  // включили вентилятор
      digitalWrite(stopEngine, LOW);
      printf("VVVVVVVVVVVVVVVVVVVVVVVVVVVV");
      // включить реле вытягивания подсоса

        //        printf("===NE GLUSHIM === TIANEV PODSOS \n");
      Voltage = 0;
      lastGENtime = millis(); genSost = 5;
      lastIncrData = millis();
      break;

    case 5: //  здесь начианем работать с генерацией
      if ((lastGENtime + 5000) < realGENtime) {  // подождали 5 секунд и если прошли
        digitalWrite(PIN_REL, HIGH);
      }
      break;
    default:

      break;
    }
  }
  else {  //  если генератор не должен работать - глушим
    if ((lastIncrData + 150) < millis()) {
      Voltage = Voltage - 4;
      if (Voltage < 0) { Voltage = 0; }
      lastIncrData = millis();
    }
    if (Voltage == 0) {
      digitalWrite(PIN_REL, LOW);
      digitalWrite(startCooler, LOW);
      digitalWrite(stopEngine, HIGH);

      genSost = 0;
    }

  }
  //  printf("GEN_SOST=%d \t sendWorkRelay=%d \n", genSost, sendWorkRelay);
}
