# Controle do Carrinho com ESP32

Este projeto utiliza um microcontrolador ESP32 para criar um carrinho controlado via Wi-Fi. O ESP32 funciona como um hotspot, permitindo que dispositivos se conectem a ele e enviem comandos para controlar o carrinho.

## Funcionalidades

- **Hotspot**: O ESP32 cria um ponto de acesso Wi-Fi com o SSID "MeuHotspot" e a senha "00000000".
- **Controle via Web**: Uma interface web permite controlar o carrinho através de botões de direção.
- **Comandos**: Os comandos disponíveis incluem:
  - Avançar
  - Recuar
  - Virar à esquerda
  - Virar à direita
  - Parar

## Estrutura do Projeto

- `main.cpp`: Código principal que configura o ESP32 como ponto de acesso e gerencia as requisições HTTP.
- `index.html`: Código HTML embutido que fornece a interface de controle.

## Requisitos

- **Hardware**:
  - ESP32
  - Motor controlador (ex: L298N) conectado ao pino D2 do ESP32.

- **Software**:
  - Arduino IDE ou PlatformIO para compilar e carregar o código no ESP32.
  - Biblioteca ArduinoJson (`ArduinoJson` v6 ou superior).
  - Biblioteca WebServer incluída na instalação padrão do ESP32.

## Como Usar

1. **Configurar o Ambiente**:
   - Instale o Arduino IDE e as bibliotecas necessárias.

2. **Carregar o Código**:
   - Abra o arquivo `main.cpp` no Arduino IDE.
   - Conecte seu ESP32 ao computador.
   - Selecione a placa correta e a porta no menu "Ferramentas".
   - Carregue o código no ESP32.

3. **Conectar ao Hotspot**:
   - No seu dispositivo (laptop ou smartphone), conecte-se à rede Wi-Fi "MeuHotspot" usando a senha "00000000".

4. **Acessar a Interface**:
   - Abra um navegador web e insira o seguinte URL:
     ```
     http://192.168.1.1/
     ```
   - A interface de controle será exibida, permitindo o controle do carrinho.

## Estrutura de Código

- **Setup**: Configurações iniciais, incluindo a configuração do ponto de acesso e a definição das rotas HTTP.
- **Loop**: Escuta e processa as requisições do cliente.

## Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para enviar um pull request ou abrir uma issue para discutir melhorias.

## Licença

Este projeto está licenciado sob a MIT License - veja o arquivo [LICENSE](LICENSE) para mais detalhes.
