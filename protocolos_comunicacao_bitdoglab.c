#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"

#include "include/ssd1306.h"
#include "include/font.h"

#include "include/vetores_numeros.h"
#include "include/matriz_led_control.h"

// I2C defines
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
ssd1306_t ssd;

// UART defines
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart0
#define BAUD_RATE 115200

// Use pins 4 and 5 for UART1
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define UART_TX_PIN 4
#define UART_RX_PIN 5

// LED RGB Defines
#define RED_LED_RGB 13
#define GREEN_LED_RGB 11
#define BLUE_LED_RGB 12

// Definições para os botões
#define BOTAO_A 5
#define BOTAO_B 6
static volatile uint32_t ultimo_tempo = 0;

// Definições para a matriz de leds RGB
#define OUT_PIN 7
pio_t meu_pio = {
    .pio = pio0,       
    .ok = false,       
    .i = 0,              
    .r = 0.0,          
    .g = 0.0,          
    .b = 0.0, 
    .sm = 0          
};

char input;

void setup_gpios();
void realizar_troca();
static void gpio_irq_handler(uint gpio, uint32_t events);
int64_t alarm_callback(alarm_id_t id, void *user_data) ;

int main()
{
    init_pio_routine(&meu_pio, OUT_PIN);
    setup_gpios();
    
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    bool cor = true;
    while (true) {
        printf("Hello, world!\n");

        cor = !cor;
        // Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor); // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor); // Desenha um retângulo
        ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 10); // Desenha uma string
        ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 30); // Desenha uma string
        ssd1306_draw_string(&ssd, "JP TECH", 15, 48); // Desenha uma string      
        ssd1306_send_data(&ssd); // Atualiza o display

        sleep_ms(1000);
    }
}

void setup_gpios()
{
    gpio_init(RED_LED_RGB);                 
    gpio_set_dir(RED_LED_RGB, GPIO_OUT);  
    gpio_init(BLUE_LED_RGB);                 
    gpio_set_dir(BLUE_LED_RGB, GPIO_OUT);
    gpio_init(GREEN_LED_RGB);                 
    gpio_set_dir(GREEN_LED_RGB, GPIO_OUT);  
    
    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);        
    gpio_pull_up(BOTAO_A);
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);        
    gpio_pull_up(BOTAO_B); 

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd); // Configura o display
    ssd1306_send_data(&ssd); // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

void realizar_troca()
{
    switch(input)
    {
        case '0':
            desenho_pio(numero_0, &meu_pio);
            break;

        case '1':
            desenho_pio(numero_1, &meu_pio);
            break;

        case '2':
            desenho_pio(numero_2, &meu_pio);
            break;

        case '3':
            desenho_pio(numero_3, &meu_pio);
            break;

        case '4':
            desenho_pio(numero_4, &meu_pio);
            break;

        case '5':
            desenho_pio(numero_5, &meu_pio);
            break;

        case '6':
            desenho_pio(numero_6, &meu_pio);
            break;

        case '7':
            desenho_pio(numero_7, &meu_pio);
            break;

        case '8':
            desenho_pio(numero_8, &meu_pio);
            break;

        case '9':
            desenho_pio(numero_9, &meu_pio);
            break;
    
        default:
            break;
    }
}

static void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Aplicando debouncing na interrupção
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time());

    if (tempo_atual - ultimo_tempo > 220000) // 220 ms de debouncing
    {
        ultimo_tempo = tempo_atual;


        if (gpio == BOTAO_A)
        {
            gpio_put(GREEN_LED_RGB, !gpio_get(GREEN_LED_RGB));
            printf("Alternando LED VERDE");
            ssd1306_fill(&ssd, false);
            ssd1306_send_data(&ssd);
            ssd1306_draw_string(&ssd, " ALTERNANDO ", 8, 10); // Desenha uma string
            ssd1306_draw_string(&ssd, "LED VERDE", 20, 30); // Desenha uma string
            ssd1306_send_data(&ssd);

        }

        if (gpio == BOTAO_B)
        {
            gpio_put(BLUE_LED_RGB, !gpio_get(BLUE_LED_RGB));
            printf("Alternando LED VERDE");
            ssd1306_fill(&ssd, false);
            ssd1306_send_data(&ssd);
            ssd1306_draw_string(&ssd, "ALTERNANDO", 8, 10); // Desenha uma string
            ssd1306_draw_string(&ssd, "LED AZUL", 20, 30); // Desenha uma string
            ssd1306_send_data(&ssd);
        }
    }
}

int64_t alarm_callback(alarm_id_t id, void *user_data) 
{
    // Put your timeout handler code in here
    return 0;
}
