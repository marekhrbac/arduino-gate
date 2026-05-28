#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// ═══════════════════════════════
// UPRAV TYTO HODNOTY
// ═══════════════════════════════
const char* ssid      = "WIFI_SSID";
const char* password  = "WIFI_PASSWORD";
const char* statusUrl = "https://gate-arduino.YOUR_NAME.workers.dev/?key=YOUR_SECRET";
const char* doneUrl   = "https://gate-arduino.YOUR_NAME.workers.dev/?key=YOUR_SECRET&action=done";
// ═══════════════════════════════

const int RELE_PIN = 4; // D2 = GPIO4

WiFiClientSecure client;

// LED functions
void ledBlink() {
  digitalWrite(LED_BUILTIN, LOW);  // LED on
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH); // LED off
  delay(500);
}

void ledOn() {
  digitalWrite(LED_BUILTIN, LOW);  // LED on
}

void ledOff() {
  digitalWrite(LED_BUILTIN, HIGH); // LED off
}

void setup() {
  // default relay - not triggered
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(9600);
  delay(1000);

  // WIFI connection
  Serial.println("Connecting to WIFI...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    ledBlink();
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  ledOn();

  client.setInsecure(); // no SSL verification
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    // reading worker state
    HTTPClient http;
    http.begin(client, statusUrl);
    int code = http.GET();

    if (code == 200) {
      String status = http.getString();
      status.trim();
      Serial.println("Status: " + status);

      if (status == "open") {
        Serial.println(">>> Öpening gate!");

        // trigger relay for 1s
        digitalWrite(RELE_PIN, LOW);
        delay(1000);
        digitalWrite(RELE_PIN, HIGH);

        // send worker its done
        HTTPClient http2;
        http2.begin(client, doneUrl);
        http2.GET();
        http2.end();

        Serial.println(">>> Done, reseting state");
      }
    } else {
      Serial.println("Chyba HTTP: " + String(code));
    }
    http.end();

  } else {
    // WIFI disconnected - try again
    Serial.println("WiFi disconnected, reconnect...");
    ledBlink();
    WiFi.begin(ssid, password);
    delay(5000);
  }

  delay(2000); //API request every 2s
}