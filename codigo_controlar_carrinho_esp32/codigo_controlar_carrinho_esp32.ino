#include <WiFi.h>
#include <WebServer.h>

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

// Variáveis de controle de movimento
bool movimentoFrente = false;
bool movimentoTras = false;

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
    let intervaloFrente, intervaloTras, intervaloEsquerda, intervaloDireita;

    document.getElementById('frente').onmousedown = () => {
        enviarComando('frente');
        intervaloFrente = setInterval(() => {
            enviarComando('frente');
        }, 200);
    };

    document.getElementById('frente').onmouseup = () => {
        clearInterval(intervaloFrente);
    };

    document.getElementById('frente').onmouseleave = () => {
        clearInterval(intervaloFrente);
    };

    document.getElementById('tras').onmousedown = () => {
        enviarComando('tras');
        intervaloTras = setInterval(() => {
            enviarComando('tras');
        }, 200);
    };

    document.getElementById('tras').onmouseup = () => {
        clearInterval(intervaloTras);
    };

    document.getElementById('tras').onmouseleave = () => {
        clearInterval(intervaloTras);
    };

    document.getElementById('esquerda').onmousedown = () => {
        enviarComando('esquerda');
        intervaloEsquerda = setInterval(() => {
            enviarComando('esquerda');
        }, 200);
    };

    document.getElementById('esquerda').onmouseup = () => {
        clearInterval(intervaloEsquerda);
    };

    document.getElementById('esquerda').onmouseleave = () => {
        clearInterval(intervaloEsquerda);
    };

    document.getElementById('direita').onmousedown = () => {
        enviarComando('direita');
        intervaloDireita = setInterval(() => {
            enviarComando('direita');
        }, 200);
    };

    document.getElementById('direita').onmouseup = () => {
        clearInterval(intervaloDireita);
    };

    document.getElementById('direita').onmouseleave = () => {
        clearInterval(intervaloDireita);
    };

    document.getElementById('parar').onclick = () => enviarComando('parar');

    function enviarComando(acao) {
        fetch("/api/comando", {
            method: "POST",
            headers: {
                "Content-Type": "application/x-www-form-urlencoded"
            },
            body: "acao=" + acao
        })
        .then(response => response.text())
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
    String acao = body.substring(body.indexOf('=') + 1); // Extrai a ação da string

    // Ativa todos os pinos
    digitalWrite(PIN_FRENTE, HIGH);
    digitalWrite(PIN_TRAS, HIGH);
    digitalWrite(PIN_ESQUERDA, HIGH);
    digitalWrite(PIN_DIREITA, HIGH);
    Serial.println("Todos os pinos ativados:");
    printPinStates();

    if (acao == "frente") {
        movimentoFrente = true;
        Serial.println("Comando: Movendo para frente");
    } else if (acao == "tras") {
        movimentoTras = true;
        Serial.println("Comando: Movendo para trás");
    } else if (acao == "esquerda") {
        digitalWrite(PIN_ESQUERDA, LOW);
        Serial.println("Comando: Virando à esquerda");
        delay(500); // Gira por 0,5 segundos
        digitalWrite(PIN_ESQUERDA, HIGH);
    } else if (acao == "direita") {
        digitalWrite(PIN_DIREITA, LOW);
        Serial.println("Comando: Virando à direita");
        delay(500); // Gira por 0,5 segundos
        digitalWrite(PIN_DIREITA, HIGH);
    } else if (acao == "parar") {
        movimentoFrente = false;
        movimentoTras = false;
        Serial.println("Comando: Parado");
        digitalWrite(PIN_FRENTE, HIGH);
        digitalWrite(PIN_TRAS, HIGH);
    } else {
        server.send(400, "text/plain", "Comando desconhecido");
        return;
    }

    server.send(200, "text/plain", "Comando recebido");
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

    digitalWrite(PIN_FRENTE, HIGH);
    digitalWrite(PIN_TRAS, HIGH);
    digitalWrite(PIN_ESQUERDA, HIGH);
    digitalWrite(PIN_DIREITA, HIGH);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/api/comando", HTTP_POST, handleComando);  

    server.begin();
}

void loop() {
    server.handleClient();

    // Executa a lógica de movimento com base nas variáveis
    if (movimentoFrente) {
        digitalWrite(PIN_FRENTE, LOW); // Ativa o movimento para frente
    } else {
        digitalWrite(PIN_FRENTE, HIGH); // Para o movimento para frente
    }

    if (movimentoTras) {
        digitalWrite(PIN_TRAS, LOW); // Ativa o movimento para trás
    } else {
        digitalWrite(PIN_TRAS, HIGH); // Para o movimento para trás
    }
}
