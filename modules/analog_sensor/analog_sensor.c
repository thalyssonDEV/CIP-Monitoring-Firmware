/**
 * @file analog_sensor.c
 * @brief Implementação do driver de sensor analógico genérico.
 *
 * Fornece a lógica de baixo nível para interagir com o módulo ADC e aplicar
 * as fórmulas de conversão definidas externamente.
 */

#include "analog_sensor.h"
#include "../sensor_manager/sensor_manager.h"
#include <stddef.h>

/**
 * @brief Implementação da leitura e conversão de um sensor analógico.
 */
int analog_sensor_read(const analog_sensor_t* sensor, float* voltage_out, float* final_value_out) {
    // Verificação de ponteiros Nulos
    if (sensor == NULL || sensor->convert == NULL || voltage_out == NULL || final_value_out == NULL) {
        return SENSOR_READ_ERROR;
    }
    
    // Interage com a camada de hardware (ADC) para obter a tensão.
    adc_status_t status = adc_module_read_voltage(sensor->adc_channel, voltage_out);

    // Valida o resultado da operação de hardware.
    if (status != ADC_STATUS_OK) {
        // Se a leitura da tensão falhar, propaga o erro imediatamente,
        // sem tentar a conversão.
        *voltage_out = SENSOR_READ_ERROR;
        *final_value_out = SENSOR_READ_ERROR;

        return SENSOR_READ_ERROR;
    }
    
    // Se a leitura da tensão for bem-sucedida, calculamos o valor final
    *final_value_out = sensor->convert(*voltage_out, sensor->param1, sensor->param2, sensor->param3);
    
    return 0;
}