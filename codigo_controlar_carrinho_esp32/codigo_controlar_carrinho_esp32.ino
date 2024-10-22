#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>  // Biblioteca para parsear JSON

const char* ssid = "FELIPE-PC";
const char* password = "00000000";
WebServer server(80);

const int D2 = 4;  // D2 corresponde ao GPIO 4 no ESP8266 e ESP32

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);

    pinMode(D2, OUTPUT);  // Define D2 como saída

    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Conectando ao WiFi...");
    }
    Serial.println("Conectado ao WiFi");

    server.on("/api/comando", HTTP_POST, []() {
        String body = server.arg("plain");  // Obtém o corpo da requisição
        Serial.println("Ação recebida: " + body);

        // Cria um objeto JSON para parsear o corpo da requisição
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, body);

        // Verifica se o JSON foi parseado corretamente
        if (error) {
            Serial.print("Erro ao parsear JSON: ");
            Serial.println(error.c_str());
            server.send(400, "application/json", "{\"status\":\"erro\", \"mensagem\":\"JSON inválido\"}");
            return;
        }

        // Extrai o valor da chave "acao" do JSON
        String acao = doc["acao"];

        // Verifica o valor da ação e controla o pino D2
        if (acao == "frente") {
            digitalWrite(D2, HIGH);  // Ativa o pino D2
            Serial.println("Movendo para frente.");
        } else {
            digitalWrite(D2, LOW);   // Desativa o pino D2 para outros comandos
            Serial.println("Comando desconhecido ou parada.");
        }

        // Envia a resposta de sucesso
        server.send(200, "application/json", "{\"status\":\"sucesso\"}");
    });

    server.begin();
}

void loop() {
    server.handleClient();
}
