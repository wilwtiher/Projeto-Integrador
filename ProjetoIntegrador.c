// Importar bibliotecas
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "hardware/pio.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "ws2812.pio.h"

// Para o I2C
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C

// Matriz de LEDS
#define IS_RGBW false
#define NUM_PIXELS 25
#define WS2812_PIN 7
// Armazenar a cor (Entre 0 e 255 para intensidade)
#define led_r 5 // Intensidade do vermelho
#define led_g 5 // Intensidade do verde
#define led_b 5 // Intensidade do azul
// Pinos
// LEDS
#define led_RED 13   // Red=13, Blue=12, Green=11
#define led_BLUE 12  // Red=13, Blue=12, Green=11
#define led_GREEN 11 // Red=13, Blue=12, Green=11
// Botoes
#define botao_pinA 5 // Botão A = 5, Botão B = 6 , BotãoJoy = 22
#define botao_pinB 6 // Botão A = 5, Botão B = 6 , BotãoJoy = 22
// Joysticks
#define VRY_PIN 26   // Pino do Joystick Y
#define VRX_PIN 27   // Pino do Joystick X
// Buzzer
#define buzzer 10    // Pino do buzzer A

// Variáveis globais
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)
bool display = true;
bool LEDS = true;
bool Verde = false;
bool cor = true;
bool alarme = false;
static volatile int8_t contador = 0; // Variável para qual frame será chamado da matriz de LEDs
int16_t displayX = 0;
int16_t displayY = 0;
// Variável para os frames da matriz de LEDs
bool led_buffer[4][NUM_PIXELS] = {
    {
        // Seta para esquerda
        0, 0, 1, 0, 0, 
        0, 1, 0, 0, 0, 
        1, 1, 1, 1, 1, 
        0, 1, 0, 0, 0, 
        0, 0, 1, 0, 0  
    },
    {
        // Seta para direita
        0, 0, 1, 0, 0, 
        0, 0, 0, 1, 0, 
        1, 1, 1, 1, 1, 
        0, 0, 0, 1, 0, 
        0, 0, 1, 0, 0  
    },
    {
        // Seta para cima (invertida)
        0, 0, 1, 0, 0, 
        0, 0, 1, 0, 0, 
        1, 0, 1, 0, 1, 
        0, 1, 1, 1, 0, 
        0, 0, 1, 0, 0  
    },
    {
        // Seta para baixo (invertida)
        0, 0, 1, 0, 0, 
        0, 1, 1, 1, 0, 
        1, 0, 1, 0, 1, 
        0, 0, 1, 0, 0, 
        0, 0, 1, 0, 0  
    }
};

// Funções para matriz LEDS
static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}

void set_one_led(uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(r, g, b);

    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {
        if (led_buffer[contador /*variavel do arrey do buffer*/][i])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0); // Desliga os LEDs com zero no buffer
        }
    }
}

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());
    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 150000) // 150 ms de debouncing
    {
        if (gpio == botao_pinA)
        {
            alarme = true;
        }
        if (gpio == botao_pinB)
        {
            alarme = false;
        }
        last_time = current_time; // Atualiza o tempo do último evento
    }
}

// Funcao para iniciar pino PWM
uint pwm_init_gpio(uint gpio, uint wrap)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);

    pwm_set_enabled(slice_num, true);
    return slice_num;
}
// Funcao para iniciar pino PWM Buzzer
uint pwm_init_gpio_buzzer(uint gpio, uint wrap)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, wrap);
    // Define o clock divider como 440
    pwm_set_clkdiv(slice_num, 440.0f);
    pwm_set_enabled(slice_num, true);
    return slice_num;
}

// Codigo principal do programa
int main()
{
    stdio_init_all();

    // Botões
    gpio_init(botao_pinA);
    gpio_set_dir(botao_pinA, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(botao_pinA);          // Habilita o pull-up interno
    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(botao_pinA, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(botao_pinB);
    gpio_set_dir(botao_pinB, GPIO_IN); // Configura o pino como entrada
    gpio_pull_up(botao_pinB);          // Habilita o pull-up interno
    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(botao_pinB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Potenciometros
    adc_init();
    adc_gpio_init(VRY_PIN);
    adc_gpio_init(VRX_PIN);

    // PWMs
    uint pwm_wrap = 4096;
    uint pwm_slice_red = pwm_init_gpio(led_RED, pwm_wrap);
    uint pwm_slice_blue = pwm_init_gpio(led_BLUE, pwm_wrap);
    uint pwm_slice_buzzer = pwm_init_gpio_buzzer(buzzer, pwm_wrap);

    uint32_t last_print_time = 0;

    // Para o display
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display
    // Limpa o display.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd); // Atualiza o display

    // configuracao do PIO
    PIO pio = pio0;
    int sm = 0;
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, WS2812_PIN, 800000, IS_RGBW);


    // Loop Infinito
    while (true) {
        // Quadrado que movimenta
        adc_select_input(1);
        int16_t vrx_value = adc_read();
        adc_select_input(0);
        int16_t vry_value = adc_read();
        displayX = (vrx_value / 34);
        displayY = (vry_value / 73);
        displayY = displayY - 56;
        displayY = abs(displayY);
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, displayY, displayX, 8, 8, 1, 1); // Desenha um quadrado
        ssd1306_rect(&ssd, 3, 3, 122, 58, display, 0);      // Desenha um retângulo
        ssd1306_send_data(&ssd);                            // Atualiza o display
        // Buzzer
        if (alarme)
        {
            pwm_set_gpio_level(buzzer, 2048);
        }else{
            pwm_set_gpio_level(buzzer, 0);
        }
        // Acender LEDs
        pwm_set_gpio_level(led_RED, vrx_value);
        pwm_set_gpio_level(led_BLUE, vry_value);
        // Matriz de LEDs
        // Verifica qual o lado mais puxado
        vrx_value = vrx_value - 2048;
        vry_value = vry_value - 2048;
        if(abs(vrx_value) > abs(vry_value)){
            if(vrx_value > 0){
                contador = 0;
            } else{
                contador = 1;
            }
        } else{
            if(vry_value > 0){
                contador = 2;
            } else{
                contador = 3;
            }
        }
        set_one_led(led_r, led_g, led_b);

        // Imprimir valores no monitor serial
        printf("Valor do potenciometro X: %c\n", vrx_value);
        printf("Valor do potenciometro Y: %c\n", vry_value);
        sleep_ms(250);
    }
}
