#ifndef FLOW_H
#define FLOW_H

float flow_voltage_read();

/**
 * @brief Lê a tensão do canal do ADC correspondente e a converte para ????.
 *
 * @return A vazão calculada em ????.
 */
float flow_read();

#endif // FLOW_H