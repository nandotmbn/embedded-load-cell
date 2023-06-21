#include "HX711.h"
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <HTTPClient.h>

#include "SPI.h"
#include <RTCLib.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "SPIFFS.h"
#include <ESPAsyncWebServer.h>
#include "Preferences.h"

#define RST_PIO 16
#define RST_PIO 4

String IP_ADDRESS = "192.168.158.87";

// Your Domain name with URL path or IP address with path
String serverName = "http://" + IP_ADDRESS + ":8081/api/v1/record/apiKey/9BwjCEw5";

// Replace with your network credentials
const char *ssidAP = "ESP32AP";
const char *passwordAP = "katakuri";
char ssid[50], password[50];

// Set web server port number to 80
AsyncWebServer server(80);
Preferences preferences;

LiquidCrystal_I2C lcd(PCF8574_ADDR_A21_A11_A01, 4, 5, 6, 16, 11, 12, 13, 14, POSITIVE);
const int HX711_dout = 27;
const int HX711_sck = 26;

HX711 scale;

void ServerAPMode() //
{
  // Connect to Wi-Fi network with SSID and password
  Serial.println("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssidAP, passwordAP);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP); // 192.168.4.1

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/accesspoint.html"); });
  server.on("/accesspoint.css", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/accesspoint.css"); });
  server.on("/accesspoint.js", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/accesspoint.js"); });
  server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              // WiFi.disconnect();
              int ssidIndex = 0;
              int passwordIndex = 1;
              String ssidGet =  request->arg(ssidIndex).c_str();
              String passwordGet = request->arg(passwordIndex).c_str();

              ssidGet.toCharArray(ssid, 50);
              passwordGet.toCharArray(password, 50);

              Serial.println(ssid);
              Serial.println(password);

              WiFi.begin(ssid, password);
              while (WiFi.status() != WL_CONNECTED)
              {
                delay(500);
                Serial.print(".");
              }

              if(WiFi.status() != 3) {
                return request->send(200, "text/plain", "ssid or password is not valid");
              }
              // Print local IP address and start web server
              Serial.println("");
              Serial.println("WiFi connected.");
              Serial.println("IP address: ");
              String _ip = WiFi.localIP().toString();
              Serial.println(_ip);

              if(_ip != "0.0.0.0") {
                Serial.println("tersimpan");
                preferences.putString("ssid", ssidGet);
                preferences.putString("password", passwordGet);
                WiFi.disconnect();
              }

              request->send(200, "text/plain", _ip); });
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  server.begin();
}

void setup()
{
  Serial.begin(115200);
  preferences.begin("credential", false); // Initiate EEPROM Object Storage named "credential"

  // You can change ssid and password as you wish
  // This ssid and password works when ESP as Client
  String ssidPref = preferences.getString("ssid", "zmpak");
  String passwordPref = preferences.getString("password", "katakuri");

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
  };

  ssidPref.toCharArray(ssid, 50);
  passwordPref.toCharArray(password, 50);

  Serial.println(ssid);
  Serial.println(password);

  Serial.print("Connecting to: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startTime) <= 10000)
  {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    ServerAPMode();
    while (1)
      ;
  }

  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // lcd.begin();
  pinMode(HX711_dout, INPUT);
  lcd.begin(2, 16);
  Serial.println("Memulai...");
  lcd.setCursor(0, 0);
  lcd.print("Monitoring Berat");
  lcd.setCursor(2, 1);
  lcd.print("By : Septian");
  delay(2500);
  lcd.clear();

  scale.begin(HX711_dout, HX711_sck);
  scale.set_scale(21.8852);
  // scale.set_scale(-471.497);                      // this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare(); // reset the scale to 0
}

void loop()
{
  Serial.println("OnLoop");
  double oneReading = scale.get_units(3);
  // double avg20Reading = scale.get_units(20);
  lcd.clear();
  if (oneReading < 1 && oneReading > -20)
  {
    oneReading = oneReading * (0);
    lcd.print("Berat = " + String(oneReading / 1000));
    lcd.setCursor(14, 0);
    lcd.print("Kg");
  }
  else
  {
    lcd.print("Berat = " + String(oneReading / 1000));
    lcd.setCursor(14, 0);
    lcd.print("Kg");
  }

  if (digitalRead(4) == LOW)
  {
    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("Calibrate...");
    scale.set_scale(21.8852);
    scale.tare();
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Calibrate Done");
    // lcd.setCursor(0, 1);
    // lcd.print("Done");
    delay(800);
    // lcd.clear();
  }

  if (digitalRead(16) == LOW)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;

      String serverPath = serverName;

      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());

      // If you need Node-RED/server authentication, insert user and password below
      // http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      http.addHeader("Content-Type", "application/json");

      String dataBuilder = String("{\"weight\":") + String(oneReading) + String("}");

      Serial.print(dataBuilder);

      // Send HTTP GET request
      int httpResponseCode = http.POST(dataBuilder);

      if (httpResponseCode > 0)
      {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else
      {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else
    {
      Serial.println("WiFi Disconnected");
    }
    // lcd.clear();
  }

  scale.power_down();
  delay(500);
  scale.power_up();
}