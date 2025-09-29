#ifndef TEMPERATURE_H
#define TEMPERATURE_H

float temperature_voltage_read();

/**
 * @brief Lê a tensão do canal do ADC correspondente e a converte para graus Celsius.
 *
 * @return A temperatura calculada em graus Celsius.
 */
float temperature_read();

#endif // TEMPERATURE_H