/**
 * @file analog_sensor.h
 * @brief Interface pública para um módulo de sensor analógico genérico.
 *
 * Este módulo abstrai a lógica de leitura e conversão de qualquer sensor
 * analógico que esteja conectado a um ADC. Ao desacoplar o hardware
 * (canal do ADC) e a lógica de calibração (fórmula de conversão) da
 * sua utilização, ele elimina a duplicação de código e promove a
 * reutilização, aderindo aos princípios de design DRY (Don't Repeat Yourself).
 */
#ifndef ANALOG_SENSOR_H
#define ANALOG_SENSOR_H

#include "../adc_manager/adc_manager.h"

/**
 * @brief Ponteiro de função para uma fórmula de conversão de sensor.
 *
 * Define um contrato para qualquer função que converta uma tensão lida
 * numa unidade de engenharia final (ex: graus Celsius, porcentagem).
 *
 * @param voltage A tensão de entrada proveniente do ADC.
 * @param p1 O primeiro parâmetro de calibração/conversão.
 * @param p2 O segundo parâmetro de calibração/conversão.
 * @param p3 O terceiro parâmetro de calibração/conversão.
 * @return O valor final convertido na unidade de engenharia.
 */
typedef float (*sensor_conversion_fn)(float voltage, float p1, float p2, float p3);

/**
 * @struct analog_sensor_t
 * @brief Estrutura de definição para um sensor analógico genérico.
 *
 * Encapsula todas as informações necessárias para definir e ler um sensor:
 * o seu mapeamento de hardware, os seus parâmetros de calibração e a
 * lógica de software para converter a sua leitura.
 */
typedef struct {
    /** @brief O canal do multiplexador do ADC ao qual o sensor está fisicamente conectado. */
    enum ads1115_mux_t adc_channel;

    /** @brief Primeiro parâmetro de calibração (ex: tensão máxima, offset). */
    float param1;

    /** @brief Segundo parâmetro de calibração (ex: valor máximo, fator de escala). */
    float param2;

    /** @brief Terceiro parâmetro de calibração (ex: valor mínimo, fator de escala). */
    float param3;

    /** @brief Ponteiro para a função que implementa a fórmula de conversão específica deste sensor. */
    sensor_conversion_fn convert;
} analog_sensor_t;

/**
 * @brief Lê a tensão de um sensor e retorna o valor final convertido.
 *
 * Função principal do módulo. Ela orquestra o processo de leitura:
 * 1. Interage com a camada de ADC para obter uma leitura de tensão bruta.
 * 2. Verifica se a leitura de hardware foi bem-sucedida.
 * 3. Se bem-sucedida, invoca a função de conversão específica do sensor
 * para calcular o valor final.
 * 4. Propaga os erros de forma inequívoca.
 *
 * @param sensor Um ponteiro constante para a estrutura de definição do sensor.
 * @param voltage_out Ponteiro para uma variável float que receberá a tensão lida.
 * @param final_value_out Ponteiro para uma variável float que receberá o valor final convertido.
 * @return O valor de engenharia final (ex: 25.5 °C), ou um valor de erro
 * em caso de falha de leitura no ADC.
 */
int analog_sensor_read(const analog_sensor_t* sensor, float* voltage_out, float* final_value_out);

#endif // ANALOG_SENSOR_H