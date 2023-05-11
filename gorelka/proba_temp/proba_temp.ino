
#include <LiquidCrystal.h>
LiquidCrystal lcd(2,3,4,5,6,7);

#include <DallasTemperature.h>
OneWire ow_oil(9);              //пин подключения датчика темп масла
DallasTemperature oil(&ow_oil);
DeviceAddress oilAddress;
#include <OneWire.h>
float oil_var;
int pin=10;
boolean flag;
void setup(void)
{
  lcd.begin(16,2);
  lcd.print("helo world");
   Serial.begin(9600);
   oil.begin();
    // точность 0,5 градуса
  oil.setResolution(9);
 pinMode(pin,OUTPUT);
digitalWrite(pin,LOW); 
}
void loop(void)
{
  if(oil_var<=100)
  {
    flag=true;
  digitalWrite(pin,HIGH);
  delay(2000);
  }
  else flag=false;
  digitalWrite(pin,LOW);
   oil.requestTemperatures();
 delay(40);
 
 Serial.print("oil ");
 Serial.println(oil.getTempCByIndex(0));
 delay(200);
Serial.println();

 ow_oil.reset_search();

 if(!oil.getAddress(oilAddress,0)){Serial.println("datchika oil net");}

 oil_var=oil.getTempCByIndex(0);
 Serial.print("oil_var= ");
 Serial.print(oil_var);
 Serial.println();
 lcd.setCursor(0,0);
 lcd.print("Oil= ");
 lcd.print(oil_var);
 lcd.setCursor(0,1);
 lcd.print("nagrew = ");
 lcd.print(flag);
 delay(2000);
 
}

