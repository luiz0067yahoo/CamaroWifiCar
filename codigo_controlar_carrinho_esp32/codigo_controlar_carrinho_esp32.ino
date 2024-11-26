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
#define PIN_MOTOR_ESQUERDO_FRENTE 18
#define PIN_MOTOR_ESQUERDO_tras 19
#define PIN_MOTOR_DIREITO_FRENTE 22
#define PIN_MOTOR_DIREITO_tras 23

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
    let estadoEsquerda = false;
    let estadoDireita = false;
    let estadoFrente  = false;
    let estadoTras  = false;
    let estadoParar  = true;

    function atualizarAcao() {
        let acao = "";
        if(!estadoDireita && !estadoEsquerda && estadoFrente && !estadoTras){
            acao = "frente";
        }
        else if(!estadoDireita && !estadoEsquerda && !estadoFrente && estadoTras){
            acao = "tras";
        }
        else if (estadoEsquerda && !estadoDireita && estadoFrente && !estadoTras) {
            acao = "esquerda_frente";
        } 
        else if (estadoDireita && !estadoEsquerda && estadoFrente && !estadoTras) {
            acao = "direita_frente";
        } 
        else if (estadoEsquerda && !estadoDireita && !estadoFrente && estadoTras) {
            acao = "esquerda_tras";
        } 
        else if (estadoDireita && !estadoEsquerda && !estadoFrente && estadoTras) {
            acao = "direita_tras";
        } 
        else if(!estadoParar){
            acao = "parar";
        }
        enviarComando(acao);
    }

    document.getElementById('esquerda').ontouchstart = () => {
        estadoEsquerda = true;
        atualizarAcao();
    };
    document.getElementById('esquerda').ontouchend = () => {
        estadoEsquerda = false;
        atualizarAcao();
    };

    document.getElementById('direita').ontouchstart = () => {
        estadoDireita = true;
        atualizarAcao();
    };
    document.getElementById('direita').ontouchend = () => {
        estadoDireita = false;
        atualizarAcao();
    };


    document.getElementById('tras').ontouchstart = () => {
        estadoTras = true;
        atualizarAcao();
    };
    document.getElementById('tras').ontouchend = () => {
        estadoTras = false;
        atualizarAcao();
    };


    document.getElementById('frente').ontouchstart = () => {
        estadoFrente = true;
        atualizarAcao();
    };
    document.getElementById('frente').ontouchend = () => {
        estadoFrente = false;
        atualizarAcao();
    };

    document.getElementById('parar').ontouchstart = () => {
        estadoFrente = false;
        atualizarAcao();
    };
    document.getElementById('parar').ontouchend = () => {
        estadoParar = true;
        atualizarAcao();
    };

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
    digitalWrite(PIN_MOTOR_ESQUERDO_tras, HIGH);
    digitalWrite(PIN_MOTOR_DIREITO_FRENTE, HIGH);
    digitalWrite(PIN_MOTOR_DIREITO_tras, HIGH);

    if (acao == "frente") {
        digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, LOW);
        digitalWrite(PIN_MOTOR_DIREITO_FRENTE, LOW);
        digitalWrite(PIN_MOTOR_DIREITO_tras, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO_tras, HIGH);
    } else if (acao == "tras") {
        digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO_tras, LOW);
        digitalWrite(PIN_MOTOR_DIREITO_tras, LOW);
    } else if (acao == "esquerda_frente") {
        digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, LOW);
        digitalWrite(PIN_MOTOR_DIREITO_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO_tras, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO_tras, LOW);
    } else if (acao == "direita_frente") {
        digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO_FRENTE, LOW);
        digitalWrite(PIN_MOTOR_ESQUERDO_tras, LOW);
        digitalWrite(PIN_MOTOR_DIREITO_tras, HIGH);
    } else if (acao == "esquerda_tras") {
        digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO_FRENTE, LOW);
        digitalWrite(PIN_MOTOR_ESQUERDO_tras, LOW);
        digitalWrite(PIN_MOTOR_DIREITO_tras, HIGH);
    } else if (acao == "direita_tras") {
        digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, LOW);
        digitalWrite(PIN_MOTOR_DIREITO_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO_tras, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO_tras, LOW);
    } else if (acao == "parar") {
      digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, HIGH);
      digitalWrite(PIN_MOTOR_ESQUERDO_tras, HIGH);
      digitalWrite(PIN_MOTOR_DIREITO_FRENTE, HIGH);
      digitalWrite(PIN_MOTOR_DIREITO_tras, HIGH);
    }

    server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_IP, gateway, subnet);

    pinMode(PIN_MOTOR_ESQUERDO_FRENTE, OUTPUT);
    pinMode(PIN_MOTOR_ESQUERDO_tras, OUTPUT);
    pinMode(PIN_MOTOR_DIREITO_FRENTE, OUTPUT);
    pinMode(PIN_MOTOR_DIREITO_tras, OUTPUT);

    digitalWrite(PIN_MOTOR_ESQUERDO_FRENTE, HIGH);
    digitalWrite(PIN_MOTOR_ESQUERDO_tras, HIGH);
    digitalWrite(PIN_MOTOR_DIREITO_FRENTE, HIGH);
    digitalWrite(PIN_MOTOR_DIREITO_tras, HIGH);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/comando", HTTP_POST, handleComando);

    server.begin();
}

void loop() {
    server.handleClient();
}
