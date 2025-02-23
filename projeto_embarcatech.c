#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Definição dos pinos utilizados
#define TRIG_PIN 8  // Pino de disparo do sensor ultrassônico
#define ECHO_PIN 9  // Pino de eco do sensor ultrassônico

#define BUZZER_PIN 10  // Pino do buzzer

const uint LED_RED = 13; // Pino para o led vermelho
const uint LED_GREEN = 11; // Pino para o led verde
const uint LED_BLUE = 12; // Pino para o led azul

volatile bool working = false; // Variável para controlar o funcionamento do sistema

const uint button_a = 5; // Botão para ativar o sistema
const uint button_b = 6; // Botão para desativar o sistema

// Função para definir a cor do LED RGB
void set_color(bool red, bool green, bool blue) {
  gpio_put(LED_RED, red);
  gpio_put(LED_GREEN, green);
  gpio_put(LED_BLUE, blue);
}

// Função para configurar PWM no buzzer
void setup_pwm(uint gpio, float frequency, float duty_cycle) {
    float level;
    gpio_set_function(gpio, GPIO_FUNC_PWM);  // Configura o pino para função PWM
    uint slice = pwm_gpio_to_slice_num(gpio); // Obtém o slice PWM correspondente ao pino

    float clock_div = 125.0;  // Configuração do divisor de clock (125 MHz / 125 = 1 MHz)
    uint wrap = (125000000 / (clock_div * frequency)) - 1;  // Cálculo do período PWM

    level = (duty_cycle / 100.0) * wrap; // Calcula o duty cycle

    pwm_set_clkdiv(slice, clock_div);
    pwm_set_wrap(slice, wrap);
    pwm_set_gpio_level(gpio, level);  // Define o nível do PWM
    pwm_set_enabled(slice, true);  // Habilita o PWM
}

// Função para medir a distância usando o sensor ultrassônico
float measure_distance() {
    // Envia um pulso de 10µs para o pino de disparo (TRIG_PIN)
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    // Aguarda o sinal do pino de eco (ECHO_PIN) ficar alto
    while (gpio_get(ECHO_PIN) == 0);
    absolute_time_t start_time = get_absolute_time(); // Registra o tempo inicial

    // Aguarda o sinal do pino de eco ficar baixo
    while (gpio_get(ECHO_PIN) == 1);
    absolute_time_t end_time = get_absolute_time(); // Registra o tempo final

    // Calcula a diferença de tempo em microssegundos
    int64_t pulse_time = absolute_time_diff_us(start_time, end_time);

    // Calcula a distância em cm (Velocidade do som = 343 m/s = 0.0343 cm/µs)
    float distance = (pulse_time * 0.0343) / 2.0;

    return distance;
}

// Inicializa gpios
void setup_gpios() {

  // Inicialização dos pinos GPIO
  gpio_init(TRIG_PIN);
  gpio_init(ECHO_PIN);
  gpio_set_dir(TRIG_PIN, GPIO_OUT);
  gpio_set_dir(ECHO_PIN, GPIO_IN);

  // Inicializacao dos pinos dos LEDS
  gpio_init(LED_RED);
  gpio_init(LED_GREEN);
  gpio_init(LED_BLUE);

  gpio_set_dir(LED_RED,GPIO_OUT);
  gpio_set_dir(LED_GREEN, GPIO_OUT);
  gpio_set_dir(LED_BLUE, GPIO_OUT);

  // Inicialização dos botoes e seta eles como pull up
  gpio_init(button_a);
  gpio_init(button_b);
  gpio_set_dir(button_a, GPIO_IN);
  gpio_set_dir(button_b, GPIO_IN);
  gpio_pull_up(button_a);
  gpio_pull_up(button_b);

}


int main() {
    stdio_init_all();
    setup_gpios();  // Inicializa gpios
    
    // loop principal
    while (true) {

      // verifica se o botao b foi pressionado
      if (!gpio_get(button_b)) {
        working = false;
        setup_pwm(BUZZER_PIN, 0, 50);
        set_color(0,0,0);
      }

      // verifica se o botao a foi pressionado 
      if (!gpio_get(button_a)) {
        working = true;
      }
      
      // verifica se o sistema deve ligar ou desligar
      if (working) {
          float distance = measure_distance();
          printf("Distância: %.2f cm\n", distance);

          // Ajusta o buzzer e LED com base na distância medida
          if (distance < 5) {
            setup_pwm(BUZZER_PIN, 100, 50);
            set_color(1,0,0); // Vermelho
          } else if (distance >= 5 && distance < 10) {
            setup_pwm(BUZZER_PIN, 1000, 50);
            set_color(1,1,1); // Branco
          } else if (distance >= 10 && distance < 15) {
            setup_pwm(BUZZER_PIN, 2000, 50);
            set_color(0,0,1); // Azul
          } else if (distance >= 15 && distance < 20) {
            setup_pwm(BUZZER_PIN, 4000, 50);
            set_color(0,1,0); // Verde
          } else if (distance >= 20 && distance < 30) {
            setup_pwm(BUZZER_PIN, 6000, 50);
            set_color(1,0,1); // Rosa
          } else {
            setup_pwm(BUZZER_PIN, 0, 50);
            set_color(0,0,0); // Desliga LED e o buzzer
          }
      }
  }

  return 0;
}