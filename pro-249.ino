#include "DHT.h"
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


#define WLAN_SSID       "pkr1"
#define WLAN_PASS       "pkr123456"

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME  "suranusha"
#define AIO_KEY       "aio_hIzm72NhBRyS6E33GvxhQpwz0LJ2"

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

Adafruit_MQTT_Publish temperature = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/temperature");
Adafruit_MQTT_Subscribe sw1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/sw1");


#define DHTPIN 15

#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);
void MQTT_connect();

float p;
const int led1 = 18;
String stringOne;

void setup() {
  Serial.begin(9600);
  pinMode(led1, OUTPUT);
  digitalWrite(led1, LOW);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());
  Serial.println("DHT11 sensor!");
  //call begin to start sensor
  dht.begin();
   mqtt.subscribe(&sw1);
}

void loop() {
    p = dht.readTemperature();
  
  // print the result to Terminal
  if (! temperature.publish(p)) {
    Serial.println(F("Temp Failed"));
  } else {
    Serial.println(F("Temp OK!"));
  }
  MQTT_connect();
  delay(1000);
  
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &sw1) {
      //Serial.print(F("Got: "));
      //Serial.println((char *)sw1.lastread);
      //digitalWrite(led1, HIGH);

      stringOne = (char *)sw1.lastread;
      Serial.print(F("stringOne: "));
      Serial.println(stringOne);

      if (stringOne == "ON") {
        digitalWrite(led1, HIGH);
      }
      if (stringOne == "OFF") {
        digitalWrite(led1, LOW);
      }
    }
  }
 
}
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  //Serial.print("Connecting to MQTT... ");
uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    //Serial.println(mqtt.connectErrorString(ret));
    //Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  

}