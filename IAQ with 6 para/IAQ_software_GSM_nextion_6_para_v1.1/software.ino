void handlesoftConnect() {
  if (SerialConnect.available()) {
    String s = SerialConnect.readStringUntil('\n');
    lt = millis();
    s.trim();
    //Serial.print("-----");Serial.println(s);

    int choice = s.substring(s.indexOf('$') + 1, s.indexOf(':')).toInt();
    s.remove(0, s.indexOf(':') + 1);
    switch (choice)
    {
      case 99: {
        printlogFile();
        break;
      } 
      case 98: {
        DeletelogFile();
        break;
      }
      case 101: {
          SerialConnect.println("$:$");
         
          sendThis = "dialog.dialHead.txt=\"Message\"";
          writeString(sendThis);
          sendThis = "dialog.dialMsg.txt=\"Software Connected\"";
          writeString(sendThis);
           sendThis = "page dialog";
          writeString(sendThis);
          softFlag = true;
          break;
        }

      case 103: {
          SdInit(ssPin);

          //          if (!sd1.begin(sdPin, SD_SCK_MHZ(50))) {
          //            SerialConnect.println("@100:#!:SD card is Missing or SPI problem");
          //          }

          if (!dirFile.open("/", O_READ)) {
            //  sd1.errorHalt("open root failed");
          }

          while (file.openNext(&dirFile, O_READ)) {
            // Skip directories and hidden files.
            if (!file.isSubDir() && !file.isHidden()) {
              // Save dirIndex of file in directory.
              dirIndex[n] = file.dirIndex();

              SerialConnect.write("@103:");
              file.printName(&SerialConnect);
              SerialConnect.println();
              delay(120);
            }
            file.close();
          }
          lt = millis();          //in case of long file copy
          dirFile.close();
          break;
        }

      case 104: {
          SdInit(ssPin);

          fileIndex = 0;
          int c;
          int count = s.toInt();

          s.trim();
          for (int i = 0; i < count; i++)
          {
            SerialConnect.println("@99:filename:" + SelectedFiles[fileIndex]);
            String fileName = SelectedFiles[fileIndex];
            fileName.trim();
            readFile(fileName);
            char last = 0;

            if (last != '\n') {
              //              SerialConnect.println();
            }
            file.close();
            fileIndex++;
          }
          SerialConnect.println("@101:terminate");
          for (int i = 0; i < count; i++) {
            SelectedFiles[i] = "";
            fileIndex = 0;
          }
          break;
        }

      //Saving the file names in array: SelectedFiles[]
      case 105: {
          SelectedFiles[fileIndex] = s;
          fileIndex++;
          break;
        }

      //Setting dateTime from Serial Software Module
      case 106: {
          if (s.indexOf("dt") > -1)
          {
            s.remove(0, s.lastIndexOf(':') + 1);
            int a[6];
            SerialConnect.println("@106:" + s);
            for (int i = 0; i < 6; i++)
            {
              a[i] = s.substring(0, s.indexOf(',')).toInt();
              s.remove(0, s.indexOf(',') + 1);
            }

            RtcDateTime compiled = RtcDateTime(a[0], a[1], a[2], a[3], a[4], a[5]);
            Rtc.SetDateTime(compiled);
          }
          else if (s.indexOf("log") > -1)
          {
            s.remove(0, s.lastIndexOf(':') + 1);
            //Serial.println(s);
            logInterval = s.toInt();
            //Serial.print("LI:"); Serial.println(LI);
            SerialConnect.println("@108:li:" + String(logInterval));
            EEPROM.write(LI, logInterval);
          }
          break;
        }

      //Get RTC Time for Software Module
      case 107:
        {
          RtcDateTime now = Rtc.GetDateTime();
          for (int i = 0; i < no_para; i++) {
            SerialConnect.println("@107:" + String(i) + ":" + para[i] + ":" + String(para_val[i]) + ":" + para_unit[i]);
            delay(50);
          }
          SerialConnect.println("@107:dt:" + getDateTime(now));
          Time = getTimeNow();
          temp = sht31.readTemperature() - 3.5;
          hum = sht31.readHumidity();

          para_val[TEM] = temp;
          para_val[HUM] = hum;
          pmSerial.write(ask, 9);
          voc = sumVOC / countVOC;
          if (voc < 0) voc = 0;
          voc_ppm = round(voc / 100.0) / 10.0;
          para_val[VOC] = voc_ppm;
          sumVOC = 0;
          countVOC = 0;
          //readCO();
          //logCO();


          //printVoltages();

          co2Serial.write(ask, 9);
          //if(digitalRead(gsmPowerPin)) SerialConnect.println("@110:stren:" + getStrength());
          break;
        }

      //Get Product Info for Software Module
      case 108:
        {
          RtcDateTime now = Rtc.GetDateTime();
          SerialConnect.println("@108:pro:" + String(ID));
          delay(50);
          SerialConnect.println("@108:ser:" + String(SER));
          delay(50);
          SerialConnect.println("@108:dt:" + getDateTime(now));
          delay(50);
          //          SerialConnect.println("@108:apn:" + apn);
          //          delay(50);
          SerialConnect.println("@108:li:" + String(logInterval));
          SerialConnect.println("@108:bat:" + String(getBatteryVolt()));
          break;
        }

      //Remove the file from SD(Internal Memory)
      case 109: {
          SdInit(ssPin);
          String filename = s;
          filename.toLowerCase();
          filename.trim();
          if (s.indexOf("@") > -1) {


            //            if (!sd1.begin(sdPin, SD_SCK_MHZ(50))) {
            //              SerialConnect.println("@100:#!:SD card is Missing or SPI problem");
            //            }

            if (!dirFile.open("/", O_READ)) {
              //  sd1.errorHalt("open root failed");
            }

            while (file.openNext(&dirFile, O_READ)) {
              // Skip directories and hidden files.
              if (!file.isSubDir() && !file.isHidden()) {
                // Save dirIndex of file in directory.
                dirIndex[n] = file.dirIndex();

                SerialConnect.write("@103:");
                file.printName(&SerialConnect);
                SerialConnect.println();
                delay(120);
              }
              file.close();
            }
            dirFile.close();
          } else deleteFile(filename);
          powerDownSD();
          break;
        }

        //      case 110: {
        //          if (s.indexOf("apn") > -1) {
        //            s.remove(0, s.indexOf(":") + 1);
        //            s.trim();
        //            apn = s;
        //            //Serial.print("APN:"); Serial.print(apn); Serial.println("::");
        //            EEPROMWrite(apn, APN);
        //          }
        //          else if (s.indexOf("TurnOn") > -1)
        //          {
        //            String resp = modemInit();
        //            if (resp == "OK") {
        //              SerialConnect.println("@110:net:" + getSPN());
        //              SerialConnect.println("@110:stren:" + getStrength());
        //
        //            } else {
        //              SerialConnect.println("@110:err:" + resp);
        //            }
        //
        //          }
        //          else if (s.indexOf("TurnOff") > -1)
        //          {
        //            turnOff_modem();
        //          }
        //          else if (s.indexOf("send") > -1)
        //          {
        //            if (sendTestData()) SerialConnect.println("@110:suc:Send Successfully");
        //            else  SerialConnect.println("@110:err:failed to send data");
        //          }
        //          else {
        //            SerialConnect.println("@110:apn:" + apn);
        //          }
        //          break;
        //        }

    }
    lt = millis();

  }
}
