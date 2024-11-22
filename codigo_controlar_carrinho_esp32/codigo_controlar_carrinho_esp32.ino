#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// Definições de WiFi
const char* ssid = "TI_DO_CEU";
const char* password = "00000000";

// Configuração de IP estático
IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// Configuração do servidor
WebServer server(80);

// Definição dos pinos
#define PIN_MOTOR_ESQUERDO_FRENTE 19
#define PIN_MOTOR_ESQUERDO_TRAZ 18
#define PIN_MOTOR_DIREITO_FRENTE 22
#define PIN_MOTOR_DIREITO_TRAZ 23

// Página HTML para controle
const char controlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Controle do Carrinho</title>
    <style>
        * { user-select: none; }
        body { font-family: Arial, sans-serif; text-align: center; background-color: #f3f3f3; }
        .controle-container { display: inline-grid; grid-template-columns: 200px 200px 200px; gap: 10px; }
        .botao { font-size: 24px; width: 200px; height: 200px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; }
        .botao-stop { background-color: #f44336; }
        .botao:active { background-color: black; }
    </style>
</head>
<body>
<div class="controle-container">
    <div></div>
    <button class="botao" id="frente">▲</button>
    <div></div>
    
    <button class="botao" id="esquerda">◀</button>
    <button class="botao botao-stop" id="parar">STOP</button>
    <button class="botao" id="direita">▶</button>
    
    <div></div>
    <button class="botao" id="tras">▼</button>
    <div></div>
</div>

<script>
    let intervalos = {};

    function iniciarIntervalo(acao) {
        if (!intervalos[acao]) {
            enviarComando(acao);
            intervalos[acao] = setInterval(() => enviarComando(acao), 200);
        }
    }

    function pararIntervalo(acao) {
        if (intervalos[acao]) {
            clearInterval(intervalos[acao]);
            enviarComando(acao + "_parar");
            delete intervalos[acao];
        }
    }

    document.getElementById('frente').ontouchstart = document.getElementById('frente').onmousedown = () => iniciarIntervalo('frente');
    document.getElementById('frente').ontouchend = document.getElementById('frente').onmouseup = document.getElementById('frente').onmouseleave = () => pararIntervalo('frente');

    document.getElementById('tras').ontouchstart = document.getElementById('tras').onmousedown = () => iniciarIntervalo('tras');
    document.getElementById('tras').ontouchend = document.getElementById('tras').onmouseup = document.getElementById('tras').onmouseleave = () => pararIntervalo('tras');

    document.getElementById('esquerda').ontouchstart = document.getElementById('esquerda').onmousedown = () => iniciarIntervalo('esquerda');
    document.getElementById('esquerda').ontouchend = document.getElementById('esquerda').onmouseup = document.getElementById('esquerda').onmouseleave = () => pararIntervalo('esquerda');

    document.getElementById('direita').ontouchstart = document.getElementById('direita').onmousedown = () => iniciarIntervalo('direita');
    document.getElementById('direita').ontouchend = document.getElementById('direita').onmouseup = document.getElementById('direita').onmouseleave = () => pararIntervalo('direita');

    document.getElementById('parar').onclick = () => enviarComando('parar');

    function enviarComando(acao) {
        fetch("/api/comando", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify({ acao: acao })
        })
        .then(response => response.text())
        .then(data => console.log("Comando enviado: " + acao))
        .catch(error => console.error("Erro ao enviar comando: " + error));
    }
</script>
</body>
</html>
)rawliteral";

void handleRoot() {
    server.send(200, "text/html", controlPage);
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
    digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, HIGH);
    digitalWrite(PIN_MOTOR_ESQUERDO_TRAZ, HIGH);
    digitalWrite(PIN_MOTOR_DIREITO_FRENTE, HIGH);
    digitalWrite(PIN_MOTOR_DIREITO_TRAZ, HIGH);

    if (acao == "frente") {
        digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, LOW);
        digitalWrite(PIN_MOTOR_DIREITO_FRENTE, LOW);
    } else if (acao == "tras") {
        digitalWrite(PIN_MOTOR_ESQUERDO_TRAZ, LOW);
        digitalWrite(PIN_MOTOR_DIREITO_TRAZ, LOW);
    } else if (acao == "esquerda_frente") {
        digitalWrite(PIN_MOTOR_DIREITO_FRENTE, LOW);
    } else if (acao == "direita_frente") {
        digitalWrite(PIN_MOTOR_DIREITO_FRENTE, LOW);
    } else if (acao == "esquerda_tras") {
        digitalWrite(PIN_MOTOR_ESQUERDO_TRAZ, LOW);
    } else if (acao == "direita_tras") {
        digitalWrite(PIN_MOTOR_DIREITO_TRAZ, LOW);
    }

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_IP, gateway, subnet);

    pinMode(PIN_MOTOR_ESQUERDO_FRENTE, OUTPUT);
    pinMode(PIN_MOTOR_ESQUERDO_TRAZ, OUTPUT);
    pinMode(PIN_MOTOR_DIREITO_FRENTE, OUTPUT);
    pinMode(PIN_MOTOR_DIREITO_TRAZ, OUTPUT);

    digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, HIGH);
    digitalWrite(PIN_MOTOR_ESQUERDO_TRAZ, HIGH);
    digitalWrite(PIN_MOTOR_DIREITO_FRENTE, HIGH);
    digitalWrite(PIN_MOTOR_DIREITO_TRAZ, HIGH);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/comando", HTTP_POST, handleComando);

    server.begin();
}

void loop() {
    server.handleClient();
}
