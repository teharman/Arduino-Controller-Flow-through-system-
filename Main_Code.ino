
//These are the needed libraries for this project
//Add GravityRtc.h and Wire.h libraries by going to "Sketch" --> "Include Library" --> "Add .ZIP Library" and select the appropriate .ZIP folder
#include <DallasTemperature.h>
#include <OneWire.h>
#include "GravityRtc.h"
#include "Wire.h"
#include <SD.h>
#include <SPI.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(26, 27, 34, 35, 36, 37); //these three lines of code are for operating the LCD screen
#define LCD_BACKLIGHT_PIN 28
int brightness = 128;

const int switchPin = 2;  //these six lines of code are for the conductivity sensors
int switchState = 0;
int condVal;
const int switchPin1 = 3;
int switchState1 = 0;
int condVal1;

GravityRtc rtc;    //function to setup the real time clock
File myFile;       //name of file function --> needed to write data to SD card

int pinCS = 53;  //chipSelect for SD card

int digitalspeak = 8;   //Pin Digital Speaker is connected to
int temp_sensor = 5;   //Pin DS18B20 Sensor 1 is connected to
int temp_sensor2 = 6;   //Pin DS18B20 Sensor 2 is connected to

float temperature = 0;   //Variable to store the temperature 1 in
float temperature1 = 0;   //Variable to store the temperature 2 in

int lowerLimit = 15;  //define the lower threshold for temperature
int higherLimit = 30;  //define the higher threshold for temperature
int pHLLimit = 7.89;   //define the lower threshold for pH
int pHHLimit = 8.60;   //define the higher threshold for pH

OneWire oneWirePin(temp_sensor);
OneWire oneWirePin2(temp_sensor2);

DallasTemperature sensors(&oneWirePin);
DallasTemperature sensors2(&oneWirePin2);


//----- this section is for setting up the pH sensors ---------
#define SensorPin 0
unsigned long int avgValue;
float b;
int buf[10],temp;

#define SensorPin1 1
unsigned long int avgValue1;
float b1;
int buf1[10],temp1;
//-------------------------------------------------------------

void setup(void) {
  // put your setup code here, to run once:
  Serial.begin(9600);

  lcd.begin(16,2);

  analogWrite(LCD_BACKLIGHT_PIN, 255);

  rtc.setup();

  rtc.adjustRtc(F(__DATE__), F(__TIME__));

  pinMode(pinCS, OUTPUT);

  pinMode(switchPin, INPUT);
  pinMode(switchPin1, INPUT);
  
  // SD Card Initialization
  if (SD.begin())
  {
    Serial.println("SD card is ready to use.");
  } else
  {
    Serial.println("SD card initialization failed");
    return;
  }
  
  pinMode (digitalspeak, OUTPUT);

  pinMode(13, OUTPUT);

  Serial.println("Ready");

  sensors.begin();
  sensors2.begin();
  rtc.read();
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.print("Requesting Temperatures from sensors: ");
  sensors.requestTemperatures();
  sensors2.requestTemperatures();
  Serial.println("DONE");

  temperature = sensors.getTempCByIndex(0);
  temperature1 = sensors2.getTempCByIndex(0);

  digitalWrite(digitalspeak, LOW);

  Serial.print(" Temp 1: ");
  Serial.print(temperature);

  Serial.print("| Temp 2: ");
  Serial.print(temperature1);

//---------------------------------------------------
  for(int i=0;i<10;i++)
  {
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6;
  float phValue0=(float)3.5*phValue;
  Serial.print("|    pH1:");
  Serial.print(phValue0,2);
  Serial.println(" ");
  delay(500);
//----------------------------------------------------
   for(int i=0;i<10;i++)
  {
    buf1[i]=analogRead(SensorPin1);
    delay(10);
  }
  for(int i=0;i<9;i++)
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf1[i]>buf1[j])
      {
        temp1=buf1[i];
        buf1[i]=buf1[j];
        buf1[j]=temp1;
      }
    }
  }
  avgValue1=0;
  for(int i=2;i<8;i++)
    avgValue1+=buf1[i];
  float phValue1=(float)avgValue1*5.0/1024/6;
  float phValue01=(float)3.5*phValue1;
  Serial.print("|    pH2:");
  Serial.print(phValue01,2);
  Serial.println(" ");
  delay(500);
//----------------------------------------------------

//----------------------------------------------------
condVal = analogRead(A2);
float voltage=condVal*(5.0/1023.0);

condVal1 = analogRead(A3);
float voltage1=condVal1*(5.0/1023.0);

Serial.print("|     Rel Conduct 1:");
Serial.print(voltage);
Serial.print("|     Rel Conduct 2:");
Serial.print(voltage1);
delay(500);
//----------------------------------------------------

  //Setup the speaker to act as output

  if(temperature <= lowerLimit){
    Serial.println(", Digital speaker is Activated");
    tone(digitalspeak,3000,1000);
  }
  else if(temperature >= higherLimit){
    Serial.println(", Digital speaker is Activated");
    tone(digitalspeak,3000,1000);
  }
  if(temperature1 <= lowerLimit){
    Serial.print(", Digital speaker is Activated");
    tone(digitalspeak,3000,1000);
  }
  else if(temperature1 >= higherLimit){
    Serial.print(", Digital speaker is Activated");
    tone(digitalspeak,3000,1000);
  }

  String dataString = "";  //This is to combine all sensors into one so that it can be read and inputted into the SD card as data

  dataString += String(temperature);
  dataString += ",";
  dataString += String(temperature1);
  dataString += ",";
  dataString += String(phValue0);
  dataString += ",";
  dataString += String(phValue01);
  dataString += ",";
  dataString += String(voltage);
  dataString += ",";
  dataString += String(voltage1);
  dataString += ",";
 
  myFile = SD.open("Data1.txt", FILE_WRITE);  //"Data1.txt" can be rewritten to anything else, such as Data2.txt or Data1.csv
  if (myFile){
    myFile.print(rtc.year);
    myFile.print("/");//month
    myFile.print(rtc.month);
    myFile.print("/");//day
    myFile.print(rtc.day);
    myFile.print(",");//hour
    myFile.print(rtc.hour);
    myFile.print(":");//minute
    myFile.print(rtc.minute);
    myFile.print(":");//second
    myFile.println(rtc.second);
    myFile.print(",");
    myFile.println(dataString);
    myFile.close();
  }
  // if the file didn't open, print an error:
  else {
    Serial.println("error opening txt file");    //This shows up on the serial monitor if an error has occurred (i.e. no SD card in slot)
  }
  delay(1000);   //How often the data will be read to the SD card

  {

    lcd.print("Tank 1:");
    delay(2000);
    lcd.clear();
    lcd.print("Temperature: ");
    delay(2000);
    lcd.clear();
    lcd.print(temperature);
    lcd.print("C");
    delay(2000);
    lcd.clear();
    lcd.print("pH: ");
    delay(2000);
    lcd.clear();
    lcd.print(phValue0);
    delay(2000);
    lcd.clear();
    lcd.print("Rel Conduct: ");
    delay(2000);
    lcd.clear();
    lcd.print(voltage);
    delay(2000);
    lcd.clear();
    lcd.print("Tank 2:");
    delay(2000);
    lcd.clear();
    lcd.print("Temperature: ");
    delay(2000);
    lcd.clear();
    lcd.print(temperature1);
    lcd.print("C");
    delay(2000);
    lcd.clear();
    lcd.print("pH: ");
    delay(2000);
    lcd.clear();
    lcd.print(phValue01);
    delay(2000);
    lcd.clear();
    lcd.print("Rel Conduct: ");
    delay(2000);
    lcd.clear();
    lcd.print(voltage1);
    delay(2000);
    lcd.clear();
  }
  
}
