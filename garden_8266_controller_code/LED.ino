void MQTTError(){
  digitalWrite(RED_DATA_PIN, LOW);
  delay(2000);
  digitalWrite(RED_DATA_PIN, HIGH);
}

void SensorSend(){
  digitalWrite(RED_DATA_PIN, LOW);
  delay(150);
  digitalWrite(RED_DATA_PIN, HIGH);
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
