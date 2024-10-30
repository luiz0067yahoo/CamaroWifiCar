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
const int PIN_FRENTE = 18;  // Frente
const int PIN_TRAS = 19;    // Trás
const int PIN_ESQUERDA = 22; // Esquerda
const int PIN_DIREITA = 23;  // Direita

// HTML da página de controle embutida (mantido como estava)
const char controlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Controle do Carrinho</title>
    <style>
        /* [Conteúdo HTML e CSS mantido como no código original] */
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

  // Função para desativar todos os pinos
  digitalWrite(PIN_FRENTE, LOW);
  digitalWrite(PIN_TRAS, LOW);
  digitalWrite(PIN_ESQUERDA, LOW);
  digitalWrite(PIN_DIREITA, LOW);

  if (acao == "frente") {
    digitalWrite(PIN_FRENTE, HIGH);
    digitalWrite(PIN_TRAS, LOW);
    digitalWrite(PIN_ESQUERDA, LOW);
    digitalWrite(PIN_DIREITA, LOW);
    delay(1000);  // Aguarda 1 segundo
    digitalWrite(PIN_FRENTE, LOW);  // Desativa após o tempo
    server.send(200, "application/json", "{\"status\":\"Movendo para frente\"}");
} else if (acao == "tras") {
    digitalWrite(PIN_TRAS, HIGH);
    digitalWrite(PIN_FRENTE, LOW);
    digitalWrite(PIN_ESQUERDA, LOW);
    digitalWrite(PIN_DIREITA, LOW);
    delay(1000);  // Aguarda 1 segundo
    digitalWrite(PIN_TRAS, LOW);  // Desativa após o tempo
    server.send(200, "application/json", "{\"status\":\"Movendo para trás\"}");
} else if (acao == "esquerda") {
    digitalWrite(PIN_ESQUERDA, HIGH);
    digitalWrite(PIN_FRENTE, LOW);
    digitalWrite(PIN_TRAS, LOW);
    digitalWrite(PIN_DIREITA, LOW);
    delay(1000);  // Aguarda 1 segundo
    digitalWrite(PIN_ESQUERDA, LOW);  // Desativa após o tempo
    server.send(200, "application/json", "{\"status\":\"Virando à esquerda\"}");
} else if (acao == "direita") {
    digitalWrite(PIN_DIREITA, HIGH);
    digitalWrite(PIN_FRENTE, LOW);
    digitalWrite(PIN_TRAS, LOW);
    digitalWrite(PIN_ESQUERDA, LOW);
    delay(1000);  // Aguarda 1 segundo
    digitalWrite(PIN_DIREITA, LOW);  // Desativa após o tempo
    server.send(200, "application/json", "{\"status\":\"Virando à direita\"}");
} else if (acao == "parar") {
    digitalWrite(PIN_FRENTE, LOW);
    digitalWrite(PIN_TRAS, LOW);
    digitalWrite(PIN_ESQUERDA, LOW);
    digitalWrite(PIN_DIREITA, LOW);
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

  // Configura os pinos de controle como saída
  pinMode(PIN_FRENTE, OUTPUT);
  pinMode(PIN_TRAS, OUTPUT);
  pinMode(PIN_ESQUERDA, OUTPUT);
  pinMode(PIN_DIREITA, OUTPUT);

  server.on("/", HTTP_GET, handleRoot);  // Rota para a página de controle
  server.on("/api/comando", HTTP_POST, handleComando);  // Rota para os comandos

  server.begin();
}

void loop() {
  server.handleClient();
}
