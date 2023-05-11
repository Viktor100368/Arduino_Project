//#include <LiquidCrystalRus.h>
//рабочая версия
#include <DallasTemperature.h>
OneWire ow_water(8);          //пин подключения датчика темп воды
OneWire ow_oil(9);              //пин подключения датчика темп масла
DallasTemperature water(&ow_water);
DallasTemperature oil(&ow_oil);
DeviceAddress waterAddress,oilAddress;
#include <MsTimer2.h>
#include<EEPROM.h>
#include <OneWire.h>
#include<LiquidCrystal.h>
#define Water_max 0
#define Water_min 1
#define Oil_max 2
#define Oil_min 3
#define Start 4
#define Stop 5
LiquidCrystal lcd(7,6,5,4,3,2);
byte count_iz_temp=0;//
byte count_lcd=0;
float rezultat;
float *p_rezult;
float t_oil;
float t_water;
float *p_t_oil;
float *p_t_water;
boolean job=false;
boolean rabota_g=false;
boolean *p_rabota_g;

byte chislo; //счетная переменная в энкодере
byte *p_chislo;

//p_chislo= & chislo;
//int shag=1;
const int pin_A=14;
const int pin_B=15;
//unsigned char encoder_A;
//unsigned char encoder_B;
//unsigned char encoder_A_prev=0;
unsigned long currentTime;
unsigned long loopTime;
char *menu[]={"Water_max","Water_min","Oil_max","Oil_min","Start","Stop"};
byte var_menu[6];
byte *p_var_menu;
char* p_menu;
int datchik_plameni=11;
int zapusk=10;
int knop_pin=12;
int urowen_up=6;
int urowen_down=7;
int podkach_pin=18;
int cirkulacii_pin=19;
int nagrew_pin=16;
int vozdux_pin=17;
boolean f_zapusk;

boolean urowen;


// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

//OneWire  ds(8);  // on pin 10

void flash(void);
void izmerenieTemper(float *p_t_oil,float *p_t_water);
void lcd_out(void);
void setup_menu(char *menu[], byte p_var_menu[] ,byte *p_chislo,byte *p_i);
boolean Zapusk(boolean *p);
//==============================================================

void setup(void) {
  pinMode(pin_A,INPUT);
  pinMode(pin_B,INPUT);
  currentTime=millis();
  loopTime=currentTime;
  pinMode(zapusk,OUTPUT);
  digitalWrite(zapusk,HIGH);
  pinMode(datchik_plameni,INPUT);
  digitalWrite(datchik_plameni,HIGH);
//  pinMode(urowen_up,INPUT);
//  digitalWrite(urowen_up,HIGH);
//  pinMode(urowen_down,INPUT);
//  digitalWrite(urowen_down,LOW);
  pinMode(podkach_pin,OUTPUT);
  digitalWrite(podkach_pin,!LOW);
  pinMode(cirkulacii_pin,OUTPUT);
  digitalWrite(cirkulacii_pin,!LOW);
  pinMode(nagrew_pin,OUTPUT);
  digitalWrite(nagrew_pin,!LOW);
  pinMode(vozdux_pin,OUTPUT);
  digitalWrite(vozdux_pin,!LOW);
  pinMode(knop_pin,INPUT);
  digitalWrite(knop_pin,HIGH);
  Serial.begin(9600);
  lcd.begin(16,2);
  lcd.print("nachalo");
  p_rezult=&rezultat;
  p_t_oil=&t_oil;
  p_t_water=&t_water;
 // p_menu=&menu[0];
  p_var_menu=var_menu;
  p_chislo=&chislo;
  p_rabota_g=&rabota_g;
  water.begin();
  oil.begin();
  water.setResolution(9);    // точность 0,5 градуса
  oil.setResolution(9); 
  
  MsTimer2::set(1000, flash); // 1000ms period
 // MsTimer2::start();
  delay(3000);
  
}
//===========================================================
void loop(void) {
  unsigned long vremja;
   unsigned long old_vremja=millis();
  byte i=0;
  byte *p_i;
  p_i=&i;
  lcd.clear();
  lcd.print("Setup");
 while(1)
 {
    vremja=millis();
   if(digitalRead (knop_pin)==LOW)
   {
  // vremja=pulseIn(knop_pin,LOW,5000000);
delay(300);

     setup_menu(menu,var_menu,p_chislo,p_i);
   old_vremja+=10000;
   i++;
   if(i>3) 
   {
     lcd.setCursor(0,0);
     lcd.print("Setup          ");
     lcd.setCursor(0,1);
     lcd.print("Exit           ");
     delay(3000);
     goto label1;
   }
   } 
   if(vremja>(old_vremja + 10000)) break;
 }
 label1:
 lcd.clear();
 lcd.print("out");
 delay(2000); 
  

 MsTimer2::start();

 
 while(1)
  {
    //....................................запуск измерения температуры воды и масла
  if (count_iz_temp >=5)  
    {
      izmerenieTemper(p_t_oil,p_t_water);
      count_iz_temp=0;
      //MsTimer2::set(1000, flash);
      MsTimer2::start();
     } 
     //................................................ //вывод на лсд
  if(count_lcd >=2)        
     lcd_out();  
     //................................................ //проверка уровня масла
    if((analogRead(urowen_up)<300)&&(analogRead(urowen_down)<300))   
          {
            digitalWrite(podkach_pin,LOW);              //включение\выключение
            urowen=false;
//            Serial.print("uruwen");
//            Serial.println(urowen);
           }
           else if ((analogRead(urowen_up)>700)&& (analogRead(urowen_down)>700)) //насоса
           {
            digitalWrite(podkach_pin,HIGH);                            //подкачки
            urowen=true;
//            Serial.print("uruwen");
//            Serial.println(urowen);
           }
 
   //...................................включение\выключение нагревателя масла
   if (urowen==true)
   {
      if ((t_oil < EEPROM.read(Oil_min))&&!job)   
      {
         digitalWrite(nagrew_pin,LOW);
         job=false;
         
      }
      else if(t_oil>=EEPROM.read(Oil_max))
       {
         digitalWrite(nagrew_pin,HIGH);
         job=true;
       }
   }   
     else 
        digitalWrite(nagrew_pin,HIGH);
   
     //..........................................................запуск горелки
      if((t_water<=EEPROM.read(Water_min))&&job && !rabota_g)
       {
        digitalWrite(cirkulacii_pin,LOW);
      delay(3000);
          // digitalWrite(vozdux_pin,LOW);
           f_zapusk=Zapusk(p_rabota_g);
           MsTimer2::start();
//           Serial.print("zapusk = ");
//           Serial.println(f_zapusk);
           if(f_zapusk==false)
           {
             MsTimer2::stop();
             digitalWrite(cirkulacii_pin,HIGH);
             digitalWrite(vozdux_pin,HIGH);
             digitalWrite(zapusk,HIGH);
             lcd.clear();
             lcd.print("ERROR");
             lcd.setCursor(0,1);
             lcd.print("DATCHIK_PLAMENI");
             while(1)
             {
               Serial.println("ERROR");
               delay(1000);
             }
           
          }
          digitalWrite(zapusk,HIGH);
       }
        else if(t_water >= EEPROM.read(Water_max))
      {
        digitalWrite(cirkulacii_pin,HIGH);
        digitalWrite(vozdux_pin,HIGH); 
      }
//      else if(t_water<EEPROM.read(Start)-1.0)
//      {
//         digitalWrite(cirkulacii_pin,HIGH);
//        digitalWrite(vozdux_pin,HIGH); 
//      }

  if (digitalRead(datchik_plameni)==HIGH)
  {
    digitalWrite(cirkulacii_pin,HIGH);
        digitalWrite(vozdux_pin,HIGH);
        rabota_g=false;
  }
  if(t_oil>=105)
  {
    digitalWrite(cirkulacii_pin,HIGH);
        digitalWrite(vozdux_pin,HIGH);
        digitalWrite(nagrew_pin,HIGH);
        digitalWrite(podkach_pin,HIGH);  
        lcd.clear();
        lcd.print("ERROR T_OIL");
        while(1){};
  }
  if(t_water>=95)
  {
    
  
  digitalWrite(cirkulacii_pin,HIGH);
        digitalWrite(vozdux_pin,HIGH);
        digitalWrite(nagrew_pin,HIGH);
        digitalWrite(podkach_pin,HIGH);  
        lcd.clear();
        lcd.print("ERROR T_WATER");
        while(1){};
  }
    
  }
}
//=======================================================================
void flash(void) //обслуживание прерывания таймер2
{
  count_iz_temp ++;
  
  count_lcd++;
}

//========================================================================
void izmerenieTemper(float *p_t_oil,float *p_t_water)  //измерение температуры
{
  
  MsTimer2::stop();
    // точность 0,5 градуса
  water.requestTemperatures();
 delay(40);
 oil.requestTemperatures();
 delay(40);
 Serial.print("water ");
 Serial.println(water.getTempCByIndex(0));
 Serial.print("oil ");
 Serial.println(oil.getTempCByIndex(0));
 delay(200);

 ow_water.reset_search();
 ow_oil.reset_search();
 if(!water.getAddress(waterAddress,0)){Serial.println("datchika water net");}
 if(!oil.getAddress(oilAddress,0)){Serial.println("datchika oil net");}
 *p_t_water=water.getTempCByIndex(0);
 *p_t_oil=oil.getTempCByIndex(0);
 
//  MsTimer2::start();
return;
  
  
}
  //========================================================
  void lcd_out(void)     //вывод на лсд
  {
    MsTimer2::stop();
      lcd. clear();
  lcd.setCursor(0,0);
  lcd.print("W ");
  lcd.setCursor(2,0);
  lcd.print(*p_t_water);
  lcd.setCursor(0,1);
  lcd.print("O ");
  lcd.setCursor(2,1);
  lcd.print(*p_t_oil);
  MsTimer2::start();
  count_lcd=0;
  
  }
  //=============================================================управление энкодером
void setup_menu(char *menu[], byte p_var_menu[] ,byte *p_chislo,byte *p_i)
{
 //static byte i=0;
 unsigned long old_time=millis(); //берем время
 unsigned long currentTime;
unsigned long loopTime;
 //unsigned long *p_count_time;
// p_count_time= &count_time;
 byte shag=1;
 byte encoder_A;
 byte *p_encoder_A;
 p_encoder_A=& encoder_A;
byte encoder_B;
byte *p_encoder_B;
p_encoder_B=& encoder_B;

byte encoder_A_prev=0;
byte *p_encoder_A_prev;
p_encoder_A_prev= & encoder_A_prev;
*(p_var_menu + *p_i)=EEPROM.read(*p_i);

*p_chislo =*(p_var_menu + *p_i); // вызываем записанные данные

lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(menu[*p_i]); // выводим меню по порядку
  do //начало цикла обработки энкодера
{
  // alt_chislo=chislo;
    currentTime=millis(); //корректное время
     if(currentTime>=(loopTime+2)) //считываем каждые 2 ms
       {
         *p_encoder_A=digitalRead(pin_A);
         *p_encoder_B=digitalRead(pin_B);
        
         
           if((!(*p_encoder_A))&&(*p_encoder_A_prev))   //если данные изменились лезем дальше
            {
              old_time+=1000;
              if(*p_encoder_B)
              {
                if(*p_chislo + shag<=105)  *p_chislo += shag; //увеличиваем показание
              }
              else
              {
                if(*p_chislo-shag >=0 ) *p_chislo -= shag;     //уменьшаем
              }
              
           }
         
         encoder_A_prev = encoder_A;   //перезаписываем энкодер
        //  *p_chislo=chislo;
         *(p_var_menu + *p_i)=*p_chislo;
         if(*(p_var_menu + Water_max)>90) *(p_var_menu + Water_max)=90;
         if(*(p_var_menu + Water_min)>90) *(p_var_menu + Water_min)=90;
       
        lcd.print("               ");
       lcd.setCursor(0,1);
     
       lcd.print(*(p_var_menu + *p_i));
        // Serial.print("chislo  ");
        // Serial.print(chislo);
        // Serial.println();
        /// Serial.print(*p_chislo);
         loopTime=currentTime;
         
       
     }
} while ((currentTime<(old_time+5000))&&(digitalRead(knop_pin)!=LOW));
         //(digitalRead(knop_pin)!=LOW);
EEPROM.write(*p_i,*(p_var_menu + *p_i));
//  *p_i++;
//  if((*p_i)>5) *p_i=0;

lcd.setCursor(10,1);
lcd.print("Write");


}
boolean Zapusk(boolean *p)
{
  byte i=0;
  MsTimer2::stop();
  do
  {
     i++;
      digitalWrite(vozdux_pin,LOW);
      digitalWrite(zapusk,LOW);
      delay(2000);
        if(digitalRead(datchik_plameni)==LOW)
        goto label2;
        else
         {
           digitalWrite(vozdux_pin,HIGH);
           digitalWrite(zapusk,HIGH);
           delay(3000);
         }
  } while(i<3);
  *p_rabota_g=false;
  return false;
   label2:
   *p_rabota_g=true;
   return true;
   
   
  
} 

