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
      MQTTError();
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 1 seconds");
      // Wait 5 seconds before retrying
      Alarm.delay(1000);
    }
  }
}
