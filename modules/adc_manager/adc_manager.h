#ifndef ADC_MANAGER_H
#define ADC_MANAGER_H

#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ads1115.h"

/**
 * @brief Inicializa o barramento I2C e o conversor ADC ADS1115.
 */
void adc_module_init();

/**
 * @brief Lê a tensão de um canal específico do ADS1115.
 * * @param channel O canal do multiplexador a ser lido (ex: ADS1115_MUX_SINGLE_0).
 * @return O valor da tensão lida em Volts. Retorna 0.0f em caso de erro.
 */
float adc_module_read_voltage(enum ads1115_mux_t channel);

#endif // ADC_MANAGER_H