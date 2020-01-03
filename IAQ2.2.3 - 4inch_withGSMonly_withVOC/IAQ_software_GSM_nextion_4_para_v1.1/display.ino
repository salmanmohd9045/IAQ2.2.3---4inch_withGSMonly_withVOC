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
      response1.remove(0, response1.indexOf("$") + 1);
      
      Message = "Display message: " + response + "\n";
      Serial.print(Message);
      saveLog(Message);
      response.trim();
      if (response == "Homepage") {
        if(startUp) startUp = false;
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
      if (response == "Menu") {

      } else if (response == "Splash") {
        //sendInfo();

      } else if (response == "Setclock") {
        displayTime();
        //delay(5000);

      } else if (response.indexOf("Time") > -1) {
        response.remove(0, response.indexOf(":") + 1);
        updateTime(response,"hh:mm");
        sendDisplayVal();

      } else if (response == "Setup") {
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
      } else if (response == "Co2Calib") {
        co2Serial.write(zero_calib, 9);
        delay(1500);
        getCO2();
        sendThis = "dialog.dialHead.txt=\"Message\"";
        writeString(sendThis);
        sendThis = "dialog.dialMsg.txt=\"Calibration Done\"";
        writeString(sendThis);
        sendThis = "page dialog";
        writeString(sendThis);
        sendThis = "calibCO2.Calib_pwd.txt=\"\"";
        writeString(sendThis);
        delay(1500);
        sendThis = "page homepage";
        writeString(sendThis);
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
  sendThis = "page homepage";
  writeString(sendThis);
}

void updateNetworkPic() {
  if (isSim) {
    if (network == "" | network == "SEARCHING" | network == "Ssearching") {
      network = getSPN();
      network.toUpperCase();
      sendThis = "homepage.txtMsg.txt=\"" + network + "\"";
      writeString(sendThis);
      sendThis = "homepage.txtMsg.pco=0";
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
      writeString("btwifi.pic=97");
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
    } else if(str == "searching") {
      writeString("btwifi.pic=97");
    }
    else {
      writeString(getPicString(str.toInt()));

    }
  }
}


String getPicString(int stren) {
  String ret = "btwifi.pic=";
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
//  sendThis = "page homepage";
//  writeString(sendThis);
  if (!isSim) writeString("btwifi.pic=97");
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

  sendThis = "c02Val.txt=";
  sendThis.concat("\"");
  sendThis.concat(String(co2));
  sendThis.concat("\"");
  writeString(sendThis);

  sendThis = "vocVal.txt=";
  sendThis.concat("\"");
  sendThis.concat(String(voc));
  sendThis.concat("\"");
  writeString(sendThis);

  sendThis = "pm2Val.txt=";
  sendThis.concat("\"");
  sendThis.concat(String(pm2_5));
  sendThis.concat("\"");
  writeString(sendThis);

  sendThis = "pm10Val.txt=";
  sendThis.concat("\"");
  sendThis.concat(String(pm10));
  sendThis.concat("\"");
  writeString(sendThis);

  //Update Time
  sendThis = "timeVal.txt=\"" + getTime()  + "\"";
  writeString(sendThis);
  sendThis = "dateVal.txt=\"" + getDate()  + "\"";
  writeString(sendThis);
  if ( k == 0) {
    sendThis = "p3.pic=110";
    writeString(sendThis);
    sendThis = "value.txt=\"" + String(co2)  + "\"";
    writeString(sendThis);
    sendThis = "p4.pic=113";
    writeString(sendThis);

  } else if ( k == 1) {
    sendThis = "p3.pic=111";
    writeString(sendThis);
    sendThis = "value.txt=\"" + String(pm2_5)  + "\"";
    writeString(sendThis);
    sendThis = "p4.pic=112";
    writeString(sendThis);

  } else if ( k == 2) {
    sendThis = "p3.pic=109";
    writeString(sendThis);
    sendThis = "value.txt=\"" + String(pm10)  + "\"";
    writeString(sendThis);
    sendThis = "p4.pic=112";
    writeString(sendThis);

  } else if( k == 3) {
    sendThis = "p3.pic=114";
    writeString(sendThis);
    sendThis = "value.txt=\"" + String(voc)  + "\"";
    writeString(sendThis);
    sendThis = "p4.pic=115";
    writeString(sendThis);
  }
  k++;
  if ( k > 3) k = 0;



  if (inibat) {
    preBat = voltStatus(v);
    inibat = false;
  }
  vb = voltLevel(v);

  if (vb >= 3) {
    sendThis = "btBatt.pic=0";
    writeString(sendThis);
  } else if (vb >= 2) {
    sendThis = "btBatt.pic=1";
    writeString(sendThis);
  } else if (vb >= 1) {
    sendThis = "btBatt.pic=2";
    writeString(sendThis);
  } else  {
    sendThis = "btBatt.pic=3";
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
