/**
 * @file http_client.h
 * @brief Interface pública para o cliente HTTP.
 *
 * Define o contrato para enviar dados de sensores para um servidor.
 */
#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdint.h>

/**
 * @enum http_status_t
 * @brief Define os possíveis códigos de estado para as operações do cliente HTTP.
 */
typedef enum {
    HTTP_OK,                    /**< A requisição foi enviada e o servidor confirmou o sucesso (HTTP 2xx). */
    HTTP_ERROR_INVALID_IP,      /**< O endereço IP do servidor fornecido é inválido. */
    HTTP_ERROR_SOCKET_CREATION, /**< Falha ao alocar um socket para a comunicação. */
    HTTP_ERROR_CONNECT_FAILED,  /**< Falha ao estabelecer a conexão TCP com o servidor. */
    HTTP_ERROR_REQUEST_TOO_LARGE,/**< O payload da requisição excedeu o tamanho do buffer. */
    HTTP_ERROR_SEND_FAILED,     /**< Ocorreu um erro durante o envio dos dados pela rede. */
    HTTP_ERROR_TIMEOUT,         /**< O servidor não respondeu dentro do tempo limite esperado. */
    HTTP_ERROR_SERVER_REJECTED, /**< O servidor respondeu com um código de erro HTTP (ex: 4xx, 5xx). */
    HTTP_ERROR_RECV_FAILED      /**< Falha ao receber dados do servidor após o envio. */
} http_status_t;

/**
 * @brief Envia os dados dos sensores para o servidor configurado via HTTP POST.
 *
 * Esta função encapsula todo o ciclo de vida de uma requisição HTTP:
 * 1. Criação do socket TCP.
 * 2. Conexão com o servidor.
 * 3. Formatação do payload JSON e dos cabeçalhos HTTP.
 * 4. Envio da requisição.
 * 5. Espera e validação da resposta do servidor.
 * 6. Encerramento da conexão.
 *
 * @param temperature O valor da temperatura a ser enviado.
 * @param conducitivity O valor da concentração a ser enviado.
 * @param flow O valor da vazão a ser enviado.
 * @return Um status `http_status_t` indicando o resultado da operação.
 */
http_status_t http_send_sensor_data(float temperature, float conductivity, float flow);

#endif // HTTP_CLIENT_H