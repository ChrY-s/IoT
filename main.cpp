#include <WiFi.h>
#include <HTTPClient.h>

#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <FS.h>
#include <LittleFS.h>

#include <ArduinoJson.h>

#define ssid "TIM-83699574"                 // SSID
#define password "VA89zqccfKWwdeCtXd6N"     // Password

// state per lo stato
// control per controllarla
const char* apiUrl;
const char* apiKey = "2e17fc45-fefc-4c85-a25e-da3afc2d56b5";

WiFiClientSecure client;
HTTPClient http;
DynamicJsonDocument doc(4096);

// Server asincrono alla porta 80
AsyncWebServer server(80);

// Comando che invio al server Govee
String jsonPayload;
// Stato lampadina
String ledState;

// Accendo lampada
void on(void){
  // Url per la richiesta
  apiUrl = "https://openapi.api.govee.com/router/api/v1/device/control";

  // Corpo JSON
  jsonPayload = R"rawliteral(
    {
      "requestId": "uuid",
      "payload": {
        "sku": "H600D",
        "device": "6A:1F:10:20:BA:A2:70:3C",
        "capability": {
          "type": "devices.capabilities.on_off",
          "instance": "powerSwitch",
          "value": 1
        }
      }
    })rawliteral";

    ledState = "ON";
}

// Spengo lampada
void off(void){
  apiUrl = "https://openapi.api.govee.com/router/api/v1/device/control";

  jsonPayload = R"rawliteral(
    {
      "requestId": "uuid",
      "payload": {
        "sku": "H600D",
        "device": "6A:1F:10:20:BA:A2:70:3C",
        "capability": {
          "type": "devices.capabilities.on_off",
          "instance": "powerSwitch",
          "value": 0
        }
      }
    })rawliteral";

    ledState = "OFF";
}

// Prendo lo stato della lampada
void state(void) {
  apiUrl = "https://openapi.api.govee.com/router/api/v1/device/state";

  jsonPayload = R"rawliteral(
    {
    "requestId": "uuid",
    "payload": {
        "sku": "H600D",
        "device": "6A:1F:10:20:BA:A2:70:3C"
    }
  })rawliteral";
}

// Gestisco la richiesta HTTP

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE")
    return ledState;
  return String();
}

// Cambio colore
/*
void change_color (){
  String jsonPayload = R"rawliteral(
    {
      "requestId": "uuid",
      "payload": {
        "sku": "H600D",
        "device": "6A:1F:10:20:BA:A2:70:3C",
        "capability": {
          "type": "devices.capabilities.color_setting",
          "instance": "colorRgb",
          "value": 0
        }
      }
    })rawliteral";
}
*/

// Invio richieste HTTP
void ask(String cmd) {
  // Richieste HTTP
  if (WiFi.status() == WL_CONNECTED) {    
    // Azione che voglio fare 
    if (cmd == "ON") on();
    else if (cmd == "OFF") off();

    // Inizio richiesta HTTP
    // Imposto header
    http.begin(client, apiUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Govee-API-Key", apiKey);

    // Ricevo risposta
    int httpResponseCode = http.POST(jsonPayload);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.println("Risposta:");
    Serial.println(response);

    // Se chiedo lo stato stampo lo stato
    if (cmd == "STATE") {
      Serial.println("\nStato: ");
      Serial.println(ledState);
    }

    // Chuido connessione HTTP
    http.end();
  }
}

void setup() {
  Serial.begin(115200);

  // Connessione WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connessione al WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnesso!");

  client.setInsecure();


  // Initialize LittleFS
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/style.css", "text/css");
  });
  // Accendo lampadina
  server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
    ask("ON");
    request->send(LittleFS, "/index.html", String(), false, processor);
  });
  // Spengo lampadina
  server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
    ask("OFF");  
    request->send(LittleFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}

void loop() {

}
