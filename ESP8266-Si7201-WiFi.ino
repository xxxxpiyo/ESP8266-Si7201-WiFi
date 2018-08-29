#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Adafruit_Si7021.h>
#include <ESP8266HTTPClient.h>

// Config
#define DEVICE_ID 2
#define POST_KEY "piyopiyo"

// Deep Sleep
#define TIME_TO_SLEEP  300        /* Time ESP32 will go to sleep (in seconds) */

// Si7021
Adafruit_Si7021 sensor = Adafruit_Si7021();

// WiFi
#define SSID "AIR-APP001"
#define WIFI_PASS "770815&771231"
ESP8266WiFiMulti wifi;

// HTTP Client
#define HTTP_URL "http://10.0.1.11/weather/input"

uint i;

void setup()
{
    Serial.println("aaa");
    Serial.begin(115200);
    Serial.println("Wake up!!");

    // Wifi
    wifi.addAP(SSID, WIFI_PASS);
    Serial.print("Wait for WiFi... ");


    i = 0;
    while(wifi.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
        i++;
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Startup Si7021
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    while (true);
  }
}



void loop()
{
    char json[65];
    float hum, temp;
    hum = sensor.readHumidity();
    temp = sensor.readTemperature();

    sprintf(json, "{ \"temperature\":%2.1f, \"pressure\":%4.1f, \"humidity\":%3.1f }", temp, 0.0, hum);
    Serial.println(json);

    Serial.println("Done.");

    // HTTP
    Serial.println("Start HTTP Client");
    HTTPClient http;
    char params[128];
    http.begin(HTTP_URL);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    sprintf(params, "key=%s&id=%d&temperature=%2.1f&pressure=%4.1f&humidity=%3.1f", POST_KEY,DEVICE_ID,temp,0.0,hum);
    int httpCode = http.POST(params);
    if (httpCode > 0) {
      // HTTP レスポンスコードが返ってくる
      Serial.printf("[HTTPS] POST... code: %d\n", httpCode);
    } else {
        Serial.println("[HTTPS] no connection or no HTTP server.");
    }
    delay(500);
    http.end();
    delay(500);

    //DEEP SLEEPモード突入命令
    Serial.println("DEEP SLEEP START!!");
    ESP.deepSleep(TIME_TO_SLEEP * 1000 * 1000 , WAKE_RF_DEFAULT);
}
