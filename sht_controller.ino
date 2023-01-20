void sht_init(){
  pinMode(POWER_PIN, OUTPUT);
  //Connection Details taken from:
  //http://www.getmicros.net/raspberry-pi-and-sht31-sensor-example-in-c.php
  digitalWrite(POWER_PIN, LOW);
  delay(1000);
  digitalWrite(POWER_PIN, HIGH);
  delay(1000);

  if (sht.init()) {
        Serial.print("init(): success\n");
    } else {
        Serial.print("init(): failed\n");
    }
    sht.setAccuracy(SHTSensor::SHT_ACCURACY_MEDIUM); // only supported by SHT3x
  Serial.println("SHT Init");

}

void sht_read(){
  if (sht.readSample()) {
     data[0] = sht.getTemperature();
     data[1] = sht.getHumidity(); 
      
      #ifdef DEBUG
      Serial.print("SHT85\t");
      Serial.print("RH: ");
      Serial.print(sht.getHumidity(), 2);
      Serial.print("\t");
      Serial.print("T:  ");
      Serial.print(sht.getTemperature(), 2);
      Serial.print("\n");
      #endif
  } else {
      Serial.print("Error in readSample()\n");
  }
}