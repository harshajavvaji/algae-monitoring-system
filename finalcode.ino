#include <RTC.h>

//#include <RTClib.h>



#include <DFRobot_PH.h>

#include <Wire.h>

#include <EEPROM.h>

#include <OneWire.h>

#include <DallasTemperature.h>

#include <SPI.h>

#include <SD.h>

#include <LiquidCrystal_I2C.h>

static DS3231 RTC;

int ho,sec,mi ;




File myFile;

#define ONE_WIRE_BUS A2                           // A2 pin is used for TEMPERATURE SENSOR

#define PH_PIN A1                                // A1 pin is used for PH SENSOR

int NTUPin = A0;                                 // A0 pin is used for TURBIDITY SENSOR

OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

LiquidCrystal_I2C lcd(0x27, 16, 2);               // I2C address 0x27, 16 column and 2 rows

//RTC_DS1307 RTC;

DFRobot_PH ph;


int count=0;

float volt,avgt=0,avgntu=0,sumt=0,sumntu=0;

float ntu;

float voltage,phValue,temperature,t;





void setup () {
   Serial.begin(9600);

   
   
   Wire.begin();
   
   RTC.begin();
   
   ph.begin();
if (SD.begin())
{
  Serial.println("Ready");
}
else
{
  Serial.println("Failed");
  return;
}
myFile = SD.open("Data.txt",FILE_WRITE);

if(myFile)
  {
  myFile.println("Timestamp  | Temperature | phValue | Voltage | NTU | TempAvg | NTUAvg");
  myFile.close();
  }
  else
  {
    Serial.print("Memory card not found");
  }

 
}
void loop () {


  if (RTC.isRunning())
  {
    //Serial.print(RTC.getDay());
    //Serial.print("-");
    //Serial.print(RTC.getMonth());
    //Serial.print("-");
    //Serial.print(RTC.getYear());
    //Serial.print(" ");
    //Serial.print(RTC.getHours());
    ho=RTC.getHours();
    //Serial.print(":");
    //Serial.print(RTC.getMinutes());
    mi=RTC.getMinutes();
    //Serial.print(":");
    //Serial.print(RTC.getSeconds());
    sec=RTC.getSeconds();
    //Serial.print("");
    if (RTC.getHourMode() == CLOCK_H12)
    {
      switch (RTC.getMeridiem()) {
      case HOUR_AM:
        //Serial.print(" AM");
        break;
      case HOUR_PM:
        //Serial.print(" PM");
        break;
      }
    }
    Serial.println("");
  delay(1000);
  }
  else
  {
    delay(200);
    Serial.println("Setting Time");
    RTC.setHourMode(CLOCK_H12);
    //RTC.setHourMode(CLOCK_H24);
    RTC.setDateTime(__DATE__, __TIME__);
    Serial.println("New Time Set");
    RTC.startClock();
  }


     Serial.print(__DATE__);
      Serial.print("   |   ");
      //Serial.print(__TIME__);
      Serial.print(ho);
      Serial.print(":");
      Serial.print(mi);
      Serial.print(":");
      Serial.print(sec);
    Serial.println();

   



   static unsigned long timepoint = millis();      // this block gives the PHvalue from the ph sensor
    if(millis()-timepoint>1000U){                  //time interval: 1s
        timepoint = millis();
        temperature = readTemperature();           // read your temperature sensor to execute temperature compensation
        voltage = analogRead(PH_PIN)/1024.0*5000;  // read the voltage
        phValue = ph.readPH(voltage,temperature);  // convert voltage to pH with temperature compensation
        Serial.print("temperature:");
        Serial.print(temperature,1);
        Serial.print("^C  pH:");
        Serial.println(phValue,2);
        Serial.println();
    }
    ph.calibration(voltage,temperature);


    volt = 0;
    
    for(int i=0; i<800; i++)
    {
        volt += ((float)analogRead(NTUPin)/1023)*5;
    }
    volt = volt/800;
    volt = round_to_dp(volt,2);
    
    ntu = (-1120.4*square(volt)+5742.3*volt-4353.8);  // NTU calculation
    
    
    Serial.print(volt);
    Serial.print(" V ");
    Serial.print(" NTU :");
    Serial.print(ntu);
    Serial.println();
     
     

   // count+=1;
   // sumt+=temperature;
   // avgt=sumt/count;           // Finds the average of temperature values from temperature sensor 
   // sumtntu+=ntu;
   // avgntu=sumntu/count;       // Finds the average of NTU values from TURBIDITY sensor 
    
   //Serial.println(avgt); 
   //Serial.println(avgntu);



  lcd.init();                  // initialize the lcd
  lcd.backlight();
  
  lcd.setCursor(0, 0);         // move cursor to   (0, 0)
   
  lcd.print(temperature);      // print message at (0, 0)
  lcd.print("C ");
  lcd.print(phValue);
  lcd.print("ph");
  
  lcd.setCursor(0,1);         // move cursor to   (2, 1)
  
  lcd.print(volt);            // print message at (2, 1)
  lcd.print("V ");
  lcd.print(ntu);
  lcd.print("NTU");



   myFile = SD.open("Data.txt",FILE_WRITE); // This block of code prints the sensor values to file saved in SD card  
    
    if (myFile) 
    {
         
   //myFile.print(now.year(), DEC);
   //myFile.print('/');
   //myFile.print(now.month(), DEC);
   //myFile.print('/');
   //myFile.print(now.day(), DEC);
   //myFile.print(' ');                        // This is printing TIMESTAMP
   //myFile.print(now.hour(), DEC);
   //myFile.print(':');
   //myFile.print(now.minute(), DEC);
   //myFile.print(':');
   //myFile.print(now.second(), DEC);
   myFile.print(__DATE__);
      myFile.print("   |   ");
      //Serial.print(__TIME__);
      myFile.print(ho);
      myFile.print(":");
      myFile.print(mi);
      myFile.print(":");
      myFile.print(sec);
   myFile.print(" | ");
   myFile.print(t);
   myFile.print("^C  | ");
   myFile.print(phValue);
   myFile.print(" | ");
   myFile.print(volt);
   myFile.print("V  | ");
   myFile.print(ntu);
   //myFile.print(" | ");
   //myFile.print(avgt);
   //myFile.print(" | "); 
   //myFile.print(avgntu);
   myFile.println();
   
   myFile.close();
      
   }

   else
   { Serial.print("Memory card not found");
   }
}

float readTemperature()
{
  //this block of code gives the temperature from the temperature sensor
  
  sensors.requestTemperatures();
 
  t=sensors.getTempCByIndex(0);
  return t;
  
}



float round_to_dp( float in_value, int decimal_place )//This block belongs to caluclation of NTU value from turbidity sensor
{
  float multiplier = powf( 10.0f, decimal_place );
  in_value = roundf( in_value * multiplier ) / multiplier;
  return in_value;
}
