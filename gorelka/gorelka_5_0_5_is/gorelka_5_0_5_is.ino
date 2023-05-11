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
#include <Wire.h> 
#include<LiquidCrystal_I2C.h>
#define Water_max 0 //
#define Water_min 1
#define Oil_max 2
#define Oil_min 3
#define Start 4
#define Stop 5
LiquidCrystal_I2C lcd(0x27,20,4); 


byte count_iz_temp=0;// счетчик цикла изверения температуры
byte count_lcd=0;     // счетчик цикла вывода на лсд 
//float rezultat;
//float *p_rezult;


float t_oil;
float t_water;
float *p_t_oil;
float *p_t_water;
boolean job=false;    //флаг нагрева масла
boolean rabota_g=false;   //флаг запуска горелки
boolean *p_rabota_g; //флаг запуска горелки (функция)
boolean anti_smog;  // чтобы не дымило при доливе масла, тушим горелку
byte chislo; //счетная переменная в энкодере
byte *p_chislo;

//p_chislo= & chislo;
//int shag=1;
const int pin_B=11;
const int pin_A=10;
//unsigned char encoder_A;
//unsigned char encoder_B;
//unsigned char encoder_A_prev=0;
unsigned long currentTime;
unsigned long loopTime;
char *menu[]={"Water_max","Water_min","Oil_max","Oil_min","Start","Stop"};
char *uroven_v_bake[]={"Up-LOW Down-LOW","Up-HIGH Down-HIGH","Up-LOW Down-HIGH"};
char *ukaz_uroven[1];
byte var_menu[6];   //массив данных 
byte *p_var_menu;
char* p_menu;
int datchik_plameni=2;    //датчик наличия пламени
int zapusk=14;    //пин управления катушкой зажигания
int knop_pin=12;    //управление кнопкой энкодера
int urowen_up=3;    //верхний уровень масла
int urowen_down=4;   //нижний уровень масла
int podkach_pin=5;   //управление насосом подкачки
int cirkulacii_pin=7;  //циркуляионный насос
int nagrew_pin=13;      //управление теном нагрева масла
int vozdux_pin=6;      //управление подачей воздуха
boolean f_zapusk;  // флаг успешного запуска горелки

boolean urowen; //флаг уровня масла


// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library

//OneWire  ds(8);  // on pin 10

void flash(void); //обработка таймера(счетчик измерений и лсд)
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
  digitalWrite(zapusk,LOW);
  pinMode(datchik_plameni,INPUT);
  digitalWrite(datchik_plameni,HIGH);
//  pinMode(urowen_up,INPUT);
//  digitalWrite(urowen_up,HIGH);
//  pinMode(urowen_down,INPUT);
//  digitalWrite(urowen_down,LOW);
  pinMode(podkach_pin,OUTPUT);
  digitalWrite(podkach_pin,LOW);
  pinMode(cirkulacii_pin,OUTPUT);
  digitalWrite(cirkulacii_pin,LOW);
  pinMode(nagrew_pin,OUTPUT);
  digitalWrite(nagrew_pin,LOW);
  pinMode(vozdux_pin,OUTPUT);
  digitalWrite(vozdux_pin,LOW);
  pinMode(knop_pin,INPUT);
  digitalWrite(knop_pin,HIGH);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("nachalo");
 // p_rezult=&rezultat;
  p_t_oil=&t_oil;
  p_t_water=&t_water;
 // p_menu=&menu[0];
  p_var_menu=var_menu;
  p_chislo=&chislo;
  p_rabota_g=&rabota_g;
  water.begin();
  oil.begin();
  water.setResolution(7);    // точность 0,5 градуса
  oil.setResolution(7); 
  
  MsTimer2::set(1000, flash); // 1000ms period
 // MsTimer2::start();
  delay(3000);
  
}
//===========================================================
void loop(void) {
  unsigned long vremja;   //  корректное время 
   unsigned long old_vremja=millis(); //время нахождения в настройках
  byte i=0;   //счетчик показания меню
  byte *p_i;  // передаем счетчик в функцию
  p_i=&i;
  lcd.clear();
  lcd.print("Setup");
  //===========================================================
 while(1)    //цикл управления энкодером
 {
    vremja=millis();   //читаем время
   if(digitalRead (knop_pin)==LOW)   // кнопку нажали
   {
  // vremja=pulseIn(knop_pin,LOW,5000000);
delay(300);  //дребезг

     setup_menu(menu,var_menu,p_chislo,p_i); //уходим считать энкодер
   old_vremja+=10000; //пока крутим энкодер, накидываем время, чтоб не выпасть из SETUPа
   i++; // следущее меню
   if(i>3) //прошли все, уходим работать
   {
     lcd.setCursor(0,0);
     lcd.print("Setup          ");
     lcd.setCursor(0,1);
     lcd.print("Exit           ");
     delay(3000);
     goto label1;
   }
   } 
   if(vremja>(old_vremja + 10000)) break; //не входили в энкодер, просто прошло время ожидания
 }
 label1:
 lcd.clear();
 lcd.print("out");
 delay(2000); 
  

 MsTimer2::start(); //запускаем таймер
 

 //===================================================
 while(1)   //цикл управления работой горелки
  {
    //-----------------------переменные прокачки масла перед запуском горелки
    unsigned long wremja_1; //время вхождения 
    unsigned long wremja_2; //временная
    unsigned long wremja_3; //
    unsigned long zaderghka;
    
    //....................................запуск измерения температуры воды и масла
  if (count_iz_temp >=5)  //прошло 5 сек
    {
      izmerenieTemper(p_t_oil,p_t_water);    //уходим мерить температуру
      count_iz_temp=0;        //сбрасываем счетчик
      //MsTimer2::set(1000, flash);
      MsTimer2::start();   //запускаем таймер(он был остановлен в функции
     } 
     //................................................ //вывод на лсд
     
     
  if(count_lcd >=2)        //прошло 2 сек 
     lcd_out();             // уходим писать лсд
    
     
     //................................................ //проверка уровня масла
    
    if((digitalRead(urowen_up)==LOW)&&(digitalRead(urowen_down)==LOW))   //бак пустой
          {
           *ukaz_uroven=*uroven_v_bake; // Up-LOW Down-LOW
            digitalWrite(vozdux_pin,LOW);
            wremja_1=millis();
            digitalWrite(podkach_pin,HIGH);              //включаем подкачку
            lcd.setCursor(0,2);
            lcd.print("Podkachka masla");
            urowen=false;                   //олускаем флаг уровня, наливаемся
            anti_smog=false;     //пока не нальемся, запуск невозможен
           
         //   Serial.print("uruwen");
         //   Serial.println(urowen);
           }
           else if ((digitalRead(urowen_up)==HIGH)&& (digitalRead(urowen_down)==HIGH)) //масло налили
           {
            *ukaz_uroven=*(uroven_v_bake+1); // Up-HIGH Down-HIGH
            digitalWrite(podkach_pin,LOW);                            //выключаем подкачку
            urowen=true;                           //поднимаем флаг, можно работать
            
          //  Serial.print("uruwen");
         //   Serial.println(urowen);
           }
           else if ((digitalRead(urowen_up)==LOW)&& (digitalRead(urowen_down)==HIGH)) //если маслао в пределах и пламя есть
                                                    
           {
            *ukaz_uroven=*(uroven_v_bake+2); // Up-LOW Down-HIGH
            //digitalWrite(podkach_pin,LOW);                            //подкачивать не нужно
            urowen=true;                                        //флаг стоит, можно работать
           }
            wremja_2=millis();
           if((wremja_2-wremja_1)>90000)
           {
                digitalWrite(podkach_pin,LOW);              //выключаем подкачку
                anti_smog=true;
           }
 
   //...................................включение\выключение нагревателя масла
  
   if (urowen==true)  // масло налито
   {
//      if ((t_oil < EEPROM.read(Oil_min))&&!job)   //масло остыло
      if (t_oil < EEPROM.read(Oil_min))   //масло остыло
      {
         digitalWrite(nagrew_pin,HIGH); // включаем нагрев
         lcd.setCursor(0,2);
         lcd.print("Nagrev masla");
         job=false;            // пока флаг не поднят, греем масло, стоим
         
      }
        
      else if(t_oil>=EEPROM.read(Oil_max)) //если масло нагрелось
       {
         digitalWrite(nagrew_pin,LOW);// выключаем нагрев
         job=true;                   //можно работать
       }
   }   
     else
    { 
        digitalWrite(nagrew_pin,LOW);// масло в диапазоне
        job=true;   //можно работать
    }
    
    if(((t_oil>50)&&(t_oil<75))&&rabota_g)
  {
   digitalWrite(vozdux_pin,LOW); //при работе масло остыло до 70, гасим горелку
   digitalWrite(cirkulacii_pin,LOW); //выключаем циркуляцию
    job=false;  //стоим, греемся
  }
 
     //..........................................................запуск горелки
     
      if((t_water<=EEPROM.read(Water_min)) && job && !rabota_g && urowen && anti_smog)//если вода остыла, масло горячее и пламени нет  и анти_дым и уровень в норме
       {
       //  Serial.println("zapusk gorelki");
           digitalWrite(cirkulacii_pin,HIGH);    //включаем циркуляцию, ждем , что бы масло прокачалось
           if(t_water<40) zaderghka=110000;// вода холоднее 40, прокачмваем масло 90 сек
              else
                   zaderghka=80000; //иначе, прокачиваем масло 30 сек 
                   
           wremja_1=millis();
           wremja_3=millis();    
                   
           while(1)
            {
              wremja_2=millis();
               if((wremja_2-wremja_3) >=5000) //прошло 5 сек, идем смотреть температуру
              {
                 izmerenieTemper(p_t_oil,p_t_water);    //уходим мерить температуру
                 lcd_out();             // уходим писать лсд
                 if (t_oil < (EEPROM.read(Oil_max)-2))   //масло начинает остывать
                     digitalWrite(nagrew_pin,HIGH); // включаем нагрев
                 else if(t_oil>=EEPROM.read(Oil_max)) //если масло нагрелось
                     digitalWrite(nagrew_pin,LOW);// выключаем нагрев 
                 wremja_3=wremja_2;
              }
            if((wremja_2-wremja_1)>=zaderghka) //прокачали масло 
            break; //идем запускать горелку
          }     
              
          // digitalWrite(vozdux_pin,LOW);
        f_zapusk=Zapusk(p_rabota_g);    //уходим запускать горелку, проверяем датчик пламени
       
            
        
             MsTimer2::start();
         //  Serial.print("zapusk = ");
        //  Serial.println(f_zapusk);
           if(f_zapusk==false) // если после трех попыток горелка не включилась, тушим все, выдаем ошибку, уходим на всегда
           {
             MsTimer2::stop();
             digitalWrite(cirkulacii_pin,LOW);
             digitalWrite(vozdux_pin,LOW);
             digitalWrite(zapusk,LOW);
             digitalWrite(podkach_pin,LOW); 
             digitalWrite(nagrew_pin,LOW);
             lcd.clear();
             lcd.print("ERROR");
             lcd.setCursor(0,1);
             lcd.print("DATCHIK_PLAMENI");
             while(1)
               {
//               Serial.println("ERROR");
//               delay(1000);
             
               }
          }
          digitalWrite(zapusk,LOW); // выключаем искру
       }
        else if(t_water >= EEPROM.read(Water_max)) // набрали температуру, тушим горелку
          {
             digitalWrite(cirkulacii_pin,LOW);
             digitalWrite(vozdux_pin,LOW); 
          }
//      else if(t_water<EEPROM.read(Start)-1.0)
//      {
//         digitalWrite(cirkulacii_pin,HIGH);
//        digitalWrite(vozdux_pin,HIGH); 
//      }

  if (digitalRead(datchik_plameni)==HIGH)   //если пламени нет
     {
        digitalWrite(cirkulacii_pin,LOW);      //выключаем циркуляцию
        digitalWrite(vozdux_pin,LOW);      //выключаем воздух
        rabota_g=false;                   //опускаем флаг работы
     }
  if(t_oil>=105)   // если масло перегрето, выключаем все, выдаем ошибку
      {
        digitalWrite(cirkulacii_pin,LOW);
        digitalWrite(vozdux_pin,LOW);
        digitalWrite(nagrew_pin,LOW);
        digitalWrite(podkach_pin,LOW); 
        digitalWrite(zapusk,LOW); 
        lcd.clear();
        lcd.print("ERROR T_OIL");
        while(1){};     //зацикливанемся навсегда
     }
  if(t_water>=92) // если вода перегрета, выключаем все, выдаем ошибку
  {  
  
        digitalWrite(cirkulacii_pin,LOW);
        digitalWrite(vozdux_pin,LOW);
        digitalWrite(nagrew_pin,LOW);
        digitalWrite(podkach_pin,LOW);
        digitalWrite(zapusk,LOW);  
        lcd.clear();
        lcd.print("ERROR T_WATER");
        while(1){};        //зацикливаемся навсегда
  }
    
  }
}
//=======================================================================
void flash(void) //обслуживание прерывания таймер2
{
  count_iz_temp ++; //счетчик измерения тьеипературы
  
  count_lcd++;     //счетчик вывода на лсд
}

//========================================================================
void izmerenieTemper(float *p_t_oil,float *p_t_water)  //измерение температуры
{
 // lcd.setCursor(0,2);
 // lcd.print("Izmerenie Temperatur");
  MsTimer2::stop();
    // точность 0,5 градуса
  water.requestTemperatures();// запускаем преобразование температуры воды
 delay(40);
 oil.requestTemperatures();     //запускаем преобразование температуры масла
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
 *p_t_water=water.getTempCByIndex(0); // читаем температуру
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
  lcd.print("WATER");
  lcd.setCursor(6,0);
  lcd.print(*p_t_water);
  lcd.setCursor(0,1);
  lcd.print("OIL ");
  lcd.setCursor(6,1);
  lcd.print(*p_t_oil);
  lcd.setCursor(0,3);
  lcd.print(*ukaz_uroven);
  MsTimer2::start();
  count_lcd=0;
  
  }
  //=============================================================управление энкодером
void setup_menu(char *menu[], byte p_var_menu[] ,byte *p_chislo,byte *p_i)
{
 //static byte i=0;
 unsigned long old_time=millis(); //берем время
 unsigned long currentTime;  // корректное время
unsigned long loopTime;
 //unsigned long *p_count_time;
// p_count_time= &count_time;
 byte shag=1; //шаг приращения энкодера
 byte encoder_A;
 byte *p_encoder_A;
 p_encoder_A=& encoder_A;
byte encoder_B;
byte *p_encoder_B;
p_encoder_B=& encoder_B;

byte encoder_A_prev=0;
byte *p_encoder_A_prev;
p_encoder_A_prev= & encoder_A_prev;
*(p_var_menu + *p_i)=EEPROM.read(*p_i); //читаем переменные из меню соответственно(EEPROM)

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
         if(*(p_var_menu + Water_max)>85) *(p_var_menu + Water_max)=85; //не даем поднять температуру воды больше значения
         if(*(p_var_menu + Water_min)>85) *(p_var_menu + Water_min)=85; // тоже самое
       
        lcd.print("               "); //стираем лсд
       lcd.setCursor(0,1);
     
       lcd.print(*(p_var_menu + *p_i));
        // Serial.print("chislo  ");
        // Serial.print(chislo);
        // Serial.println();
        /// Serial.print(*p_chislo);
         loopTime=currentTime;
         
       
     }
} while ((currentTime<(old_time+5000))&&(digitalRead(knop_pin)!=LOW));// выжод из энкодера
         //(digitalRead(knop_pin)!=LOW);
EEPROM.write(*p_i,*(p_var_menu + *p_i)); //записываем в память соответственно меню
//  *p_i++;
//  if((*p_i)>5) *p_i=0;

lcd.setCursor(10,1);
lcd.print("Write");


}

//=============================================проверка успешного запуска горелки
boolean Zapusk(boolean *p)// запуск горелки
{

    //-----------------------переменные прокачки масла перед запуском горелки
    unsigned long wremja_1; //время вхождения 
    unsigned long wremja_2; //временная
    unsigned long wremja_3; //
  
  byte i=0;
  MsTimer2::stop(); 
  do
  {
     i++;
      lcd.setCursor(0,2);
     lcd.print("Zapusk Gorelki");
      digitalWrite(zapusk,HIGH);    // подаем искру
      delay(500);
      digitalWrite(vozdux_pin,HIGH);// подаем воздух
      delay(2500);
        if(digitalRead(datchik_plameni)==LOW)  //пламя есть,
        {
      //  Serial.print("plamja on");
        goto label2;   //есть работа 
        }
        else
         {
           digitalWrite(vozdux_pin,LOW);
           digitalWrite(zapusk,LOW);// иначе все выключаем, работы нет
           
        wremja_1=millis();
        wremja_3=millis();
        while(1)
        {
          wremja_2=millis();
          if((wremja_2-wremja_3) >=5000) //прошло 5 сек, идем смотреть температуру
             {
               izmerenieTemper(p_t_oil,p_t_water);    //уходим мерить температуру
                lcd_out();             // уходим писать лсд
                 if (t_oil < (EEPROM.read(Oil_max)-2))   //масло начинает остывать
                     digitalWrite(nagrew_pin,HIGH); // включаем нагрев
                 else if(t_oil>=EEPROM.read(Oil_max)) //если масло нагрелось
                     digitalWrite(nagrew_pin,LOW);// выключаем нагрев 
                wremja_3=wremja_2;
             }
             if((wremja_2-wremja_1)>=20000) //прокачали масло 
             break; //идем запускать горелку
        } 
           
         }
  } while(i<3); // повторяем 3 раза
  *p_rabota_g=false;
  return false;
   label2:
  // Serial.println("datchik plameni ON");
   *p_rabota_g=true;
   return true;
   
   
  
} 

