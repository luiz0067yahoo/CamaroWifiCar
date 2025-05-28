#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>


// Definições de WiFi
const char* ssid = "SENAC_WIFI_CAR";
const char* password = "00000000";


// Configuração de IP estático
IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);


// Configuração do servidor
WebServer server(80);


// Definição dos pinos
#define PIN_MOTOR_FRENTE 18
#define PIN_MOTOR_TRAZ 19
#define PIN_MOTOR_DIREITO 22
#define PIN_MOTOR_ESQUERDO 23


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
        else if (estadoEsquerda && !estadoDireita && !estadoFrente && !estadoTras) {
            acao = "esquerda";
        }
        else if (estadoDireita && !estadoEsquerda && !estadoFrente && !estadoTras) {
            acao = "direita";
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
        digitalWrite(PIN_MOTOR_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_TRAZ, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO, HIGH);


    if (acao == "frente") {
        digitalWrite(PIN_MOTOR_FRENTE, LOW);
        digitalWrite(PIN_MOTOR_TRAZ, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO, HIGH);
    } else if (acao == "tras") {
        digitalWrite(PIN_MOTOR_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_TRAZ, LOW);
        digitalWrite(PIN_MOTOR_DIREITO, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO, HIGH);
    } else if (acao == "esquerda") {
        digitalWrite(PIN_MOTOR_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_TRAZ, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO, LOW);
    } else if (acao == "direita") {
        digitalWrite(PIN_MOTOR_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_TRAZ, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO, LOW);
        digitalWrite(PIN_MOTOR_ESQUERDO, HIGH);
    }  
    else if (acao == "esquerda_frente") {
        digitalWrite(PIN_MOTOR_FRENTE, LOW);
        digitalWrite(PIN_MOTOR_TRAZ, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO, LOW);
    } else if (acao == "direita_frente") {
        digitalWrite(PIN_MOTOR_FRENTE, LOW);
        digitalWrite(PIN_MOTOR_TRAZ, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO, LOW);
        digitalWrite(PIN_MOTOR_ESQUERDO, HIGH);
    } else if (acao == "esquerda_tras") {
        digitalWrite(PIN_MOTOR_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_TRAZ, LOW);
        digitalWrite(PIN_MOTOR_DIREITO, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO, LOW);
    } else if (acao == "direita_tras") {
        digitalWrite(PIN_MOTOR_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_TRAZ, LOW);
        digitalWrite(PIN_MOTOR_DIREITO, LOW);
        digitalWrite(PIN_MOTOR_ESQUERDO, HIGH);
    } else if (acao == "parar") {
        digitalWrite(PIN_MOTOR_FRENTE, HIGH);
        digitalWrite(PIN_MOTOR_TRAZ, HIGH);
        digitalWrite(PIN_MOTOR_DIREITO, HIGH);
        digitalWrite(PIN_MOTOR_ESQUERDO, HIGH);
    }


    server.send(200, "application/json", "{\"status\":\"ok\"}");
}


void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_IP, gateway, subnet);


    pinMode(PIN_MOTOR_FRENTE, OUTPUT);
    pinMode(PIN_MOTOR_TRAZ, OUTPUT);
    pinMode(PIN_MOTOR_DIREITO, OUTPUT);
    pinMode(PIN_MOTOR_ESQUERDO, OUTPUT);


    digitalWrite(PIN_MOTOR_FRENTE, HIGH);
    digitalWrite(PIN_MOTOR_TRAZ, HIGH);
    digitalWrite(PIN_MOTOR_DIREITO, HIGH);
    digitalWrite(PIN_MOTOR_ESQUERDO, HIGH);


    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/comando", HTTP_POST, handleComando);


    server.begin();
}


void loop() {
    server.handleClient();
}

