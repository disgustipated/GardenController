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
  Serial.println("Attempting MQTT connection...");
  if (client.connect(DEVICENAME, mqtt_user, mqtt_password)) {
    Serial.println("connected");
  } else {
    MQTTError();
    Serial.print("failed, rc=");
    Serial.print(client.state());
    blinkErrorCode(client.state());
  }

}

void publishMessage(String mqtttopic, JsonObject message, bool webResponse=false){
  checkMQTT();
  char buffer[512];
  size_t n = serializeJson(message, buffer);
  Serial.println(buffer);
  if(client.publish(toCharArray(mqtttopic), toCharArray(buffer), (size_t)n)){
    dataSend();
    if(webResponse){
      server.sendHeader("Location","/");
      server.send(303,"text/plain",(String)buffer + " sent to mqtt topic " + mqtttopic);
    }
  } else {
    MQTTError();
  }
}
