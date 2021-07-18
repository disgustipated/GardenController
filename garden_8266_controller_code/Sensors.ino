void checkSensors(){
  currMillis = millis();
  StaticJsonDocument<512> mqttDoc;
  JsonObject sMsg = mqttDoc.to<JsonObject>();
  if (currMillis - prevMillisSensors >= CHECK_SENSORS_INTERVAL){
    prevMillisSensors = currMillis;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float f = dht.readTemperature(true);
    
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      SensorError();
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
    // Compute heat index in Fahrenheit and c
    float hif = dht.computeHeatIndex(f, h);
    float hic = dht.computeHeatIndex(t, h, false);
  
    if (f != 2147483647 || h != 2147483647 || hif != 2147483647){
        sMsg["temp"] = f;
        sMsg["humidity"] = h;
        sMsg["indx"] = hif;
        publishMessage("home/garden",sMsg,true);
      } else {
        sMsg["temp"] = 0;
        sMsg["humidity"] = 0;
        sMsg["indx"] = 0;
        SensorError();
        publishMessage("home/garden",sMsg,true);
      }
  }
}

void checkWaterLevels(){
  currMillis = millis();
  StaticJsonDocument<512> mqttDoc;
  JsonObject wlMsg = mqttDoc.to<JsonObject>();
  if (currMillis - wprevMillisSensors >= WATER_CHECK_SENSORS_INTERVAL){
    wprevMillisSensors = currMillis;
    
    publishMessage("home/garden/MainBarrel", wlMsg, true);
  }
}
