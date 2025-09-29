#ifndef CONCENTRATION_H
#define CONCENTRATION_H

float concentration_voltage_read();

/**
 * @brief Lê a tensão do canal do ADC correspondente e a converte para porcentagem.
 *
 * @return A concentração calculada em porcentagem.
 */
float concentration_read();

#endif // CONCENTRATION_H