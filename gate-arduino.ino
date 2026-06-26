#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

// ═══════════════════════════════
// SSID & Worker
// ═══════════════════════════════
const char* ssid      = "your_ssid";
const char* password  = "ssid_password";
const char* statusUrl = "https://gate-arduino.your_name.workers.dev/?key=your_secret";
const char* doneUrl   = "https://gate-arduino.your_name.workers.dev/?key=your_secret&action=done";
// ═══════════════════════════════

const int RELE_PIN = 4; 
const long AUTO_CLOSE_DELAY = 70000; 
bool gateIsOpen = false;
unsigned long gateOpenedAt = 0;

WiFiClientSecure client;

void ledBlink() {
  digitalWrite(LED_BUILTIN, LOW);  
  delay(500);
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(500);
}

void ledOn() {
  digitalWrite(LED_BUILTIN, LOW);  
}

void ledOff() {
  digitalWrite(LED_BUILTIN, HIGH); 
}

void setup() {
  pinMode(RELE_PIN, OUTPUT);
  digitalWrite(RELE_PIN, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(9600);
  delay(1000);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    ledBlink();
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi pripojeno!");
  Serial.print("IP adresa: ");
  Serial.println(WiFi.localIP());
  ledOn();

  client.setInsecure();
}

void triggerRelay() {
  digitalWrite(RELE_PIN, LOW);
  delay(500);
  digitalWrite(RELE_PIN, HIGH);
}

void loop() {

   if (gateIsOpen && (millis() - gateOpenedAt > AUTO_CLOSE_DELAY)) {
    Serial.println(">>> Auto-closing gate after 60s");
    triggerRelay();
    gateIsOpen = false;
  }
  
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;
    http.begin(client, statusUrl);
    int code = http.GET();

    if (code == 200) {
      String status = http.getString();
      status.trim();
      Serial.println("Status: " + status);

    if (status == "open" && !gateIsOpen) {  
      Serial.println(">>> Otviram branu!");
      triggerRelay();                        
      gateIsOpen = true;                    
      gateOpenedAt = millis(); 

      HTTPClient http2;
      http2.begin(client, doneUrl);
      http2.GET();
      http2.end();

        Serial.println(">>> Done, state reset");
      }
    } else {
      Serial.println("Chyba HTTP: " + String(code));
    }
    http.end();

  } else {
    Serial.println("WiFi disconnected, reconnect...");
    ledBlink();
    WiFi.begin(ssid, password);
    delay(5000);
  }

  delay(2000); 
}
