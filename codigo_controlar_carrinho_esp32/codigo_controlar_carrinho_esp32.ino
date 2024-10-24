#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char* ssid = "MeuHotspot";  // Nome do Hotspot
const char* password = "00000000";  // Senha do Hotspot

// Configurações do IP estático
IPAddress local_IP(192, 168, 1, 1);  // IP estático desejado para o hotspot
IPAddress gateway(192, 168, 1, 1);   // Gateway
IPAddress subnet(255, 255, 255, 0);  // Máscara de sub-rede

WebServer server(80);
const int D2 = 4;  // Pino de controle do carro

// HTML da página de controle embutida
const char controlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Controle do Carrinho</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background-color: #f7f7f7;
            display: flex;
            justify-content: center;
            align-items: center;
            height: 100vh;
            margin: 0;
        }
        .controle-container {
            display: grid;
            grid-template-columns: repeat(3, 300px);  /* Dobrado o tamanho */
            grid-template-rows: repeat(3, 300px);     /* Dobrado o tamanho */
            gap: 20px;  /* Aumentado o espaço entre os botões */
            align-items: center;
            justify-items: center;
        }
        .botao {
            width: 200px;   /* Dobrado o tamanho */
            height: 200px;  /* Dobrado o tamanho */
            font-size: 60px;  /* Dobrado o tamanho da fonte */
            background-color: #4CAF50;
            color: white;
            border: none;
            border-radius: 10px;
            cursor: pointer;
            display: flex;
            justify-content: center;
            align-items: center;
        }
        .botao:hover {
            background-color: #45a049;
        }
        .botao-stop {
            background-color: red;
            font-size: 48px;  /* Dobrado o tamanho da fonte */
            border-radius: 50%;
        }
        .botao-stop:hover {
            background-color: darkred;
        }
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
            console.log("Comando enviado: " + acao);  // Apenas registra o comando no console
        })
        .catch(error => console.error("Erro ao enviar comando: " + error));  // Mostra erros no console
    }
</script>

</body>
</html>
)rawliteral";

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

  if (acao == "frente") {
    digitalWrite(D2, HIGH);  // Movendo para frente
    server.send(200, "application/json", "{\"status\":\"Movendo para frente\"}");
  } else if (acao == "tras") {
    digitalWrite(D2, LOW);  // Movendo para trás
    server.send(200, "application/json", "{\"status\":\"Movendo para trás\"}");
  } else if (acao == "esquerda") {
    // Comando esquerda
    digitalWrite(D2, LOW);  // Movendo para esquerda
    server.send(200, "application/json", "{\"status\":\"Virando à esquerda\"}");
  } else if (acao == "direita") {
    // Comando direita
    digitalWrite(D2, LOW);  // Movendo para direita
    server.send(200, "application/json", "{\"status\":\"Virando à direita\"}");
  } else if (acao == "parar") {
    digitalWrite(D2, LOW);  // Parando
    server.send(200, "application/json", "{\"status\":\"Parado\"}");
  } else {
    server.send(400, "application/json", "{\"status\":\"erro\", \"mensagem\":\"Comando desconhecido\"}");
  }
}

void setup() {
  Serial.begin(115200);

  // Inicializa o modo de ponto de acesso
  WiFi.softAP(ssid, password);

  // Configura IP estático
  WiFi.softAPConfig(local_IP, gateway, subnet);

  Serial.println("Hotspot criado!");
  Serial.print("IP do Hotspot: ");
  Serial.println(WiFi.softAPIP());

  pinMode(D2, OUTPUT);

  server.on("/", HTTP_GET, handleRoot);  // Rota para a página de controle
  server.on("/api/comando", HTTP_POST, handleComando);  // Rota para os comandos

  server.begin();
}

void loop() {
  server.handleClient();
}