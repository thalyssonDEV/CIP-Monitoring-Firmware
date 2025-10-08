#include "sensor_manager.h"
#include <stdio.h>
#include "../analog_sensor/analog_sensor.h" 
#include "../adc_manager/adc_manager.h"

static float convert_linear_interpolation(float v, float max_v, float max_value, float min_value) {
    if (v < 0.0f) {
        return SENSOR_READ_ERROR;
    }
    if (max_v == 0.0f) {
        return min_value; // Evita divisão por zero
    }
    // Fórmula de interpolação linear completa:
    // y = y_min + (x - x_min) * (y_max - y_min) / (x_max - x_min)
    float value_range = max_value - min_value;
    return min_value + (v * (value_range / max_v));
}

static const analog_sensor_t temperature_sensor = {
    .adc_channel = ADS1115_MUX_SINGLE_0,
    .param1      = SENSOR_TEMPERATURE_MAX_VOLTAGE, 
    .param2      = SENSOR_TEMPERATURE_MAX_VALUE,  
    .param3      = SENSOR_TEMPERATURE_MIN_VALUE,   
    .convert     = convert_linear_interpolation
};

static const analog_sensor_t concentration_sensor = {
    .adc_channel = ADS1115_MUX_SINGLE_1,
    .param1      = SENSOR_CONCENTRATION_MAX_VOLTAGE, 
    .param2      = SENSOR_CONCENTRATION_MAX_VALUE,  
    .param3      = SENSOR_CONCENTRATION_MIN_VALUE,   
    .convert     = convert_linear_interpolation
};

static const analog_sensor_t flow_sensor = {
    .adc_channel = ADS1115_MUX_SINGLE_2,
    .param1      = SENSOR_FLOW_MAX_VOLTAGE, 
    .param2      = SENSOR_FLOW_MAX_VALUE,  
    .param3      = SENSOR_FLOW_MIN_VALUE,   
    .convert     = convert_linear_interpolation
};

/**
 * @brief Inicializa todos os módulos de hardware.
 *
 * Esta função verifica o resultado da inicialização do ADC. Se a
 * inicialização falhar, o erro é propagado para o chamador.
 *
 * @return 0 em caso de sucesso, 1 em caso de falha.
 */
int sensors_init(void) {
    printf("[INFO] Inicializando sensores...\n");

    // Verifica o status da inicialização do ADC.
    if (adc_module_init() != ADC_STATUS_OK) {
        printf("[ERRO FATAL] Falha ao inicializar o módulo ADC. Verifique o hardware.\n");
        return 1;
    }

    printf("[OK] Sensores inicializados com sucesso.\n");
    return 0;
}

/**
 * @brief Lê todos os sensores e preenche a estrutura de dados.
 *
 * Se uma leitura de tensão indicar um erro (retornando um valor negativo), a conversão
 * subsequente ainda é chamada (para propagar o erro), mas o sistema está
 * ciente da falha.
 *
 * @param reading Ponteiro para a estrutura que armazenará os dados lidos.
 * @return 0 em caso de sucesso, 1 se o ponteiro de entrada for nulo.
 */
int sensors_read_all(sensors_reading_t* reading) {
    // Validação do ponteiro de entrada.
    if (!reading) {
        printf("[ERRO] Ponteiro para leitura dos sensores é nulo.\n");
        return 1;
    }

    float temp_v, conc_v, flow_v;

    analog_sensor_read(&temperature_sensor, &temp_v, &reading->temperature_c);
    analog_sensor_read(&concentration_sensor, &conc_v, &reading->concentration_percent);
    analog_sensor_read(&flow_sensor, &flow_v, &reading->flow_liter);

    // O printf agora tem toda a informação de que precisa.
    printf("[DADOS] Temp: %.2f C (%.5f V) | Conc: %.2f %% (%.5f V) | Vazao: %.2f L/min (%.5f V)\n", 
           reading->temperature_c, 
           temp_v,
           reading->concentration_percent,
           conc_v,
           reading->flow_liter,
           flow_v
    );

    return 0;
}