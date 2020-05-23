﻿#include <SPI.h>                                               // Подключаем библиотеку для работы с шиной SPI.
#include <nRF24L01.h>                                          // Подключаем файл настроек из библиотеки RF24.
#include <RF24.h>                                              // Подключаем библиотеку для работы с nRF24L01+.
#include "printf.h"
RF24     radio(5, 6);                                         // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов модуля (CE, SS).
unsigned long      myData;                                            // Объявляем массив для хранения и передачи данных (до 32 байт включительно).
                                                               //
void setup() {                                                  //
  Serial.begin(9600);
  printf_begin();
  radio.begin();                                  // Инициируем работу модуля nRF24L01+.
  radio.setChannel(120);                                // Указываем канал передачи данных (от 0 до 125), 27 - значит передача данных осуществляется на частоте 2,427 ГГц.
  radio.setDataRate(RF24_250KBPS);                        // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек.
  radio.setPALevel(RF24_PA_MAX);                       // Указываем уровень усиления передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm).
  radio.openWritingPipe(0x1234567890LL);                    // Открываем трубу с адресом 0xAABBCCDD11 для передачи данных (передатчик может одновременно вещать только по одной трубе).
  delay(200);
}                                                              //
                                                               //
void loop() {
  myData=millis(); //
  if (radio.write(&myData, sizeof(myData))) {                // Если указанное количество байт массива myData было доставлено приёмнику, то ...
  //  Данные передатчика были корректно приняты приёмником.  // Тут можно указать код который будет выполняться при получении данных приёмником.
    printf ("%d \n",myData);

 }
  else {                                                     // Иначе (если данные не доставлены) ...
 //  Данные передатчика не приняты или дошли с ошибкой CRC. // Тут можно указать код который будет выполняться если приёмника нет или он не получил данные.
    printf("J");

 }                                                          //
 
  delay(50);                                                 // Устанавливаем задержку на 50 мс. В этом скетче нет смысла слать данные чаще.
}