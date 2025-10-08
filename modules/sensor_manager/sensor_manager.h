#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

/**
 * @brief Valor sentinela para indicar falha na leitura do sensor.
 *
 * Utilizado para garantir que erros de leitura sejam inequivocamente
 * distinguíveis de leituras válidas (que, por definição, não são negativas).
 */
#define SENSOR_READ_ERROR -1.0f

/**
 * @brief Estrutura para dados consolidados dos sensores
 */
typedef struct {
    float temperature_c;
    float concentration_percent;
    float flow_liter;
} sensors_reading_t;

/**
 * @brief Inicializa todos os sensores
 * @return 0 em caso de sucesso, 1 em caso de erro
 */
int sensors_init(void);

/**
 * @brief Lê dados de todos os sensores
 * @param reading Estrutura onde serão armazenados os dados lidos
 * @return 0 em caso de sucesso, 1 em caso de erro
 */
int sensors_read_all(sensors_reading_t* reading);

#endif // SENSOR_MANAGER_H
