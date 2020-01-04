#define countof(a) (sizeof(a) / sizeof(a[0]))

String getTime() {
  RtcDateTime now = Rtc.GetDateTime();
  char datestring[6];
  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u:%02u"),
             now.Hour(),
             now.Minute() );
  return datestring;
}

String getDate()
{
  RtcDateTime dt = Rtc.GetDateTime();
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%02u"),
             dt.Day(),
             dt.Month(),
             dt.Year() - 2000
            );
  return datestring;
}

int getMinute() {
  RtcDateTime dt = Rtc.GetDateTime();
  return dt.Minute();
}




String getDateTimeNow()
{
  RtcDateTime dt = Rtc.GetDateTime();
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%04u-%02u-%02u %02u:%02u:%02u"),
             dt.Year(),
             dt.Month(),
             dt.Day(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  return datestring;
}

String getDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u,%02u:%02u:%02u"),
             dt.Day(),
             dt.Month(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  return datestring;
}


String getTimeNow()
{
  RtcDateTime dt = Rtc.GetDateTime();
  char datestring[9];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u:%02u:%02u"),
             dt.Hour(),
             dt.Minute(),
             dt.Second());
  return datestring;
}


String getTimeStamp()
{
  RtcDateTime dt = Rtc.GetDateTime();
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%02u %02u:%02u"),
             dt.Day(),
             dt.Month(),
             dt.Year() - 2000,
             dt.Hour(),
             dt.Minute());
  return datestring;
}



void updateTime(String st) {
  int serTimeArray[6];
  //hh:mm:DD:MM:YY:
  for (int i = 0; i < 5; i++) {
    String s = st.substring(0, st.indexOf(":"));
    st.remove(0, st.indexOf(":")+1);
    serTimeArray[i] = s.toInt();
  }
  RtcDateTime now = Rtc.GetDateTime();
  //yy,MM,DD,hh,mm,ss
  RtcDateTime sevTime = RtcDateTime(serTimeArray[4] + 2000, serTimeArray[3], serTimeArray[2], serTimeArray[0], serTimeArray[1], 0);
  Serial.println(sevTime);
  Serial.println(now);
  Serial.println((long int)now - (long int)sevTime);
  if (abs((long int)now - (long int)sevTime) > 20) {
    Serial.println("updating time...");
    Rtc.SetDateTime(sevTime);
  }
}
