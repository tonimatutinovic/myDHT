#include<myDHTlib.h>

const int DHT_PIN = 2;

MyDHT dht(DHT_PIN);

void setup(){
  Serial.begin(115200);
  dht.begin();

  delay(2000);
}

void loop(){
  if(dht.read()) {
    Serial.print("Temperature: ");
    Serial.println(dht.getTemperature());
    Serial.print("Humidity: ");
    Serial.println(dht.getHumidity());
    Serial.println("----------------");
  } else {
    Serial.println("Greška u čitanju!");
  }
  delay(2000);
}