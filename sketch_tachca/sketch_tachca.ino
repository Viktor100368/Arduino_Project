#include <MsTimer2.h>
#define count 6
#define leftPwmPin 9
#define rightPwmPin 10
#define leftDirPin 8
#define rightDirPin 11

int midle=1500;
int leftPwm;//шим левого борта
int rightPwm;//шим правого борта
int leftVal;//длительность сигнала используемая для вычисления левого шим
int rightVal;//длительность сигнала используемая для вычисления правого шим
int *p_midle;//указатель на midle (средний уровень длины сигнала)
int *p_leftVal; //указатель на leftVal
int *p_rightVal;//указатель на rightVal
bool forvardOrBack = true;//флаг true - вперед, false - назад
bool *p_forvardOrBack;
int prevValueForvardOrBack = 1002; //предидущее значение сигнала смены направлениея движения
int *p_prevValueForvardOrBack;
bool togleEvent = false;// флаг преключения направления движения
bool *p_togleEvent;
unsigned long time = 0;
int chanel[] ={2,3,4,5,6,7};

int duration[count];
int countTimer;
int swithDirectionCounter;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  MsTimer2::set(100,timerInterupt);
  p_leftVal = &leftVal;
  p_rightVal = &rightVal;
  p_midle = &midle;
  p_forvardOrBack = &forvardOrBack;
  p_togleEvent = &togleEvent;
  p_prevValueForvardOrBack = &prevValueForvardOrBack;
  
  for(int i=0; i<count; i++){
    pinMode(chanel[i],INPUT);
  }
  pinMode(leftPwmPin,OUTPUT);
  pinMode(rightPwmPin,OUTPUT);  
  pinMode(leftDirPin,OUTPUT);
  pinMode(rightDirPin,OUTPUT);

  MsTimer2::start();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if(countTimer >= 2){
    setSpeedAndDirection(duration[2], duration[3], p_leftVal, p_rightVal);
  }
  if(swithDirectionCounter >=3){
    swithingDirectionMoving(duration[5], p_forvardOrBack, p_midle, p_prevValueForvardOrBack, p_togleEvent);
  }
  if(togleEvent){
    stopCar(duration[2], p_leftVal, p_rightVal);
  }
  //управление двигателями
  leftPwm = map(leftVal,1000,2000,0,100);
  rightPwm = map(rightVal,1000,2000,0,100);
  leftPwm = constrain(leftPwm,0,95);
  rightPwm = constrain(rightPwm,0,95);
  //проверка смены направления движения
  if(togleEvent == true){
    stopCar();
  }
  Serial.println("==========================");
  Serial.print("leftPwm = ");
  Serial.println(leftPwm);
  Serial.print("rightPwm = ");
  Serial.println(rightPwm);

  // for(int i=0; i<count; i++){
  //   Serial.print("chanel ");
  //   Serial.print(i);
  //   Serial.print(" = ");
  //   Serial.println(duration[i]);
  // }
  // Serial.println("------------------------------");
  time = millis();
  unsigned long newTime = millis();
  while(newTime - time < 1000){
    newTime = millis();
  }
}
void timerInterupt(){
  MsTimer2::stop();
  for(int i=0; i<count; i++){
    duration[i]=pulseIn(chanel[i],HIGH);
  }
  countTimer++;
  swithDirectionCounter++;
  MsTimer2::start();
}
void setSpeedAndDirection(int valSpeed, int valDirection, int *p_lV, int *p_rV){
  MsTimer2::stop();
  if((valDirection > 1300)&&(valDirection < 1480)){
    *p_lV = valSpeed-150;//коэффициент пересчета
    *p_rV = valSpeed;
  }else if((valDirection > 1100)&&(valDirection < 1300)){
    *p_lV = valSpeed-300;//коэффициент пересчета
    *p_rV = valSpeed;
  }else if(valDirection < 1100){
    *p_lV = 600;
    *p_rV = valSpeed;
  }else if((valDirection > 1520)&&(valDirection < 1700)){
    *p_lV = valSpeed;
    *p_rV = valSpeed-150;//коэффициент пересчета
  }else if((valDirection > 1700)&&(valDirection < 1900)){
    *p_lV = valSpeed;
    *p_rV = valSpeed-300;//коэффициент пересчета    
  }else if(valDirection > 1900){
    *p_lV = valSpeed;
    *p_rV = 600;
  }else{
    *p_lV = valSpeed;
    *p_rV = valSpeed;
  }
  Serial.println("******************************");
  Serial.print("left value = ");
  Serial.println(*p_lV);
  Serial.print("right value = ");
  Serial.println(*p_rV);
  Serial.println("****************************");
  countTimer=0;
  MsTimer2::start();
}
//функция определения включения тумблера заднего хода
void swithingDirectionMoving(int valForOrBack, bool *p_forvardOrBack, int *p_midle, int *p_prevValueForvardOrBack, bool *p_togleEvent){
  MsTimer2::stop();
  if(valForOrBack < *p_midle){//измеряем сигнал с тумблера меньше среднего 
    *p_forvardOrBack = true;//значит едем вперед
  }else if(valForOrBack > *p_midle){//измеряем сигнал с тумблера больше среднего 
    *p_forvardOrBack = false; //едем вперед
  }
  if(valForOrBack != *p_prevValueForvardOrBack){//текущее значение длины импульса заднего хода не равно предидущему
    *p_togleEvent = true;   //было включение тумблера
  }else 
    *p_togleEvent = false;//иначе нет
  swithDirectionCounter = 0;
  MsTimer2::start();
}
//функция торможения двигателей бортовых передач
void stopCar(int throtleImpulse, int *p_lV, int *p_rV){
  MsTimer2::stop();
  unsigned long initTime = 0, secondTime = 0;
  for(int i=throtleImpulse; i>0; i-=10){// в цикле гасим скорость двигателей с переодом 10 значений положения ручки газа
    analogWrite(leftPwmPin,throtleImpulse);
    analogWrite(rightPwmPin,throtleImpulse);
    initTime = millis();
    secondTime = millis();
    while(secondTime - initTime <100){//задержка 100 мс
      secondTime = millis();
    }
  }
  *p_lV = 0;// сбрасываем значения шима бортов в 0 
  *p_rV =0;
  MsTimer2::start();
}