#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN D5        // Pin where the DHT22 is connected
#define LEDPIN D4       
#define DHTTYPE DHT22    // DHT22 (AM2302) sensor

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);
  dht.begin();
  delay(2000); // 🛠 Give DHT22 extra time to initialize
  Serial.println("DHT22 Sensor Test");
}

void loop() {
  float temperature = dht.readTemperature();  // Read temperature (Celsius)
  float humidity = dht.readHumidity();        // Read humidity (%)

  printTempHumidity(temperature, humidity);
  turnLED(temperature, humidity);
  delay(2000);
}

void printTempHumidity(float temperature, float humidity) {
  if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
  } else {
      Serial.print("Temperature: ");
      Serial.print(temperature);
      Serial.print("°C  |  Humidity: ");
      Serial.print(humidity);
      Serial.println("%");
  }
}

void turnLED(float temperature, float humidity) {
  if (temperature >= 30.0 || humidity > 70.0) {
    digitalWrite(LEDPIN, LOW);
  } else {
    digitalWrite(LEDPIN, HIGH);
  }
}
