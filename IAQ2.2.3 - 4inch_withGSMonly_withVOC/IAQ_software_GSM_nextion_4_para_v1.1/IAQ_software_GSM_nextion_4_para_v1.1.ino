


#include "Adafruit_SHT31.h"     //For Temperature and humidity
#include<Wire.h>                //Prerequisite to i2c communications
#include<EEPROM.h>              //To save the data at ROM
#include <RtcDS3231.h>
#include<SPI.h>                 //To save the data in SD. Prerequisite to SD.h
#include <SdFat.h>


#include "Adafruit_CCS811.h"
#include <SoftwareSerial.h>
#include <TimerOne.h>
SoftwareSerial mySerial(A14, A15);

#define gsmPowerPin 23 // gsm power pin 
#define SerialConnect Serial


//#define ID 1
//#define ModelNo "IAQ W A6"
//#define OSVer "ZNB 2.0.3"
//#define PID "IAQ99999999"
//#define SER "123"

#define ID "IAQ10001165"
#define ModelNo "IAQ-G-A6"
#define OSVer "EES 3.0.1"
#define SER "1912004100340"

//const int ID = 2;
#define pmSerial Serial1
#define co2Serial Serial2
#define displaySerial Serial3

#define ssPin 53
#define sdPower 6    // sd card power pin
#define motorPin 35
#define dispPower 28
#define batPin A4
String IMEI;
String APN = "NAN";

int counter, counter2;
// #1 for keypad
#define MENU 36
#define NEXT 37
#define ENTER 40
#define EXIT 41






RtcDS3231<TwoWire> Rtc(Wire);
Adafruit_SHT31 sht31 = Adafruit_SHT31();
Adafruit_CCS811 ccs;



SdFat sd1, sd;
SdFile file;
SdFile dirFile;


const char* apn_array[] = {"airtelgprs.com", "bsnlnet", "imis/internet", "live.vodafone.com", "TATA.DCOMO.INTERNET", "hologram", "Jionet", "Not Avlble"};
String apnMenu[] = {"SELECT NETWORK :", "AIRTEL", "BSNL", "IDEA", "VODAFONE", "TATA DOCOMO", "HOLOGRAM", "JIO 4G", "Custom Network"};
String apn = "airtelgprs.com";
int apnIndex;
bool isSim = false;
bool startUp = true;
const uint16_t no_para = 7;
enum para_enum {TEM, HUM, PM_1, PM_2_5, PM_10, CO2, VOC};
String para[no_para] = {"Temperature", "Humidity", "PM 1", "PM 10", "PM 2.5", "CO2", "VOC"};


String para_unit[no_para] = {"*C", "%", "ug/m^3", "ug/m^3", "ug/m^3", "PPM", "PPB"};
String para_val_str[no_para] = {"0", "0", "0", "0", "0", "0", "0"};
float para_val[no_para];

uint16_t au16data[16];
int8_t state = 0;
uint8_t logInterval;
bool ask_pm = true;
bool softConnect = false;
//Variable Declarations for PM Sensors
byte ask[] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

byte setMode[] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
byte qaMode[] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46};


//Variable Declarations for CO2 Sensors

byte zero_calib[] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};
//byte qaMode_c[] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46};


int pm1, pm2_5, pm10, co2, voc, co;
int co_raw;
float temp, hum, co_ppm, voc_ppm;
String Time;
long int sumVOC = 0;
int countVOC = 0;
float coShift = 0;

float Voltage1 = 0;
float Voltage2 = 0;
int LI;
int preMinute = 10, preMinute1;

const uint16_t nMax = 50;
int fileIndex = 0; // Position of file's directory entry.
uint16_t dirIndex[nMax];
bool checkBox[nMax];
String line = "";
int choice;
uint16_t n = 0;
bool enter = true;
String SelectedFiles[50], network;
bool softFlag = false;
bool homePageFlag = false;
String sendThis = "";
int k = 0;
long int lt4;
bool inibat = true;
int preBat, vb;
float v;
float EMA_S, EMA_a = 0.2;
String Message;

File myFile;

long int lt, ltv, lts;
void setup() {
  pinMode(batPin, INPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(dispPower, OUTPUT);
  digitalWrite(dispPower, HIGH);
  digitalWrite(motorPin, HIGH);

  pinMode(sdPower, OUTPUT);
  digitalWrite(sdPower, HIGH);

  Rtc.Begin();
  Serial.begin(9600);
  SdInit(ssPin);
  Message = "\nstart up---\n" + getDateTimeNow() + "\n";
  Serial.print(Message);
  saveLog(Message);



  pmSerial.begin(9600);
  displaySerial.begin(9600);
  co2Serial.begin(9600);
  co2Serial.write(ask, 9);

  pmSerial.write(qaMode, 9);
  Message = "-------------Initializing modem...";
  Serial.print(Message);
  saveLog(Message);

  String Info = modemInit();

  Message = Info + "\n";
  Serial.print(Message);
  saveLog(Message);

  IMEI = getIMEI();
  if (Info.indexOf("OK") > -1) {
    isSim = true;
  }
  else turnOff_modem();
  if (EEPROMRead(40) == "NaN") {
    EEPROMWrite("", 40);
  }
  apn = EEPROMRead(40);
  //  if (EEPROM.read(3) == 255) EEPROM.write(3, 15);
  //  LI = EEPROM.read(3);
  LI = 15;
  if (EEPROM.read(4) == 255) EEPROM.write(4, 0);
  coShift = EEPROM.read(4);


  //sht31.begin(0x44);             // temp hum begin
  ccs.begin();


  lt = millis() - 7500;
  getPM();
  getCO2();
  pmSerial.write(ask, 9);
  co2Serial.write(ask, 9);
  //SdInit(ssPin);

  preMinute = getMinute();
  preMinute1 = preMinute;
  sendInfo();

  Message = "Setup Done\n";
  Serial.print(Message);
  saveLog(Message);
}

void loop() {
  
  getPM();
  getCO2();

  if (!softFlag) {

    if (millis() - lt > 7500) {
      Time = getTime();

      Message = getTimeNow() + "\n";
      Serial.print(Message);
      saveLog(Message);

      if (ask_pm) {

        //Serial.println(Time);
        v = getBatteryVolt();
                //temp = sht31.readTemperature();
                //hum = sht31.readHumidity();

        para_val[TEM] = temp;
        para_val[HUM] = hum;



        updateNetworkPic();


        ask_pm = false;
      } else {
        voc = sumVOC / countVOC;
        if (voc < 0) voc = 0;
        voc_ppm = round(voc / 100.0) / 10.0;
        para_val[VOC] = voc;
        sumVOC = 0;
        countVOC = 0;
        //readCO();
        //logCO();
        co2Serial.write(ask, 9);
        pmSerial.write(ask, 9);



        //printVoltages();

        //co2Serial.write(ask, 9);
        ask_pm = true;
      }

      if (homePageFlag) {
        Message = "passing values..\n";
        Serial.print(Message);
        saveLog(Message);
        sendDisplayVal();
      }
      if (startUp) {
        saveToSD();
        //startUp = false;
        homePageFlag = true;
        sendThis = "page homepage";
        writeString(sendThis);
      }

      lt = millis();
    }


    //handleSerial();
    int nowMinute = getMinute();
    if ((nowMinute % LI == 0) & (preMinute1 != nowMinute)) {
      preMinute1 = nowMinute;
      if (saveToSD()) {
         Message = "Data saved\n";
      } else {
        Message = "Unable to save data\n";
      }
      
      Serial.print(Message);
      saveLog(Message);
    }
    handleDataSend(nowMinute);


  } else {
    //Serial.println("---------------");
  }



  getVOC();


  if (softFlag & millis() - lt > 30000 ) {
    softFlag = false;
    sendThis = "page homepage";
    writeString(sendThis);
  }


  handleDisplay();
  handlesoftConnect();

  if (!homePageFlag & millis() - lt4 > 30000) {
    sendThis = "page homepage";
    writeString(sendThis);
    //Serial.println(page
  }

}   // END of loop-------------------------------







void getPM() {
  if (pmSerial.available()) {
    byte response[9];
    int i = 0;
    long int t = millis();
    while (millis() - t < 100) {
      while (pmSerial.available()) {
        response[i++] = pmSerial.read();
      }
    }
    pm1 = ((response[6] << 8) + response[7]);
    pm2_5 = ((response[2] << 8) + response[3]);
    pm10 = ((response[4] << 8) + response[5]);
    para_val[PM_1] = pm1;
    para_val[PM_2_5] = pm2_5;
    para_val[PM_10] = pm10;

  }
}

void getCO2() {
  if (co2Serial.available()) {
    byte response[9];
    int i = 0;
    long int t = millis();
    while (millis() - t < 100) {
      while (co2Serial.available()) {
        response[i++] = co2Serial.read();
        //        Serial.print(response[i - 1], HEX);
        //        Serial.print(" ");
      }
    }
    //Serial.println("wake");
    co2 = ((response[2] << 8) + response[3]);
    para_val[CO2] = co2;
    //    Serial.print("CO2 :"); Serial.println(co2);
  }
}



void getVOC() {
  if (millis() - ltv > 500) {
    if (ccs.available()) {
      float t = ccs.calculateTemperature();
      if (!ccs.readData()) {
        sumVOC = sumVOC = ccs.getTVOC();
        countVOC++;
      }
    }
    ltv = millis();
  }
}




//Voltage level of the battery
int voltLevel(float volt) {
  float buf = 0.1, val;
  if (volt > 7.2 + buf) val = 3;
  else if (volt > 6.5 + buf & volt <= 7.2 - buf) val = 2;
  else if (volt > 5.5 + buf & volt <= 6.5 - buf) val = 1;
  else if (volt <= 5.5 - buf ) val = 0;
  else val = preBat;
  preBat = val;
  return val;
}

//Measure the voltage status of the battery at initial
int voltStatus(float volt) {
  //EMA_S = analogRead(batPin) * 5 / 1023;
  int val;
  if (EMA_S > 7.2) val = 3;
  else if (volt > 6.5 & volt <= 7.2) val = 2;
  else if (volt > 5.5 & volt <= 6.5) val = 1;
  else val = 0;
  return val;
}


float getBatteryVolt() {
  long int n = 0;
  float volt;
  for (int i = 0; i < 10; i++)
  {
    n += analogRead(batPin);
  }
  if (EMA_S == 0) EMA_S = analogRead(batPin) * 5.0 / 1023 * 3;
  Serial.print("bat voltage = "); Serial.println(n / 10.0 * 5.0 / 1023.0 * 3);
  volt = n / 10.0 * 5.0 / 1023 * 3;
  EMA_S = (EMA_a * volt) + ((1 - EMA_a) * EMA_S);
  Serial.println(EMA_S);
  return EMA_S;
}
