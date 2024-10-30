# Controle do Carrinho com ESP32

Este projeto utiliza um microcontrolador ESP32 para criar um carrinho controlado via Wi-Fi. O ESP32 funciona como um hotspot, permitindo que dispositivos se conectem a ele e enviem comandos para controlar o carrinho.

## Funcionalidades

- **Hotspot**: O ESP32 cria um ponto de acesso Wi-Fi com o SSID `"MeuHotspot"` e a senha `"00000000"`.
- **Controle via Web**: Uma interface web permite controlar o carrinho através de botões de direção.
- **Comandos**: Os comandos disponíveis incluem:
  - Avançar
  - Recuar
  - Virar à esquerda
  - Virar à direita
  - Parar (coloca todos os pinos em nível LOW)

## Estrutura do Projeto

- **main.cpp**: Código principal que configura o ESP32 como ponto de acesso e gerencia as requisições HTTP.
- **index.html**: Código HTML embutido que fornece a interface de controle.

## Requisitos

### Hardware

- ESP32
- 4 relés T73-S-105D
- 4 transistores C32725
- 4 resistores de 1k ohm
- 1 carrinho de brinquedo
- 2 motores 5V DC

### Software

- Arduino IDE ou PlatformIO para compilar e carregar o código no ESP32.
- Biblioteca `ArduinoJson` (ArduinoJson v6 ou superior).
- Biblioteca `WebServer`, incluída na instalação padrão do ESP32.

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




