void handleDisplay() {

  if (displaySerial.available()) {
    lt = millis();
    int resp[20];
    int i = 0;
    byte endByte[3];
    bool startMessage = false;
    String response1;
    while (millis() - lt < 50) {
      while (displaySerial.available()) {
        if (displaySerial.available()) {
          byte b = displaySerial.read();
          response1 += (char)b;
        }
      }
    }
    while (response1.indexOf("$") > -1) {
      response1.remove(0, response1.indexOf("$") + 1);
      String response = response1.substring(0, response1.indexOf("$"));
      //response1.remove(0, response1.indexOf("$") + 1);

      Message = "Display message: " + response + "\n";
      Serial.print(Message);
      saveLog(Message);
      response.trim();
      if (response == F("Homepage")) {
        homePageFlag = true;
        softFlag = false;
        //if (!startUp)
        sendDisplayVal();
        // if (startUp) startUp = false;
      } else {
        homePageFlag = false;
        softFlag = true;
        lt4 = millis();
      }
      if (response == F("Menu")) {

      } else if (response == F("Splash")) {
        //sendInfo();

      } else if (response == F("Setclock")) {
        displayTime();

      } else if (response.indexOf("Time") > -1) {
        response.remove(0, response.indexOf(":") + 1);
        updateTime(response);
        sendDisplayVal();

      } else if (response == F("Setup")) {
        //apn = "abc";
        //Serial.println("sending ssid");
        delay(50);
        sendThis = "t0.txt=\"" + apn + "\"";
        writeString(sendThis);
      } else if (response.indexOf("APN") > -1) {
        response.remove(0, response.indexOf(":") + 1);
        apn = response;
        EEPROMWrite(apn, 40);
        displayDialog();
      } else if (response == F("Co2Calib")) {
        co2Serial.write(zero_calib, 9);
        delay(1500);
        getCO2();
        sendThis = F("dialog.dialHead.txt=\"Message\"");
        writeString(sendThis);
        sendThis = F("dialog.dialMsg.txt=\"Calibration Done\"");
        writeString(sendThis);
        sendThis = F("page dialog");
        writeString(sendThis);
        sendThis = F("calibCO2.Calib_pwd.txt=\"\"");
        writeString(sendThis);
        delay(1500);
        sendThis = F("page homepage");
        writeString(sendThis);
      } else if (response.indexOf(F("calibTemp")) > -1 ) {
        response.remove(0, response.indexOf(":") + 1);
        response = response.substring(0,response.indexOf(":"));

        tempShift = sht31.readTemperature()-response.toFloat();
        
        EEPROM.put(TEMP_SHIFT_ADDR, tempShift);
        temp = sht31.readTemperature();
        temp = temp - tempShift;
        
          para_val[TEM] = temp;

        Serial.println("temp shift: "); Serial.println(tempShift);
      }
    }
  }
}


void displayDialog() {
  sendThis = "page dialog";
  writeString(sendThis);
  sendThis = "dialog.dialHead.txt=\"Message\"";
  writeString(sendThis);
  sendThis = "dialog.dialMsg.txt=\"Updated Successfully\"";
  writeString(sendThis);
  delay(2000);
  sendThis = F("page homepage");
  writeString(sendThis);
}

void updateNetworkPic() {
  if (isSim) {
    if (network == "") {
      network = getSPN();
      network.toUpperCase();
      sendThis = "homepage.txtMsg.txt=\"" + network + "\"";
      writeString(sendThis);
      sendThis = F("homepage.txtMsg.pco=0");
      writeString(sendThis);
      //       sendThis = "homepage.txtMsg.xcen=\"Center\"";
      //      writeString(sendThis);
      Message = "Network: " + network + "\n";
      Serial.print(Message);
      saveLog(Message);
      //Serial.print("Network: "); Serial.println(network);
    }
    String str = getStrength();
    Message = "Strength: " + str + "\n";
    Serial.print(Message);
    saveLog(Message);
    if (str == "error") {
      writeString(F("btwifi.pic=97"));
      turnOff_modem();
      delay(50);
      //isSim = false;
      if (modemInit().indexOf("OK") > -1) {
        isSim = true;
      }
      else {
        turnOff_modem();
        isSim = false;
      }
    }
    else {
      writeString(getPicString(str.toInt()));

    }
  }
}


String getPicString(int stren) {
  String ret = F("btwifi.pic=");
  if (stren < 10) {
    ret += "98";
  } else if ( stren >= 10 & stren < 18) {
    ret += "99";
  } else if ( stren >= 18 & stren < 28) {
    ret += "100";
  } else if ( stren >= 28 ) {
    ret += "101";
  }
  return ret;
}




void displayTime() {
  RtcDateTime now = Rtc.GetDateTime();
  sendThis = "hourVal.val=" + String(now.Hour());
  writeString(sendThis);
  sendThis = "minVal.val=" + String(now.Minute());
  writeString(sendThis);
  sendThis = "dayVal.val=" + String(now.Day());
  writeString(sendThis);
  sendThis = "monVal.val=" + String(now.Month());
  writeString(sendThis);
  sendThis = "yearVal.val=" + String(now.Year() - 2000);
  writeString(sendThis);
}






void sendInfo() {
  sendThis = "prodinfo.txtPID.txt=\"" + String(ID) + "\"";
  writeString(sendThis);
  sendThis = "prodinfo.txtIMEI.txt=\"" + IMEI + "\"";
  writeString(sendThis);
  sendThis = "prodinfo.txtModelNo.txt=\"" + String(ModelNo) + "\"";
  writeString(sendThis);
  sendThis = "prodinfo.txtOSVer.txt=\"" + String(OSVer) + "\"";
  writeString(sendThis);
  //  sendThis = "setupGSM.ssid.txt=\"" + apn + "\"";
  //  writeString(sendThis);
  //  sendThis = "btwifi.pic=59";
  //  writeString(sendThis);
  sendThis = F("page homepage");
  writeString(sendThis);
  if (!isSim) writeString(F("btwifi.pic=97"));
  //updateNextionRTC();
}

void sendDisplayVal() {
  //Serial.println(">>>>");

  //  sendThis = "tempVal.txt=";
  //  sendThis.concat("\"");
  //  sendThis.concat(String(temp, 1));
  //  sendThis.concat("\"");
  //  writeString(sendThis);

  //  sendThis = "humVal.txt=";
  //  sendThis.concat("\"");
  //  sendThis.concat(String(hum, 1));
  //  sendThis.concat("\"");
  //  writeString(sendThis);

  sendThis = F("c02Val.txt=");
  sendThis.concat("\"");
  sendThis.concat(String(co2));
  sendThis.concat("\"");
  writeString(sendThis);

  sendThis = F("vocVal.txt=");
  sendThis.concat("\"");
  sendThis.concat(String(voc));
  sendThis.concat("\"");
  writeString(sendThis);

  sendThis = F("pm2Val.txt=");
  sendThis.concat("\"");
  sendThis.concat(String(pm2_5));
  sendThis.concat("\"");
  writeString(sendThis);

  sendThis = F("pm10Val.txt=");
  sendThis.concat("\"");
  sendThis.concat(String(pm10));
  sendThis.concat("\"");
  writeString(sendThis);

  sendThis = F("temp.txt=");
  sendThis.concat("\"");
  sendThis.concat(String(temp, 1));
  sendThis.concat("\"");
  writeString(sendThis);

  sendThis = F("hum.txt=");
  sendThis.concat("\"");
  sendThis.concat(String(hum, 1));
  sendThis.concat("\"");
  writeString(sendThis);

  //Update Time
  sendThis = "timeVal.txt=\"" + getTime()  + "\"";
  writeString(sendThis);
  sendThis = "dateVal.txt=\"" + getDate()  + "\"";
  writeString(sendThis);
  if ( k == 0) {
    sendThis = F("p3.pic=110");
    writeString(sendThis);
    sendThis = "value.txt=\"" + String(co2)  + "\"";
    writeString(sendThis);
    sendThis = "p4.pic=113";
    writeString(sendThis);

  } else if ( k == 1) {
    sendThis = F("p3.pic=111");
    writeString(sendThis);
    sendThis = "value.txt=\"" + String(pm2_5)  + "\"";
    writeString(sendThis);
    sendThis = F("p4.pic=112");
    writeString(sendThis);

  } else if ( k == 2) {
    sendThis = F("p3.pic=109");
    writeString(sendThis);
    sendThis = "value.txt=\"" + String(pm10)  + "\"";
    writeString(sendThis);
    sendThis = F("p4.pic=112");
    writeString(sendThis);

  } else if ( k == 3) {
    sendThis = F("p3.pic=114");
    writeString(sendThis);
    sendThis = "value.txt=\"" + String(voc)  + "\"";
    writeString(sendThis);
    sendThis = F("p4.pic=115");
    writeString(sendThis);

  } else if ( k == 4) {
    sendThis = F("p3.pic=117");
    writeString(sendThis);
    sendThis = "value.txt=\"" + String(temp, 1)  + "\"";
    writeString(sendThis);
    sendThis = F("p4.pic=119");
    writeString(sendThis);

  } else {
    sendThis = F("p3.pic=116");
    writeString(sendThis);
    sendThis = "value.txt=\"" + String(hum, 1)  + "\"";
    writeString(sendThis);
    sendThis = F("p4.pic=118");
    writeString(sendThis);
  }
  k++;
  if ( k > 5) k = 0;



  if (inibat) {
    preBat = voltStatus(v);
    inibat = false;
  }
  vb = voltLevel(v);

  if (vb >= 3) {
    sendThis = F("btBatt.pic=0");
    writeString(sendThis);
  } else if (vb >= 2) {
    sendThis = F("btBatt.pic=1");
    writeString(sendThis);
  } else if (vb >= 1) {
    sendThis = F("btBatt.pic=2");
    writeString(sendThis);
  } else  {
    sendThis = F("btBatt.pic=3");
    writeString(sendThis);
  }

}


void writeString(String stringData) {
  for (int i = 0; i < stringData.length(); i++)
    displaySerial.write(stringData[i]);

  displaySerial.write(0xff);
  displaySerial.write(0xff);
  displaySerial.write(0xff);
}
