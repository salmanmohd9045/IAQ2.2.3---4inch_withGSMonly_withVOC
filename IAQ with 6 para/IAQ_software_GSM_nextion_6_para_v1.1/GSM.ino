
#define TINY_GSM_MODEM_SIM800

// Increase RX buffer if needed
//#define TINY_GSM_RX_BUFFER 512

//#include "E:/Arduino/weatther_1284_advance/libraries/TinyGSM/src/TinyGsmClient.h"
#include "TinyGsmClient.h"
#include<ArduinoJson.h>

// Uncomment this if you want to see all AT commands
//#define DUMP_AT_COMMANDS

// Uncomment this if you want to use SSL
//#define USE_SSL

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT mySerial


const char url[] = "customer.enggenv.com/iaqdevice_data.php";
//const char url[] = "https://ens9x3qspho9.x.pipedream.net/";

char JSONmessageBuffer[1000];
String longitude, latitude;
long int data = 0;

#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
TinyGsm modem(SerialAT);
#endif

void handleDataSend(int nowMinute) {
  if (isSim) {
    if (nowMinute % 2 == 0 && preMinute != nowMinute) {
      sendThis = "p1.pic=73";
      writeString(sendThis);
      preMinute = nowMinute;
      Message = "try Sending data..";
      Serial.print(Message);
      saveLog(Message);
      if (connectGPRS()) {
        if (sendData()) {
          Message = "data sent\n";
          Serial.print(Message);
          saveLog(Message);
        } else {
          Message = "unable to send\n";
          Serial.print(Message);
          saveLog(Message);
        }
      } else {
        delay(50);
        sendThis = "p1.pic=74";  //
        writeString(sendThis);
        Message = "unable to connect GPRS\n";
        Serial.print(Message);
        saveLog(Message);
      }
    }
  }
}

String modemInit() {
  pinMode(gsmPowerPin, OUTPUT);
  digitalWrite(gsmPowerPin, HIGH);
  //Serial.println("turning on modem");

  // Set GSM module baud rate
  SerialAT.begin(9600);
  delay(2000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  //SerialMon.print(F("Initializing modem..."));
  int i = modem.Init();   // -1:modem error, 0: sim not found, 1: sim ready, 2: sim locked
  if (i == -1) {
    return "Modem error";
    turnOff_modem();
  } else if ( i == 2) {
    return "sim locked";
    turnOff_modem();
  } else if ( i == 1) {
    return "OK";
  } else {

    return "sim not found";
    turnOff_modem();
  }
}





void getJason() {
  StaticJsonBuffer<500> JSONbuffer;   //Declaring static JSON buffer
  JsonObject& JSONencoder = JSONbuffer.createObject();

  //float v = getBatStatus();
  JSONencoder["temperature"] = String(para_val[TEM]);
  JSONencoder["humidity"] = String(para_val[HUM]);
  JSONencoder["voc"] = String((int)para_val[VOC]);
  JSONencoder["co_2"] = String((int)para_val[CO2]);
  JSONencoder["pm_2_5"] = String((int)para_val[PM_2_5]);
  JSONencoder["pm_10"] = String((int)para_val[PM_10]);
  JSONencoder["device-id"] = String(ID);

  JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println(JSONmessageBuffer);
}



bool connectGPRS() {
  if (!modem.waitForNetwork()) {

    return false;
  }

  if (!modem.isGprsConnected()) {
    String apn_chose;
    if (apn == "") {
      String n = modem.getSPN();
      n.trim();
      n.toUpperCase();
      //Serial.println(n);

      for (int i = 0; i < sizeof(apnMenu); i++) {
        if ( n == apnMenu[i]) {
          apn_chose = apn_array[i - 1];
          break;
        }
      }

      //    if (apn_chose == "") {
      //      if (apnIndex == sizeof(apn_array) - 1) {
      //        apn_chose = apn;
      //      } else apn_chose = apn_array[apnIndex];
      //    }
    } else {
      apn_chose = apn;
    }


    if (!modem.gprsConnect(apn_chose.c_str())) {
      //SerialMon.println(" fail");

      return false;
    }
    return true;
  } else return true;

}




bool sendData() {
  bool ret;
  if (!modem.httpInit(url)) {
    return;
  }
  getJason();
  String resp = modem.httpPost(JSONmessageBuffer);
  sendThis = "p1.pic=74";
  writeString(sendThis);
  if (resp.indexOf("Done") > -1) {
    ret = true;
    sendThis = "p2.pic=71";
    writeString(sendThis);
  } else if ( resp == "" | resp == "NAN") {

  }
  else {
    sendThis = "p2.pic=73";
    writeString(sendThis);
    ret = false;
  }
  delay(80);
  sendThis = "p2.pic=74";
  writeString(sendThis);

  modem.httpClose();
  return ret;
}


void turnOff_modem() {
  SerialAT.flush();
  SerialAT.end();
  digitalWrite (gsmPowerPin, LOW);
  pinMode(gsmPowerPin, INPUT);
  delay (50);
}


String getIMEI() {
  return modem.getIMEI();
}

String getSPN() {
  if (modem.getSimStatus() == 1) {
    if (!modem.waitForNetwork()) {
      return "error";
    }
    return modem.getSPN();
  } else return "error";
}


String getStrength() {
  if (modem.getSimStatus() == 1) {

    return String(modem.getSignalQuality());
  } else return "error";
}
