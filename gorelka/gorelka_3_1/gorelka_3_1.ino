//#include <LiquidCrystalRus.h>
//рабочая версия
#include <MsTimer2.h>
#include<EEPROM.h>
#include <OneWire.h>
#include<LiquidCrystal.h>
LiquidCrystal lcd(7,6,5,4,3,2);
byte count_iz_temp=0;//
byte count_lcd=0;
float rezultat;
float *p_rezult;
byte t_oil;
byte t_water;
byte *p_t_oil;
byte *p_t_water;

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

int knop_pin=12;
int urowen_up=11;
int urowen_down=13;
int podkach_pin=18;
int cirkulacii_pin=19;
int nagrew_pin=16;
int vozdux_pin=17;
float nagr_on=26.0;
float nagr_off=28.5;
float gorelka_start=28.0;
float gorelka_stop=30.0;
boolean urowen;


// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

OneWire  ds(8);  // on pin 10

void flash(void);
void izmerenieTemper(float *p);
void lcd_out(void);
void setup_menu(char *menu[], byte p_var_menu[] ,byte *p_chislo,byte *p_i);
//==============================================================

void setup(void) {
  pinMode(pin_A,INPUT);
  pinMode(pin_B,INPUT);
  currentTime=millis();
  loopTime=currentTime;
  pinMode(urowen_up,INPUT);
  digitalWrite(urowen_up,HIGH);
  pinMode(urowen_down,INPUT);
  digitalWrite(urowen_down,LOW);
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
   
   i++;
   if(i>5) 
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
    
  if (count_iz_temp >=5)   //цикл измерения температуры
    {
      izmerenieTemper(p_rezult);
     } 
     //................................................
  if(count_lcd >=2)         //вывод на лсд
     lcd_out();  
     //................................................
    if(!digitalRead(urowen_up)&&(!digitalRead(urowen_down)))    //проверка уровня масла
          {
            digitalWrite(podkach_pin,LOW);              //включение\выключение
            urowen=false;
           }
           else if (digitalRead(urowen_up)&& digitalRead(urowen_down)) //насоса
           {
            digitalWrite(podkach_pin,HIGH);                            //подкачки
            urowen=true;
           }
 
   //.....................................................
   if (urowen==true)
   {
      if (rezultat<=nagr_on)   //включение нагревателя масла
      {
         digitalWrite(nagrew_pin,LOW);
      }
      else if(rezultat>=nagr_off)
       {
         digitalWrite(nagrew_pin,HIGH);
       }
   }   
     else 
        digitalWrite(nagrew_pin,HIGH);
   
     //..........................................................
      if((rezultat > (gorelka_start-1.0))&& rezultat<=gorelka_start)
       {
        digitalWrite(cirkulacii_pin,LOW);
        digitalWrite(vozdux_pin,LOW);
       }
     else if(rezultat >= gorelka_stop)
      {
        digitalWrite(cirkulacii_pin,HIGH);
        digitalWrite(vozdux_pin,HIGH); 
      }
      else if(rezultat<gorelka_start-1.0)
      {
         digitalWrite(cirkulacii_pin,HIGH);
        digitalWrite(vozdux_pin,HIGH); 
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
void izmerenieTemper(float *p)  //измерение температуры
{
  MsTimer2::stop();
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius;
  
  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    delay(250);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.print(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(750);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present,HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;

  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");

  
  count_iz_temp=0;
  MsTimer2::start();
  *p= celsius;
  
  
}
  //========================================================
  void lcd_out(void)     //вывод на лсд
  {
    MsTimer2::stop();
      lcd. clear();
  lcd.setCursor(0,0);
  lcd.print("temperatura");
  lcd.setCursor(0,1);
  lcd.print(rezultat);
  MsTimer2::start();
  count_lcd=0;
  
  }
  //=============================================================управление энкодером
void setup_menu(char *menu[], byte p_var_menu[] ,byte *p_chislo,byte *p_i)
{
 //static byte i=0;
 unsigned long count_time=millis(); //берем время
 //unsigned long *p_count_time;
// p_count_time= &count_time;
 byte shag=5;
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
              count_time+=100;
              if(*p_encoder_B)
              {
                if(*p_chislo + shag<=255)  *p_chislo += shag; //увеличиваем показание
              }
              else
              {
                if(*p_chislo-shag >=0 ) *p_chislo -= shag;     //уменьшаем
              }
           }
         
         encoder_A_prev = encoder_A;   //перезаписываем энкодер
        //  *p_chislo=chislo;
         *(p_var_menu + *p_i)=*p_chislo;
       
        lcd.print("               ");
       lcd.setCursor(0,1);
     
       lcd.print(*(p_var_menu + *p_i));
        // Serial.print("chislo  ");
        // Serial.print(chislo);
        // Serial.println();
        /// Serial.print(*p_chislo);
         loopTime=currentTime;
         
       
     }
} while((count_time>=(currentTime-5000))&&(digitalRead(knop_pin)!=LOW));
//while(digitalRead(knop_pin)!=LOW);
EEPROM.write(*p_i,*(p_var_menu + *p_i));
//  *p_i++;
//  if((*p_i)>5) *p_i=0;

lcd.setCursor(10,1);
lcd.print("Write");

}


