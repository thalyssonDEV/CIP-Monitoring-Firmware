#include "adc_manager.h"
#include <stdio.h>

#define I2C_PORT i2c0
#define I2C_FREQ 400000
#define ADS1115_I2C_ADDR 0x48
const uint8_t SDA_PIN = 0;
const uint8_t SCL_PIN = 1;

static struct ads1115_adc adc;
static bool is_initialized = false;

void adc_module_init() {
    if (is_initialized) {
        return; 
    }

    i2c_init(I2C_PORT, I2C_FREQ);
    gpio_set_function(SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PIN);
    gpio_pull_up(SCL_PIN);

    ads1115_init(I2C_PORT, ADS1115_I2C_ADDR, &adc);
    
    ads1115_set_pga(ADS1115_PGA_4_096, &adc);
    ads1115_set_data_rate(ADS1115_RATE_128_SPS, &adc);

    is_initialized = true;
    printf("[OK] Modulo ADC (ADS1115) inicializado.\n");
}

float adc_module_read_voltage(enum ads1115_mux_t channel) {
    if (!is_initialized) {
        printf("[ERRO] Modulo ADC nao inicializado. Retornando valor nulo.\n");
        return 0.0f;
    }

    uint16_t adc_value;
    
    // Configura o canal de entrada para a leitura atual
    ads1115_set_input_mux(channel, &adc);
    ads1115_write_config(&adc); // Aplica a mudança de canal

    sleep_ms(10); 

    // Lê o canal adc correspondente
    ads1115_read_adc(&adc_value, &adc);
    
    float volts = ads1115_raw_to_volts(adc_value, &adc);

    // Retorna a voltagem lida no canal
    return (volts < 0) ? 0 : volts;
}