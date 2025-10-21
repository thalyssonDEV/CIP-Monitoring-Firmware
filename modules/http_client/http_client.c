#include "http_client.h"
#include "ethernet_manager.h" 
#include "socket.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HTTP_REQUEST_BUF_SIZE 512
#define HTTP_RESPONSE_BUF_SIZE 512

static bool is_network_ready() {
    if (ethernet_get_status() == ETHERNET_CONNECTED) {
        return true;
    }

    printf("[AVISO] Ethernet desconectado. Tentando reconectar...\n");
    if (ethernet_restart() != 0) {
        printf("[ERRO] Falha na reconexão Ethernet.\n");
        return false;
    }

    // Se o restart foi bem-sucedido, o init interno já validou o link
    printf("[OK] Ethernet reconectado com sucesso.\n");
    return true;
}

/**
 * @brief Converte uma string de IP no formato "X.X.X.X" para um array de bytes.
 * @return 0 em sucesso, -1 em erro.
 */
static int http_parse_ip_string(const char* ip_str, uint8_t* ip_bytes) {
    if (!ip_str || !ip_bytes) {
        return -1;
    }
    int ip1, ip2, ip3, ip4;
    if (sscanf(ip_str, "%d.%d.%d.%d", &ip1, &ip2, &ip3, &ip4) != 4) {
        return -1;
    }
    if (ip1 < 0 || ip1 > 255 || ip2 < 0 || ip2 > 255 || 
        ip3 < 0 || ip3 > 255 || ip4 < 0 || ip4 > 255) {
        return -1;
    }
    ip_bytes[0] = (uint8_t)ip1;
    ip_bytes[1] = (uint8_t)ip2;
    ip_bytes[2] = (uint8_t)ip3;
    ip_bytes[3] = (uint8_t)ip4;
    return 0;
}

http_status_t http_send_sensor_data(float temperature, float concentration, float flow) {

    // Buffers estáticos para requisição e resposta
    static uint8_t http_request_buf[HTTP_REQUEST_BUF_SIZE];
    static uint8_t http_response_buf[HTTP_RESPONSE_BUF_SIZE];

    // Verificação de pré-condição: a rede está pronta?
    if (!is_network_ready()) {
        return HTTP_ERROR_CONNECT_FAILED; // Retorna um erro
    }

    uint8_t dest_ip[4];
    if (http_parse_ip_string(TARGET_SERVER_IP, dest_ip) != 0) {
        printf("[ERRO] IP do servidor inválido: %s\n", TARGET_SERVER_IP);
        return HTTP_ERROR_INVALID_IP;
    }
    
    uint16_t dest_port = TARGET_PORT;
    const char* uri = TARGET_PATH;
    uint8_t socket_num = 0;

    printf("[INFO] Tentando conectar ao servidor %d.%d.%d.%d:%d...\n", 
           dest_ip[0], dest_ip[1], dest_ip[2], dest_ip[3], dest_port);
    
    // 1. Criar socket TCP
    if (socket(socket_num, Sn_MR_TCP, 0, 0) != socket_num) {
        printf("[ERRO] Falha ao criar socket TCP.\n");
        return HTTP_ERROR_SOCKET_CREATION;
    }
    
    // 2. Conectar ao servidor
    if (connect(socket_num, dest_ip, dest_port) != SOCK_OK) {
        printf("[ERRO] Falha ao conectar ao servidor.\n");
        close(socket_num);
        return HTTP_ERROR_CONNECT_FAILED;
    }
    printf("[OK] Conexão TCP estabelecida.\n");
    
    // 3. Preparar JSON payload
    char json_payload[256];
    int json_len = snprintf(json_payload, sizeof(json_payload),
        "{\"temperature\":%.2f,\"concentration\":%.2f,\"flow\":%.2f}",
        temperature, concentration, flow);
    
    if (json_len >= sizeof(json_payload)) {
        printf("[ERRO] JSON payload muito grande\n");
        disconnect(socket_num);
        close(socket_num);
        return HTTP_ERROR_REQUEST_TOO_LARGE;
    }
    printf("[DADOS] Enviando JSON: %s\n", json_payload);
    
    // 4. Montar requisição HTTP completa
    int request_len = snprintf((char*)http_request_buf, HTTP_REQUEST_BUF_SIZE,
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Authorization: Bearer %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        uri, TARGET_SERVER_IP, BEARER_TOKEN, json_len, json_payload);

    if (request_len >= HTTP_REQUEST_BUF_SIZE) {
        printf("[ERRO] Requisição HTTP muito grande\n");
        disconnect(socket_num);
        close(socket_num);
        return HTTP_ERROR_REQUEST_TOO_LARGE;
    }
    
    // 5. Enviar requisição
    if (send(socket_num, http_request_buf, request_len) < 0) {
        printf("[ERRO] Falha ao enviar requisição HTTP.\n");
        disconnect(socket_num);
        close(socket_num);
        return HTTP_ERROR_SEND_FAILED;
    }
    printf("[OK] Requisição enviada. Aguardando resposta...\n");
    
    // 6. Aguardar e ler resposta com timeout
    uint32_t timeout = 0;
    while (getSn_RX_RSR(socket_num) == 0 && timeout < HTTP_TIMEOUT_MS) {
        sleep_ms(10);
        timeout += 10;
    }
    
    if (timeout >= HTTP_TIMEOUT_MS) {
        printf("[AVISO] Timeout na resposta do servidor\n");
        disconnect(socket_num);
        close(socket_num);
        return HTTP_ERROR_TIMEOUT;
    }
    
    int32_t response_len = recv(socket_num, http_response_buf, HTTP_RESPONSE_BUF_SIZE - 1);
    if (response_len <= 0) {
        printf("[ERRO] Falha ao receber dados do servidor. Código: %ld\n", response_len);
        disconnect(socket_num);
        close(socket_num);
        return HTTP_ERROR_RECV_FAILED;
    }
    
    http_response_buf[response_len] = '\0';
    printf("[INFO] Resposta do servidor:\n%s\n", (char*)http_response_buf);

    // 7. Analisar o código de estado HTTP
    if (strstr((char*)http_response_buf, "HTTP/1.1 200 OK") == NULL &&
        strstr((char*)http_response_buf, "HTTP/1.1 201 Created") == NULL) {
        printf("[ERRO] Servidor respondeu com um código de estado de erro.\n");
        disconnect(socket_num);
        close(socket_num);
        return HTTP_ERROR_SERVER_REJECTED;
    }

    // 8. Fechar conexão
    disconnect(socket_num);
    close(socket_num);
    
    printf("[OK] Ciclo de envio concluído com sucesso.\n");
    return HTTP_OK;
}