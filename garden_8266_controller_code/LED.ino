void MQTTError(){
  digitalWrite(RED_DATA_PIN, LOW);
  delay(2000);
  digitalWrite(RED_DATA_PIN, HIGH);
}

void blinkErrorCode(int errorNumber){
  int iblink = -1;
  while(iblink < errorNumber){
    digitalWrite(RED_DATA_PIN, LOW);
    delay(100);
    digitalWrite(RED_DATA_PIN, HIGH);
    delay(300);
    iblink++;
  }
}

void dataSend(){
  digitalWrite(WIFI_INFO_LED_PIN, LOW);
  delay(150);
  digitalWrite(WIFI_INFO_LED_PIN, HIGH);
}

void SensorError(){
  digitalWrite(RED_DATA_PIN, LOW);
  delay(50);
  digitalWrite(RED_DATA_PIN, HIGH);
  delay(50);
  digitalWrite(RED_DATA_PIN, LOW);
  delay(50);
  digitalWrite(RED_DATA_PIN, HIGH);
}
