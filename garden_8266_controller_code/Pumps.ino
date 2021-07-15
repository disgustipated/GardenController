void pumpRunning()
{
  //should the pump be running
  currMillis = millis();
  if ((digitalRead(PUMP_ACTIVATE_PIN) == LOW) && (currMillis > (deviceActivateStart + ACTIVATE_DURATION))){
    stopPump();
    }
}

void activatePump()
{
  Alarm.delay(500);
  deviceActivateStart = millis();
  server.send(200,"text/plain", "OK"); 
  Serial.print("Activating deviceActivateStart = ");
  Serial.println(deviceActivateStart);   
  Serial.println(deviceActivateStart + ACTIVATE_DURATION);
  digitalWrite(PUMP_ACTIVATE_PIN, LOW); //reversed these to prevent relay from going on during a reboot
  digitalWrite(WIFI_INFO_LED_PIN,LOW);
}

void stopPump()
{
  Serial.println("Stopping pump");
  digitalWrite(PUMP_ACTIVATE_PIN, HIGH);
  digitalWrite(WIFI_INFO_LED_PIN,HIGH);
}
