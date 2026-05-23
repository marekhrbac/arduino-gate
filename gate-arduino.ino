#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// ═══════════════════════════════
// CHANGE THOSE VALUES
// ═══════════════════════════════
const char* ssid      = "your_ssid";
const char* password  = "your_ssid_password";
const char* statusUrl = "https://gate-arduino.YOUR_NAME.workers.dev/?key=YOUR_SECRET";
const char* doneUrl   = "https://gate-arduino.YOUR_NAME.workers.dev/?key="YOUR_SECRET=done";
// ═══════════════════════════════

const int RELE_PIN = 4; // D2 = GPIO4

WiFiClientSecure client;

void setup() {
  // default relay state - not triggered
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, HIGH);

  Serial.begin(9600);
  delay(1000);

  // Connect to WIFI
  Serial.println("Connecting WIFI...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setInsecure(); // No SSL verification
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {

    // Read state from Worker
    HTTPClient http;
    http.begin(client, statusUrl);
    int code = http.GET();

    if (code == 200) {
      String status = http.getString();
      status.trim();
      Serial.println("Status: " + status);

      if (status == "open") {
        Serial.println(">>> Opening gate!");

        // Trigger relay for 1 second
        digitalWrite(RELE_PIN, LOW);
        delay(1000);
        digitalWrite(RELE_PIN, HIGH);

        // send action=done to worker
        HTTPClient http2;
        http2.begin(client, doneUrl);
        http2.GET();
        http2.end();

        Serial.println(">>> Done, state reset!");
      }
    } else {
      Serial.println("ERROR HTTP: " + String(code));
    }
    http.end();

  } else {
    // WiFi disconnected - try again
    Serial.println("WiFi disconnected, reconnect...");
    WiFi.begin(ssid, password);
    delay(5000);
  }

  delay(2000); // 2 second delay
}