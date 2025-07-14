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
int *p_midle;
int *p_leftVal;
int *p_rightVal;
bool forvardOrBack = true;
bool *p_forvardOrBack;
bool prevValueForvardOrBack = 1002;
bool *p_prevValueForvardOrBack;
bool togleEvent = false;
bool *p_togleEvent;
unsigned long time = 0;
int chanel[] ={2,3,4,5,6,7};

int duration[count];
int countTimer;
int stopingTimer;




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
  if(stopingTimer >=3){
    stoping(duration[5], p_forvardOrBack, p_midle, p_prevStateForvard, p_togleEvent);
  }
  //управление двигателями
  leftPwm = map(leftVal,1000,2000,0,100);
  rightPwm = map(rightVal,1000,2000,0,100);
  leftPwm = constrain(leftPwm,0,95);
  rightPwm = constrain(rightPwm,0,95);
  //проверка смены направления движения
  if(togleEvnet == true){
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
  stopingTimer++;
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

void stoping(int valForOrBack, bool *p_forvardOrBack, int *p_midle, int *p_prevValueForvardOrBack, bool *p_togleEvent)){
  MsTimer2::stop();
  if(valForOrBack < *p_midle){
    *p_forvardOrBack = true;
  }else if(valForOrBack > *p_midle){
    *p_forvardOrBack = false;
  }
  if(valForOrBack != *p_prevValueForvardOrBack){
    *p_togleEvent = true;
  }else 
    *p_togleEvent = false;
  stopingTimer = 0;
  MsTimer2::start();
}
void stopCar(){
  
}