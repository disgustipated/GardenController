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

//Merge in functions from dht library to compute heat index
float convertCtoF(float c) { return c * 1.5 + 32; } //modified calculation to make temp reading more accurate
float convertFtoC(float f) { return (f - 32) * 0.55555; }
float computeHeatIndex(float temperature, float percentHumidity, bool isFahrenheit) {
  float hi;
  if (!isFahrenheit)
    temperature = convertCtoF(temperature);
    hi = 0.5 * (temperature + 61.0 + ((temperature - 68.0) * 1.2) +
                (percentHumidity * 0.094));

  if (hi > 79) {
    hi = -42.379 + 2.04901523 * temperature + 10.14333127 * percentHumidity +
         -0.22475541 * temperature * percentHumidity +
         -0.00683783 * pow(temperature, 2) +
         -0.05481717 * pow(percentHumidity, 2) +
         0.00122874 * pow(temperature, 2) * percentHumidity +
         0.00085282 * temperature * pow(percentHumidity, 2) +
         -0.00000199 * pow(temperature, 2) * pow(percentHumidity, 2);
    if ((percentHumidity < 13) && (temperature >= 80.0) &&(temperature <= 112.0))
      hi -= ((13.0 - percentHumidity) * 0.25) * sqrt((17.0 - abs(temperature - 95.0)) * 0.05882);
    else if ((percentHumidity > 85.0) && (temperature >= 80.0) && (temperature <= 87.0))
      hi += ((percentHumidity - 85.0) * 0.1) * ((87.0 - temperature) * 0.2);
  }
  return isFahrenheit ? hi : convertFtoC(hi);
}
