#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Envia dados dos sensores para o servidor via HTTP POST usando sockets TCP
 * @param data Estrutura contendo os dados dos sensores
 * @return 0 em caso de sucesso, negativo em caso de erro
 */

int http_send_sensor_data(float temperature, float concentration, float flow);

/**
 * @brief Converte string IP para array de bytes
 * @param ip_str String IP (ex: "192.168.1.100")
 * @param ip_bytes Array de 4 bytes para receber o IP
 * @return 0 em caso de sucesso, -1 em caso de erro
 */
int http_parse_ip_string(const char* ip_str, uint8_t* ip_bytes);

#endif // HTTP_CLIENT_Hzz