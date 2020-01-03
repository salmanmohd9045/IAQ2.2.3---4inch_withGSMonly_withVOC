

void co2Calibration() {

  long int lt3 = millis() - 1000;
  bool calib = false;
  int secCounter = 20 * 60;
  while (1) {
    if (!calib) {
      if (millis() - lt3 > 1000) {
        lt3 = millis();
    
       
        co2Serial.write(ask, 9);
        secCounter--;
        if (secCounter == -1) {
          calib = true;
          
        }
      }
    } else {
      if (!digitalRead(ENTER)) {

        co2Serial.write(zero_calib, 9);
        break;
      }
    }

    if (!digitalRead(EXIT)) {
      break;
    }

    getCO2();

  }
}
