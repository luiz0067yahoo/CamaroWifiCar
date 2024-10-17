#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "FELIPE-PC";
const char* password = "00000000";
WebServer server(80);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("Conectado ao WiFi");

    server.on("/api/comando", HTTP_POST, []() {
        String body = server.arg("plain");
        Serial.println("Ação recebida: " + body);
        server.send(200, "application/json", "{\"status\":\"sucesso\"}");
    });

    server.begin();
}

void loop() {
    server.handleClient();
}