/******************************************************************
 ManageDriveCaterpillar - библиотека управления гусеницами
 MDC_V001 - название проекта
 Libraries   :
 Author      :  0softic0@gmail.com
 Description :  Управление с помщью Arduino скоростью и 
                направлением. Основное назначение - управление 
                сигналами на драйвер бесколлекторного двигателя
******************************************************************/

#include "MDC_V001.h"


ManageDriveCaterpillar::ManageDriveCaterpillar(int _PIN_NAPR_, int _PIN_VELOCITY_){
  PIN_NAPR=_PIN_NAPR_;          // присвоили значение порта по которому отдается команда по направлению
  PIN_VELOCITY=_PIN_VELOCITY_;  // ======================================================== скорости
}
/*************************************************************************
* Inic - инициализация пинов управления. Настраиваем их на ВЫХОД.
**************************************************************************/
void ManageDriveCaterpillar::Inic (){
  pinMode(PIN_NAPR,OUTPUT);
  pinMode(PIN_VELOCITY, OUTPUT);
  realTime=millis();            // получили значение таймера
  lastTime=realTime;            // установили его как последнее и текущее
}
/**********************************************************
* SetInputReadVelocity - установка требуемой скорости
* _inputReadVelocity - целочисленное значение скорости
**********************************************************/
void ManageDriveCaterpillar::SetInputReadVelocity(int _inputReadVelocity){
  inputReadVelocity=_inputReadVelocity;
}
/**********************************************************
* VelocityCaterpillar - процедура контроля скорости движения 
*   и ее изменения с учетом текущего и требуемого состояний. 
*   При смене состояний - производится замедление. Разгон и 
*   торможение - при сохранении состояния, но изменении 
*   требуемой скоростию (процедура не меняет направление движения
* _readVelocity - требуемая скорость
**********************************************************/
void ManageDriveCaterpillar::VelocityCaterpillar(int _readVelocity){
  readVelocity=_readVelocity;
  if (readVelocity<varSTOP){readVelocity=varSTOP;}  // если требуемая скорость меньше скорости определенной 
                                                    // как останов - останавливаемся
//  Serial.println(readVelocity);
// Дальнейшие дествия - зависят от текущего состояния
  switch(realSost) {
  case 0: // стоим

  // продолжаем стоять
    if (readVelocity<=varSTOP){
      realSost=0; lastSost=0;
      realTime=millis(); lastTime=realTime; // запомнили состояние таймеров
    }
    if (readVelocity>varSTOP){ //разгоняемся
      realSost=1; lastSost=1;
      //Velocity=varMinVelocity;
      realTime=millis(); lastTime=realTime; // запомнили состояние таймеров
    }
  break;
  
  case 1: // разгоняемся
    // продолжаем разгоняться
    if (readVelocity>Velocity){
      realTime=millis();
      if (realTime>(lastTime+incrVelocity)){ // если пора добавлять скорость
        lastTime=realTime; //запомнили время инкрементации
        Velocity=Velocity+5; // увеличили значение текущей скорости на 5
        if (Velocity > readVelocity) { Velocity = readVelocity; }
      }
      realSost=1;
    }
    // достигли скорости разгона и держим ее
    if (readVelocity==Velocity){
      realTime=millis(); lastTime=realTime;
      realSost=3;
    }
    // тормозим
    if (readVelocity<Velocity){
      realTime=millis(); lastTime=realTime;
      realSost=2;
    }
  break;
  case 2: // тормозим
    // продолжаем тормозить
    if (readVelocity<Velocity){
      realTime=millis();
      if (realTime>(lastTime+decrVelocity)){
        lastTime=realTime;
        Velocity=Velocity-10;
        if (Velocity < readVelocity) { Velocity = readVelocity; }
      }
      realSost=2;
    }
    // достигли скорости до которой тормозили
    if (readVelocity==Velocity){
      realTime=millis(); lastTime=realTime;
      if (Velocity<=varSTOP){ // достигли останова
        realSost=0;
        readVelocity=varSTOP;
        Velocity=varSTOP;
      } else {
        realSost=3;
      }
    }
    // разгоняется
    if (readVelocity>Velocity){
      realTime=millis(); lastTime=realTime;
      realSost=1;
    }

  break;
  case 3: // движемся ровно
    // продолжаем двигаться ровно
    if (readVelocity==Velocity){
      realTime=millis(); lastTime=realTime;
      realSost=3;
    }
    // разгоняемся
    if (readVelocity>Velocity){
      realTime=millis(); lastTime=realTime;
      realSost=1;
    }
    // начали тормозить
    if (readVelocity<Velocity){
      realTime=millis(); lastTime=realTime;
      realSost=2;
    }

    break;
  default:
  
  break;
  } 
}
/**********************************************************
* DriveCaterpillar - подготавливаем данные с учетом 
*   текущего состояния.
*   Состояние - это направление движения: вперед, назад, 
*   стоим. 
**********************************************************/
void ManageDriveCaterpillar::DriveCaterpillar(){
  if (inputReadVelocity<0) {  // определяем направление движения "-" - назад "+" вперед
    readNapr=-1;
  } else {
    if (inputReadVelocity==0){
      readNapr=0;
    } else{
      readNapr=1;
    }
  }
  if (inputReadVelocity>=0){  // приводим введенное значение к положительному
    readVelocity=inputReadVelocity;
  }
  else {
    readVelocity=-inputReadVelocity;
  }
  //readVelocity=map(readVelocity,0 ,200 ,0 ,200 );  // нормализуем до диапазона приемлемого для драйвера 
  //Serial.print(readNapr);
  //Serial.print ("==");
  //Serial.println (readVelocity);
  // Если направление не поменялось
  if (realNapr==readNapr){
    VelocityCaterpillar(readVelocity);
  } else{
    // стояли - едем вперед
    if (realNapr==0&&readNapr==1){
      digitalWrite(PIN_NAPR,HIGH);
      realNapr=readNapr;
      VelocityCaterpillar(readVelocity);
    }
    // стояли - едем назад
    if (realNapr==0&&readNapr==-1){
      digitalWrite(PIN_NAPR,LOW);
      realNapr=readNapr;
      VelocityCaterpillar(readVelocity);
    }
    // ехали вперед - останов
    if (realNapr==1&&readNapr==0){
      // если скорость еще не остановились
      if (Velocity>varSTOP){
        digitalWrite(PIN_NAPR,HIGH);
        VelocityCaterpillar(varSTOP);
      } else{ // остановились
        digitalWrite(PIN_NAPR,HIGH);
        realNapr=0;
      }
    }
    // ехали вперед - ехать назад
    if (realNapr==1&&readNapr==-1){
      if (Velocity>varSTOP){
        digitalWrite(PIN_NAPR,HIGH);
        VelocityCaterpillar(varSTOP);
      } else{ // остановились
        digitalWrite(PIN_NAPR,LOW);
        realNapr=0;
      }
    }
    // ехали назад - останов
    if (realNapr==-1&&readNapr==0){
      if (Velocity>varSTOP){
        digitalWrite(PIN_NAPR,LOW);
        VelocityCaterpillar(varSTOP);
      } else {
        digitalWrite(PIN_NAPR,LOW);
        realNapr=0;
      }
    }
    // ехади назад - едем вперед
    if (realNapr==-1&&readNapr==1){
      if (Velocity>varSTOP){
        digitalWrite(PIN_NAPR,LOW);
        VelocityCaterpillar(varSTOP);
      } else {
        digitalWrite(PIN_NAPR,HIGH);
        realNapr=0;
      }
    }
  }
}
/**********************************************************
* Run_Run - процедура отдающее непосредственно 
*   продготовленную команду с ранее подготовленными данными
**********************************************************/
void ManageDriveCaterpillar::Run_Run(){
  analogWrite(PIN_VELOCITY,Velocity);
}
