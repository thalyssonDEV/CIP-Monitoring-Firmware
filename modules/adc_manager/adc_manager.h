/**
 * @file adc_manager.h
 * @brief Interface pública para um driver do ADC ADS1115.
 *
 * Este ficheiro define o "contrato" público para interagir com o módulo ADC.
 * A sua principal característica é o uso de um tipo de enumeração (adc_status_t)
 * para retornar o estado explícito de cada operação.
 */
#ifndef ADC_MANAGER_H
#define ADC_MANAGER_H

#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ads1115.h"

/**
 * @enum adc_status_t
 * @brief Define os possíveis códigos de estado retornados pelas funções do módulo ADC.
 */
typedef enum {
    ADC_STATUS_OK,                  /**< A operação foi concluída com sucesso. */
    ADC_STATUS_NOT_INITIALIZED,     /**< A operação falhou porque o módulo não foi inicializado. */
    ADC_STATUS_INIT_FAILED,         /**< A inicialização falhou, provável falha de comunicação com o hardware. */
    ADC_STATUS_READ_FAILED,         /**< A leitura falhou, provável perda de comunicação em tempo de execução. */
    ADC_STATUS_INVALID_PARAM        /**< A operação falhou devido a um parâmetro inválido (ex: ponteiro nulo). */
} adc_status_t;

/**
 * @brief Inicializa o barramento I2C e o conversor ADC ADS1115.
 *
 * Realiza a configuração do hardware e, crucialmente, verifica se o dispositivo
 * ADC está presente e a comunicar no barramento I2C antes de sinalizar sucesso.
 * Esta função deve ser chamada com sucesso antes de qualquer outra função do módulo.
 *
 * @return ADC_STATUS_OK em caso de sucesso, ou um código de erro relevante em caso de falha.
 */
adc_status_t adc_module_init(void);

/**
 * @brief Lê a tensão de um canal específico do ADS1115.
 *
 * Esta função encapsula a seleção do canal, a leitura do valor bruto e a
 * conversão para Volts. Inclui uma verificação de saúde em tempo de execução
 * para garantir que a comunicação com o ADC ainda está ativa.
 *
 * @param channel O canal do multiplexador a ser lido (ex: ADS1115_MUX_SINGLE_0).
 * @param voltage_out Ponteiro para uma variável float onde o valor da tensão
 * lida será armazenado. O ponteiro não deve ser nulo.
 * @return ADC_STATUS_OK se a leitura for bem-sucedida, ou um código de erro
 * relevante em caso de falha.
 */
adc_status_t adc_module_read_voltage(enum ads1115_mux_t channel, float *voltage_out);

#endif // ADC_MANAGER_H