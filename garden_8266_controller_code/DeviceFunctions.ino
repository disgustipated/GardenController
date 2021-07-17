void getTimeFromNtp() {
  // Begin NTP
  Serial.print("Begin NTP...");
  timeClient.begin();
  while (!timeClient.update()) yield();
  timeStruct.hours = timeClient.getHours();
  timeStruct.minutes = timeClient.getMinutes();
  timeStruct.seconds = timeClient.getSeconds();
  timeStruct.nextNtp = ntpInterval;
  Serial.println(" OK.");
}

char* toCharArray(String str) {
  return &str[0];
}
