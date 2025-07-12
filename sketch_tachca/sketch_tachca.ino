#include <MsTimer2.h>
#define count 6
#define leftPwmPin 9
#define rightPwmPin 10
#define leftDirPin 8
#define rightDirPin 11

int midle=1500;
int leftPwm;
int rightPwm;
int leftVal;
int rightVal;
int *p_midle;
int *p_leftVal;
int *p_rightVal;
unsigned long time = 0;
int chanel[] ={2,3,4,5,6,7};
int duration[count];
int countTimer;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  MsTimer2::set(100,timerInterupt);
  p_leftVal = &leftVal;
  p_rightVal = &rightVal;
  p_midle = &midle;
  
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
    setDirection(duration[3], p_leftVal, p_rightVal, p_midle);
  }
  int l_val = map(leftVal,0,midle,0,255);
  int r_val = map(rightVal,0,midle,0,255);
  leftPwm = map(l_val,0,255,0,100);
  rightPwm = map(r_val,0,255,0,100);
  leftPwm = constrain(leftPwm,0,95);
  rightPwm = constrain(rightPwm,0,95);
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
  MsTimer2::start();
}
void setDirection(int val, int *p_lV, int *p_rV,int *p_midle){
  MsTimer2::stop();
  if((val > 1200)&&(val < 1480)){
    *p_lV = val/2;
    *p_rV = midle;
  }else if(val < 1200){
    *p_lV = 0;
    *p_rV = midle;
  }else if((val > 1520)&&(val < 1800)){
    *p_lV = midle;
    int tempR = val - midle;
    tempR = midle - tempR;
    *p_rV = tempR/2;
  }else if(val > 1800){
    *p_lV = midle;
    *p_rV = 0;
  }else{
    *p_lV = midle;
    *p_rV = midle;
  }
  MsTimer2::start();
}