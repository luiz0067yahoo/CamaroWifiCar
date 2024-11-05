# Controle do Carrinho com ESP32

Este projeto utiliza um microcontrolador ESP32 para criar um carrinho controlado via Wi-Fi. O ESP32 funciona como um hotspot, permitindo que dispositivos se conectem a ele e enviem comandos para controlar o carrinho.

## Funcionalidades

- **Hotspot**: O ESP32 cria um ponto de acesso Wi-Fi com o SSID `"Carrinho_WiFi"` e a senha `"00000000"`.
- **IP**: htp://192.168.1.1
- **Controle via Web**: Uma interface web permite controlar o carrinho através de botões de direção.
- **Comandos**: Os comandos disponíveis incluem:
  - Avançar (LOW)
  - Recuar (LOW)
  - Virar à esquerda (LOW)
  - Virar à direita (LOW)
  - Parar (coloca todos os pinos em nível UP)

## Estrutura do Projeto

- **main.cpp**: Código principal que configura o ESP32 como ponto de acesso e gerencia as requisições HTTP.
- **index.html**: Código HTML embutido que fornece a interface de controle.

## Requisitos

### Hardware

- **ESP32**
- **Relés T73-S-105D** (4 unidades): Controlam a ativação dos motores do carrinho.
- **Transistores C32725** (4 unidades): Conectados aos pinos de controle do ESP32, eles permitem acionar os relés com mais potência.
- **Resistores de 1k ohm** (4 unidades): Limitam a corrente para a base de cada transistor.
- **Carrinho de brinquedo**
- **Motores 5V DC** (2 unidades)
- **Carregador portatil 12.000 mha** (1 unidades)
- **Cabo mini usb** (2 unidades)
- **Cabo flexivel de cobre 0.3 vermelho** (50 cm)
- **Cabo flexivel de cobre 0.3 preto** (50 cm)
- **Tubo De Solda Estanho C/ Fluxo Ra (t2)** (63x37 1mm)
- **Pasta De Solda - Fluxo Em Pasta - Pote** (110g)
- **Ferro De Soldar Hikari Profissional 127v Sc-30** (25W)
- **Fonte de alimentação AC 110/220V DC 5V 1A** (1)
- **Protoboard Breadboard 400 Pontos Furos Pinos** (1)
- **O LED difuso de 5mm** (4)
- **Cabos Jumper 20cm Macho X Macho** (10)
- **Cabos Jumper 20cm Femea X Femea** (10)

#### Conexões

- **Pino D18 (Frente)**: Conectado a um relé que ativa o motor para mover o carrinho para a frente.
- **Pino D19 (Ré)**: Conectado a um relé que ativa o motor para mover o carrinho para trás.
- **Pino D22 (Esquerda)**: Conectado a um relé que ativa o motor para virar o carrinho para a esquerda.
- **Pino D23 (Direita)**: Conectado a um relé que ativa o motor para virar o carrinho para a direita.

Cada pino de controle do ESP32 é conectado à base de um transistor, que, por sua vez, controla o relé correspondente. Assim, quando um comando é enviado, o relé ativo é acionado e os demais são desativados (nível LOW), permitindo que apenas uma ação aconteça por vez.

### Software

- **Arduino IDE** ou **PlatformIO** para compilar e carregar o código no ESP32.
- **Biblioteca ArduinoJson** (v6 ou superior) para processamento de comandos em JSON.
- **Biblioteca WebServer** para criar o servidor HTTP, incluída na instalação padrão do ESP32.

## Como Usar

### Configurar o Ambiente

1. Instale o Arduino IDE e as bibliotecas necessárias.

### Carregar o Código

1. Abra o arquivo `main.cpp` no Arduino IDE.
2. Conecte o ESP32 ao computador.
3. Selecione a placa correta e a porta no menu "Ferramentas".
4. Carregue o código no ESP32.

### Conectar ao Hotspot

1. No seu dispositivo (laptop ou smartphone), conecte-se à rede Wi-Fi "MeuHotspot" usando a senha "00000000".

### Acessar a Interface

1. Abra um navegador web e insira o seguinte URL:
   `http://192.168.1.1/`
2. A interface de controle será exibida, permitindo o controle do carrinho.

## Estrutura de Código

- **Setup**: Configurações iniciais, incluindo a configuração do ponto de acesso e a definição das rotas HTTP.
- **Loop**: Escuta e processa as requisições do cliente, executando cada ação por 1 segundo e depois colocando todos os pinos em nível LOW para garantir apenas uma ação ativa de cada vez.

## Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para discutir melhorias.

