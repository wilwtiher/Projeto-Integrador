# Aluno: Wilton Lacerda Silva Júnior
## Matrícula: TIC370100193
# Video explicativo: https://youtu.be/huCbdb1L5Qg
# Projeto Integrador - Embarcatech
O objetivo do projeto é revisar e consolidar os conhecimentos adquiridos sobre o microcontrolador RP2040 e os principais recursos disponíveis na placa de desenvolvimento BitDogLab.
## Funcionalidades

- **Joystick:**  
  - O Joystick funciona como dois potenciômetros, podendo assim ler os volores do X e Y dele com ADC.
  - Ele será usado para passar dados para os LEDs, tanto o LED rgb quanto a matriz de LEDs, além do display.
- **2 Leds:**
  - Os leds serão um tipo de retorno visual do joystick.
- **Display LED**
  - O display mostrará por meio de um quadrado se movimentando em qual estado o joystick se encontra.
- **2 Botões**
  - Os botões serão usados para alterar o estado do buzzer.
- **Buzzer**
  - O buzzer será um retorno sonoro dos botões.
- **Matriz de LEDs**
  - A matriz de LED retornará uma seta apontando para a direção a qual o joystick está.
- **UART**
  - A UART será responsavel por passar o valor captado pelo ADC dos potenciômetros para um monitor serial.
# Requisitos
## Hardware:

- Raspberry Pi Pico W.
- 2 potenciômetros nas portas 26 e 27.
- 3 LEDS nas portas 11 e 13.
- 2 botões nas portas 5 e 6.
- 1 display ssd1306 com o sda na porta 14 e o scl na porta 15.
- 1 buzzer na porta 10
- Matriz de LED 5x5 na porta 7

## Software:

- Ambiente de desenvolvimento VS Code com extensão Pico SDK.

# Instruções de uso
## Configure o ambiente:
- Certifique-se de que o Pico SDK está instalado e configurado no VS Code.
- Compile o código utilizando a extensão do Pico SDK.
## Teste:
- Utilize a placa BitDogLab para o teste. Caso não tenha, conecte os hardwares informados acima nos pinos correspondentes.

# Explicação do projeto:
## Contém:
- O projeto terá quatro meios de entrada: 2 potenciômertros e 2 botões.
- Também contará com saída visual, com 1 matriz de led 5x5, 1 led rgb, 1 display de led 128x64 e 1 saida de informação na UART.
- Tem também 1 saida sonora com 1 buzzer.
## Funcionalidades:
- O valor captado pelo joystick alterará aonde o quadrado está posicionado no display de led.
- O valor captado pelo joystick alterará a intensidade do led azul e do led vermelho do led central.
- O valor captado pelo joystick alterará a seta desenhada na matriz de led.
- O botão A ligará o buzzer.
- O botão B desligará o buzzer.
- A saida UART enviará o valor do potênciometro convertido, de -2048 ate 2047.
