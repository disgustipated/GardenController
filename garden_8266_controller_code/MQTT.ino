//checks connection to mqtt and attempts connection if not connected
void checkMQTT() {
  currMillis = millis();
  if (currMillis - prevMillisMQTT >= CHECK_MQTT_INTERVAL)
  {
    Serial.println("Checking MQTT");
    prevMillisMQTT = currMillis;
    Serial.print("connected status: ");
    Serial.println(client.state());
    if (!client.connected()) {
      Serial.println("Not connected to MQTT, triggering reconnect");
      reconnect();
    }
    else
    {
      Serial.println("Client still connected");
    }
  }
}

void reconnect() {
  int reconnectCount = 0;
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.println("Attempting MQTT connection...");
    if (client.connect(DEVICENAME, mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      MQTTError(RED_DATA_PIN);
      int statecode = client.state();
      Serial.print("failed, rc=");
      Serial.print(statecode);
      blinkErrorCode(RED_DATA_PIN,statecode);
      Serial.println(" try again in 1 seconds");
      // Wait 1 second before retrying
      Alarm.delay(1000);
    }
  }
}

void publishMessage(String mqtttopic, JsonObject message, bool webResponse=false){
  checkMQTT();
  char buffer[512];
  size_t n = serializeJson(message, buffer);
  Serial.println(buffer);
  if(client.publish(toCharArray(mqtttopic), toCharArray(buffer), (size_t)n)){
    dataSend(WIFI_INFO_LED_PIN);
    if(webResponse){
      server.send(200,"text/plain",buffer);
    }
  } else {
    MQTTError(RED_DATA_PIN);
  }
}
