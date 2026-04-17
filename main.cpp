#include <WiFi.h>
#include <HTTPClient.h>

#define ssid "TIM-83699574"                 // SSID
#define password "VA89zqccfKWwdeCtXd6N"     // Password

const char* apiUrl = "https://openapi.api.govee.com/router/api/v1/device/control";
const char* apiKey = "2e17fc45-fefc-4c85-a25e-da3afc2d56b5";

// Comando che invio al server Govee
String jsonPayload;

// Accendo lampada
void on (){
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
}

// Spengo lampada
void off (){
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
}

// Cambio colore
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

  WiFiClientSecure client;
  client.setInsecure();

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(client, apiUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Govee-API-Key", apiKey);

    // Azione che voglio fare 
    change_color();

    int httpResponseCode = http.POST(jsonPayload);

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    String response = http.getString();
    Serial.println("Risposta:");
    Serial.println(response);

    http.end();
  }
}

void loop() {

}
