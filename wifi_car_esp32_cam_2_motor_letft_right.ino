#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "esp_camera.h"

// ==================== CONFIGURAÇÕES DA CÂMERA ======================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ==================== REDE WI-FI ======================
const char* ssid = "ESP32-CAM-Car";
const char* password = "12345678";
IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

// ==================== PINOS DOS MOTORES ======================
#define motor_esquerdo_frente 12
#define motor_esquerdo_traz   13
#define motor_direito_frente  15
#define motor_direito_traz    14

// ==================== CONTROLE ======================
WebServer server(80);

// ==================== HTML COM TRANSMISSÃO + CONTROLE ======================
const char html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Controle do Carrinho</title>
    <style>
        * { user-select: none; }
        body { font-family: Arial, sans-serif; text-align: center; background-color: #f3f3f3; }
        .controle-container { display: inline-grid; grid-template-columns: 50px 50px 50px; gap: 10px; }
        .botao { font-size: 24px; width: 50px; height: 50px; background-color: #4CAF50; color: white; border: none; border-radius: 5px; cursor: pointer; }
        .botao-stop { background-color: #f44336; }
        .botao:active { background-color: black; }
         img { width: 90%%; max-width: 90%%;  min-height: 480px;max-height: 70%%; border-radius: 10px; margin: 20px auto; display: block;  transform: rotate(0deg);}
    </style>
</head>
<body>
  <h2>Controle do Carrinho ESP32-CAM</h2>
  <img id="camera" src="/cam">
  <div class="controle-container">
    <button class="botao" id="esquerda_frente">↖</button>
    <button class="botao" id="frente">▲</button>
    <button class="botao" id="direita_frente">↗</button>
    
    <button class="botao" id="esquerda">◀</button>
    <button class="botao botao-stop" id="parar">⬛</button>
    <button class="botao" id="direita">▶</button>
    
    <button class="botao" id="esquerda_tras">↙</button>
    <button class="botao" id="tras">▼</button>
    <button class="botao" id="direita_tras">↘</button>
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
        setTimeout(function() {
          //atualizarAcao();
        }, 200);
    }
    //atualizarAcao();
    document.getElementById('frente').ontouchstart = () => {
        estadoFrente = true;
        atualizarAcao();
    };
    document.getElementById('frente').ontouchend = () => {
        estadoFrente = false;
        atualizarAcao();
    };
    document.getElementById('frente').onmousedown = () => {
        estadoFrente = true;
        atualizarAcao();
    };
    document.getElementById('frente').onmouseup = () => {
        estadoFrente = false;
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
    document.getElementById('tras').onmousedown = () => {
        estadoTras = true;
        atualizarAcao();
    };
    document.getElementById('tras').onmouseup = () => {
        estadoTras = false;
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
    document.getElementById('direita').onmousedown = () => {
        estadoDireita = true;
        atualizarAcao();
    };
    document.getElementById('direita').onmouseup = () => {
        estadoDireita = false;
        atualizarAcao();
    };

    document.getElementById('direita_tras').ontouchstart = () => {
        estadoDireita = true;
        estadoTras = true;
        atualizarAcao();
    };
    document.getElementById('direita_tras').ontouchend = () => {
        estadoDireita = false;
        estadoTras = false;
        atualizarAcao();
    };
    document.getElementById('direita_tras').onmousedown = () => {
        estadoDireita = true;
        estadoTras = true;
        atualizarAcao();
    };
    document.getElementById('direita_tras').onmouseup = () => {
        estadoDireita = false;
        estadoTras = false;
        atualizarAcao();
    };

    document.getElementById('direita_frente').ontouchstart = () => {
        estadoDireita = true;
        estadoFrente = true;
        atualizarAcao();
    };
    document.getElementById('direita_frente').ontouchend = () => {
        estadoDireita = false;
        estadoFrente = false;
        atualizarAcao();
    };
    document.getElementById('direita_frente').onmousedown = () => {
        estadoDireita = true;
        estadoFrente = true;
        atualizarAcao();
    };
    document.getElementById('direita_frente').onmouseup = () => {
        estadoDireita = false;
        estadoFrente = false;
        atualizarAcao();
    };





    document.getElementById('esquerda').ontouchstart = () => {
        estadoEsquerda = true;
        atualizarAcao();
    };
    document.getElementById('esquerda').ontouchend = () => {
        estadoEsquerda = false;
        atualizarAcao();
    };
    document.getElementById('esquerda').onmousedown = () => {
        estadoEsquerda = true;
        atualizarAcao();
    };
    document.getElementById('esquerda').onmouseup = () => {
        estadoEsquerda = false;
        atualizarAcao();
    };

    document.getElementById('esquerda_tras').ontouchstart = () => {
        estadoEsquerda = true;
        estadoTras = true;
        atualizarAcao();
    };
    document.getElementById('esquerda_tras').ontouchend = () => {
        estadoEsquerda = false;
        estadoTras = false;
        atualizarAcao();
    };
    document.getElementById('esquerda_tras').onmousedown = () => {
        estadoEsquerda = true;
        estadoTras = true;
        atualizarAcao();
    };
    document.getElementById('esquerda_tras').onmouseup = () => {
        estadoEsquerda = false;
        estadoTras = false;
        atualizarAcao();
    };

    document.getElementById('esquerda_frente').ontouchstart = () => {
        estadoEsquerda = true;
        estadoFrente = true;
        atualizarAcao();
    };
    document.getElementById('esquerda_frente').ontouchend = () => {
        estadoEsquerda = false;
        estadoFrente = false;
        atualizarAcao();
    };
    document.getElementById('esquerda_frente').onmousedown = () => {
        estadoEsquerda = true;
        estadoFrente = true;
        atualizarAcao();
    };
    document.getElementById('esquerda_frente').onmouseup = () => {
        estadoEsquerda = false;
        estadoFrente = false;
        atualizarAcao();
    };




    document.getElementById('parar').ontouchstart = () => {
        estadoFrente = false;
        estadoTraz = false;
        estadoEsquerda = false;
        estadoDireita = false;
        atualizarAcao();
    };
    document.getElementById('parar').ontouchend = () => {
        estadoParar = true;
        atualizarAcao();
    };
    document.getElementById('parar').onmousedown = () => {
         estadoFrente = false;
        estadoTraz = false;
        estadoEsquerda = false;
        estadoDireita = false;
        atualizarAcao();
    };
    document.getElementById('parar').onmouseup = () => {
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

    function atualizarCamera() {
        const cam = document.getElementById('camera');
        cam.src = "/cam?" + new Date().getTime(); // evita cache
        setTimeout(atualizarCamera, 200); // 3 frames por segundo
    }
    atualizarCamera();
</script>
</body>
</html>
)rawliteral";

// ==================== SETUP DA CÂMERA ======================
void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (psramFound()) {
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_QQVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_camera_init(&config);
}

void handleCam() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    server.send(500, "text/plain", "Erro ao capturar imagem");
    return;
  }

  server.sendHeader("Content-Type", "image/jpeg");
  server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
  esp_camera_fb_return(fb);
}


void parar() {
  digitalWrite(motor_esquerdo_frente, LOW);
  digitalWrite(motor_esquerdo_traz, LOW);
  digitalWrite(motor_direito_frente, LOW);
  digitalWrite(motor_direito_traz, LOW);
}

void frente() {
  digitalWrite(motor_esquerdo_frente, HIGH);
  digitalWrite(motor_esquerdo_traz, LOW);
  digitalWrite(motor_direito_frente, HIGH);
  digitalWrite(motor_direito_traz, LOW);
}

void tras() {
  digitalWrite(motor_esquerdo_frente, LOW);
  digitalWrite(motor_esquerdo_traz, HIGH);
  digitalWrite(motor_direito_frente, LOW);
  digitalWrite(motor_direito_traz, HIGH);
}

void esquerdoFrente() {
  digitalWrite(motor_esquerdo_frente, HIGH);
  digitalWrite(motor_esquerdo_traz, LOW);
  digitalWrite(motor_direito_frente, LOW);
  digitalWrite(motor_direito_traz, HIGH);
}

void esquerdoTras() {
  digitalWrite(motor_esquerdo_frente, LOW);
  digitalWrite(motor_esquerdo_traz, HIGH);
  digitalWrite(motor_direito_frente, HIGH);
  digitalWrite(motor_direito_traz, LOW);
}

void direitoFrente() {
  digitalWrite(motor_esquerdo_frente, LOW);
  digitalWrite(motor_esquerdo_traz, HIGH);
  digitalWrite(motor_direito_frente, HIGH);
  digitalWrite(motor_direito_traz, LOW);
}

void direitoTras() {
  digitalWrite(motor_esquerdo_frente, HIGH);
  digitalWrite(motor_esquerdo_traz, LOW);
  digitalWrite(motor_direito_frente, LOW);
  digitalWrite(motor_direito_traz, HIGH);
}

void handleAPI() {
  StaticJsonDocument<200> doc;
  deserializeJson(doc, server.arg("plain"));
  String comando = doc["acao"];

  if (comando == "frente") frente();
  else if (comando == "tras") tras();
  else if (comando == "esquerda_frente") esquerdoFrente();
  else if (comando == "esquerda_tras") esquerdoTras();
  else if (comando == "direita_frente") direitoFrente();
  else if (comando == "direita_tras") direitoTras();
  else parar();

  server.send(200, "application/json", "{\"status\":\"ok\"}");
}

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  startCamera();

  pinMode(4, OUTPUT);  // GPIO 4 controla o LED flash
  digitalWrite(4, HIGH); // Apaga inicialmente (LOW = desligado, HIGH = ligado)

  pinMode(motor_esquerdo_frente, OUTPUT);
  pinMode(motor_esquerdo_traz, OUTPUT);
  pinMode(motor_direito_frente, OUTPUT);
  pinMode(motor_direito_traz, OUTPUT);

  server.on("/", HTTP_GET, []() { server.send(200, "text/html", html); });
  server.on("/cam", HTTP_GET, handleCam);
  server.on("/api/comando", HTTP_POST, handleAPI);
  server.begin();
}

void loop() {
  server.handleClient();
}
