#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "Carrinho_WiFi";  // Nome do Hotspot
const char* password = "00000000";  // Senha do Hotspot

// Configurações do IP estático
IPAddress local_IP(192, 168, 1, 1);  // IP estático desejado para o hotspot
IPAddress gateway(192, 168, 1, 1);   // Gateway
IPAddress subnet(255, 255, 255, 0);  // Máscara de sub-rede

WebServer server(80);
const int PIN_FRENTE = 18;   // Frente
const int PIN_TRAS = 19;     // Trás
const int PIN_ESQUERDA = 22; // Esquerda
const int PIN_DIREITA = 23;  // Direita

// HTML da página de controle embutida
const char controlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Controle do Carrinho</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background-color: #f3f3f3; }
        .controle-container { display: inline-grid; grid-template-columns: 100px 100px 100px; gap: 10px; }
        .botao { font-size: 24px; padding: 20px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; }
        .botao-stop { background-color: #f44336; }
        .botao:active { background-color: #3e8e41; }
    </style>
</head>
<body>

<div class="controle-container">
    <div></div>
    <button class="botao" onclick="enviarComando('frente')">▲</button>
    <div></div>
    
    <button class="botao" onclick="enviarComando('esquerda')">◀</button>
    <button class="botao botao-stop" onclick="enviarComando('parar')">STOP</button>
    <button class="botao" onclick="enviarComando('direita')">▶</button>
    
    <div></div>
    <button class="botao" onclick="enviarComando('tras')">▼</button>
    <div></div>
</div>

<script>
    function enviarComando(acao) {
        fetch("/api/comando", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({ acao: acao })
        })
        .then(response => response.json())
        .then(data => {
            console.log("Comando enviado: " + acao);
        })
        .catch(error => console.error("Erro ao enviar comando: " + error));
    }
</script>

</body>
</html>
)rawliteral";

void printPinStates() {
    Serial.print("Estado do PIN_FRENTE: "); Serial.println(digitalRead(PIN_FRENTE) ? "LOW" : "HIGH");
    Serial.print("Estado do PIN_TRAS: "); Serial.println(digitalRead(PIN_TRAS) ? "LOW" : "HIGH");
    Serial.print("Estado do PIN_ESQUERDA: "); Serial.println(digitalRead(PIN_ESQUERDA) ? "LOW" : "HIGH");
    Serial.print("Estado do PIN_DIREITA: "); Serial.println(digitalRead(PIN_DIREITA) ? "LOW" : "HIGH");
}

void handleRoot() {
    server.send(200, "text/html", controlPage);  // Serve a página de controle
}

void handleComando() {
    String body = server.arg("plain");

    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
        server.send(400, "application/json", "{\"status\":\"erro\", \"mensagem\":\"JSON inválido\"}");
        return;
    }

    String acao = doc["acao"];

    // Ativa todos os pinos
    digitalWrite(PIN_FRENTE, HIGH);
    digitalWrite(PIN_TRAS, HIGH);
    digitalWrite(PIN_ESQUERDA, HIGH);
    digitalWrite(PIN_DIREITA, HIGH);
    Serial.println("Todos os pinos ativados:");
    printPinStates();

    if (acao == "frente") {
        digitalWrite(PIN_FRENTE, LOW);
        Serial.println("Comando: Movendo para frente");
        printPinStates();
        delay(1000);  
        digitalWrite(PIN_FRENTE, HIGH);  
        server.send(200, "application/json", "{\"status\":\"Movendo para frente\"}");
    } else if (acao == "tras") {
        digitalWrite(PIN_TRAS, LOW);
        Serial.println("Comando: Movendo para trás");
        printPinStates();
        delay(1000);  
        digitalWrite(PIN_TRAS, HIGH);  
        server.send(200, "application/json", "{\"status\":\"Movendo para trás\"}");
    } else if (acao == "esquerda") {
        digitalWrite(PIN_ESQUERDA, LOW);
        Serial.println("Comando: Virando à esquerda");
        printPinStates();
        delay(500);  
        digitalWrite(PIN_ESQUERDA, HIGH);  
        server.send(200, "application/json", "{\"status\":\"Virando à esquerda\"}");
    } else if (acao == "direita") {
        digitalWrite(PIN_DIREITA, LOW);
        Serial.println("Comando: Virando à direita");
        printPinStates();
        delay(500);  
        digitalWrite(PIN_DIREITA, HIGH);  
        server.send(200, "application/json", "{\"status\":\"Virando à direita\"}");
    } else if (acao == "parar") {
        Serial.println("Comando: Parado");
        printPinStates();
        server.send(200, "application/json", "{\"status\":\"Parado\"}");
    } else {
        server.send(400, "application/json", "{\"status\":\"erro\", \"mensagem\":\"Comando desconhecido\"}");
    }

    // Exibe o estado final dos pinos após o comando
    Serial.println("Estado dos pinos após o comando:");
    printPinStates();
}

void setup() {
    Serial.begin(115200);

    // Inicializa o modo de ponto de acesso
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_IP, gateway, subnet);

    Serial.println("Hotspot criado!");
    Serial.print("IP do Hotspot: ");
    Serial.println(WiFi.softAPIP());

    // Configura os pinos de controle como saída
    pinMode(PIN_FRENTE, OUTPUT);
    pinMode(PIN_TRAS, OUTPUT);
    pinMode(PIN_ESQUERDA, OUTPUT);
    pinMode(PIN_DIREITA, OUTPUT);

    server.on("/", HTTP_GET, handleRoot);  
    server.on("/api/comando", HTTP_POST, handleComando);  

    server.begin();
}

void loop() {
    server.handleClient();
}
