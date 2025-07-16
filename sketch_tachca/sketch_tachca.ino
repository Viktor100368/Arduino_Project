#include <MsTimer2.h>
#define count 6
#define leftPwmPin 9
#define rightPwmPin 10
#define leftDirPin 8
#define rightDirPin 11

int midle = 1500;
int leftPwm;                //шим левого борта
int rightPwm;           //шим правого борта   
int *p_leftPwm;
int *p_rightPwm;
            
int leftVal;                //длительность сигнала используемая для вычисления левого шим
int rightVal;               //длительность сигнала используемая для вычисления правого шим
int *p_midle;               //указатель на midle (средний уровень длины сигнала)
int *p_leftVal;             //указатель на leftVal
int *p_rightVal;            //указатель на rightVal
bool forvardOrBack = false;  //флаг true - назад, false - вперед
bool *p_forvardOrBack;
int prevValueForvardOrBack = 1002;  //предидущее значение сигнала смены направлениея движения
int *p_prevValueForvardOrBack;
bool togleEvent = false;  // флаг преключения направления движения
bool *p_togleEvent;
unsigned long time = 0;
int chanel[] = { 2, 3, 4, 5, 6, 7 };

int duration[count];
int countTimer;
int swithDirectionCounter;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  MsTimer2::set(20, timerInterupt);
  p_leftVal = &leftVal;
  p_rightVal = &rightVal;
  p_midle = &midle;
  p_forvardOrBack = &forvardOrBack;
  p_togleEvent = &togleEvent;
  p_prevValueForvardOrBack = &prevValueForvardOrBack;
  p_leftPwm = &leftPwm;
  p_rightPwm = &rightPwm;

  for (int i = 0; i < count; i++) {
    pinMode(chanel[i], INPUT);
  }
  pinMode(leftPwmPin, OUTPUT);
  pinMode(rightPwmPin, OUTPUT);
  pinMode(leftDirPin, OUTPUT);
  pinMode(rightDirPin, OUTPUT);
  digitalWrite(leftDirPin,LOW);
  digitalWrite(rightDirPin,LOW);

  MsTimer2::start();
}

void loop() {
  // put your main code here, to run repeatedly:
  if (countTimer >= 2) {
    setSpeedAndDirection(duration[2], duration[3], p_leftVal, p_rightVal);
  }
   //проверка смены направления движения
  if (swithDirectionCounter >= 3) {
    swithingDirectionMoving(duration[5], p_forvardOrBack, p_midle, p_prevValueForvardOrBack, p_togleEvent);
  }
  //торможение двигателей бортов
  // if (togleEvent == true) {
  //   if(forvardOrBack == true){
  //     digitalWrite(leftDirPin,LOW);
  //     digitalWrite(rightDirPin,LOW);      
  //   }else{
  //     digitalWrite(leftDirPin,HIGH);
  //     digitalWrite(rightDirPin,HIGH);      
      
  //   }
  //  stopCar(duration[2], p_leftVal, p_rightVal);

  // }
  //управление двигателями
  leftPwm = map(leftVal, 1000, 2000, 0, 255);
  rightPwm = map(rightVal, 1000, 2000, 0, 255);
  leftPwm = constrain(leftPwm, 0, 240);
  rightPwm = constrain(rightPwm, 0, 240);
  analogWrite(leftPwmPin,leftPwm);
  analogWrite(rightPwmPin,rightPwm);
 
 
  Serial.println("==========================");
  Serial.print("leftPwm = ");
  Serial.println(leftPwm);
  Serial.print("rightPwm = ");
  Serial.println(rightPwm);

  
  // time = millis();
  // unsigned long newTime = millis();
  // while (newTime - time < 1000) {
  //   newTime = millis();
  // }
}
void timerInterupt() {
  MsTimer2::stop();
  for (int i = 0; i < count; i++) {  // в цикле считываем длительность сигнала управления по каналам
    duration[i] = pulseIn(chanel[i], HIGH);
  }
  countTimer++;
  swithDirectionCounter++;
  MsTimer2::start();
}
void setSpeedAndDirection(int valSpeed, int valDirection, int *p_lV, int *p_rV) {
  MsTimer2::stop();
  int leftPwm = 0;
  int rightPwm = 0;
  if ((valDirection > 1300) && (valDirection < 1480)) {
    *p_lV = valSpeed - 150;  //изменение скорости поворота влево при малом отклонении ручки
    *p_rV = valSpeed;
  } else if ((valDirection > 1100) && (valDirection < 1300)) {
    *p_lV = valSpeed - 300;  //увелечение отклонения ручки управления
    *p_rV = valSpeed;
  } else if (valDirection < 1100) {
    *p_lV = 600;  // при максимальном отклонении
    *p_rV = valSpeed;
  } else if ((valDirection > 1520) && (valDirection < 1700)) {
    *p_lV = valSpeed;
    *p_rV = valSpeed - 150;  //изменение скорости поворота вправо при малом отклонении ручки
  } else if ((valDirection > 1700) && (valDirection < 1900)) {
    *p_lV = valSpeed;
    *p_rV = valSpeed - 300;  //увелечение отклонения ручки управления
  } else if (valDirection > 1900) {
    *p_lV = valSpeed;
    *p_rV = 600;  // при максимальном отклонении
  } else {
    *p_lV = valSpeed;  //иначе едем прямо
    *p_rV = valSpeed;
  }

  
  // Serial.println("******************************");
  // Serial.print("left value = ");
  // Serial.println(*p_lV);
  // Serial.print("right value = ");
  // Serial.println(*p_rV);
  // Serial.println("****************************");
  countTimer = 0;
  MsTimer2::start();
}
//функция определения включения тумблера заднего хода
void swithingDirectionMoving(int valForOrBack, bool *p_forvardOrBack, int *p_midle, int *p_prevValueForvardOrBack, bool *p_togleEvent) {
  MsTimer2::stop();
  if (valForOrBack < *p_midle) {         //измеряем сигнал с тумблера меньше среднего
    *p_forvardOrBack = true;             //значит едем вперед
  } else if (valForOrBack > *p_midle) {  //измеряем сигнал с тумблера больше среднего
    *p_forvardOrBack = false;            //едем вперед
  }
  if (valForOrBack != *p_prevValueForvardOrBack) {  //текущее значение длины импульса заднего хода не равно предидущему
    *p_togleEvent = true;                           //было включение тумблера
  } else
    *p_togleEvent = false;  //иначе нет
  swithDirectionCounter = 0;
  MsTimer2::start();
}
//функция торможения двигателей бортовых передач
void stopCar(int throtleImpulse, int *p_lV, int *p_rV) {
  MsTimer2::stop();
  unsigned long initTime = 0;
  unsigned long secondTime = 0;
  for (int i = throtleImpulse; i > 0; i -= 10) {  // в цикле гасим скорость двигателей с переодом 10 значений положения ручки газа
    analogWrite(leftPwmPin, throtleImpulse);
    analogWrite(rightPwmPin, throtleImpulse);
    unsigned long initTime = millis();
    unsigned long secondTime = millis();
    while (secondTime - initTime < 100) {  //задержка 100 мс
      secondTime = millis();
    }
  }
  *p_lV = 0;  // сбрасываем значения шима бортов в 0
  *p_rV = 0;
  MsTimer2::start();
}