#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <ESP8266WiFi.h>
#include "config.h"

#define DHTPIN D5        // Pin where the DHT22 is connected
#define LEDPIN D4       
#define DHTTYPE DHT22    // DHT22 (AM2302) sensor

DHT dht(DHTPIN, DHTTYPE);
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  connectToWiFi();
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
  delay(5000);

  WiFiClient client = server.available();
  if (client) {
    Serial.println("\n[Client connected]");
    
    // read line by line what the client (web browser) is requesting
    while (client.connected()) {
      String line = client.readStringUntil('\r');
      Serial.print(line);
      // wait for end of client's request, that is marked with an empty line
      if (line.length() == 1 && line[0] == '\n') {
        client.println(prepareHtmlPage(temperature, humidity));
        break;
      }
    }
  }
  while (client.available()) {
      // but first, let client finish its request
      // that's diplomatic compliance to protocols
      // (and otherwise some clients may complain, like curl)
      // (that is an example, prefer using a proper webserver library)
      client.read();
    }

    // close the connection:
    client.stop();
    Serial.println("[Client disconnected]");
}

void connectToWiFi() {
  Serial.println();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print(" Connected");
  server.begin();
  Serial.printf("Web server started, open %s in a web browser\n", 
                WiFi.localIP().toString().c_str());
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

String prepareHtmlPage(float temperature, float humidity) {
  String htmlPage;
  htmlPage.reserve(1024);               // prevent ram fragmentation
  htmlPage = F("HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "Connection: close\r\n"  // the connection will be closed after completion of the response
               "Refresh: 5\r\n"         // refresh the page automatically every 5 sec
               "\r\n"
               "<!DOCTYPE HTML>"
               "<html>");
  htmlPage += "Temperature: " + String(temperature, 2) + "&deg;C <br/>";
  htmlPage += "Humidity: " + String(humidity, 2) + "%";
  htmlPage += F("</html>"
                "\r\n");
  return htmlPage;
}
