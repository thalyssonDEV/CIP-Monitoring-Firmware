#include "http_client.h"
#include "socket.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HTTP_REQUEST_BUF_SIZE 512
#define HTTP_RESPONSE_BUF_SIZE 512
#define HTTP_TIMEOUT_MS 10000

// Buffers estáticos para requisição e resposta
static uint8_t http_request_buf[HTTP_REQUEST_BUF_SIZE];
static uint8_t http_response_buf[HTTP_RESPONSE_BUF_SIZE];

int http_parse_ip_string(const char* ip_str, uint8_t* ip_bytes) {
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

int http_send_sensor_data(float temperature, float concentration, float flow) {
    // Configurações do servidor (definidas em secrets.cmake)
    uint8_t dest_ip[4];
    if (http_parse_ip_string(TARGET_SERVER_IP, dest_ip) != 0) {
        printf("[ERRO] IP do servidor inválido: %s\n", TARGET_SERVER_IP);
        return -1;
    }
    
    uint16_t dest_port = TARGET_PORT;
    const char* uri = TARGET_PATH;
    uint8_t socket_num = 0;  // Usar socket 0
    
    printf("[INFO] Tentando conectar ao servidor %d.%d.%d.%d:%d...\n", 
           dest_ip[0], dest_ip[1], dest_ip[2], dest_ip[3], dest_port);
    
    // Criar socket TCP
    int32_t ret = socket(socket_num, Sn_MR_TCP, 0, 0);
    if (ret != socket_num) {
        printf("[ERRO] Falha ao criar socket TCP. Código: %ld\n", ret);
        return ret;
    }
    
    // Conectar ao servidor
    ret = connect(socket_num, dest_ip, dest_port);
    if (ret != SOCK_OK) {
        printf("[ERRO] Falha ao conectar ao servidor. Código: %ld\n", ret);
        close(socket_num);
        return ret;
    }
    
    printf("[OK]  Conexão TCP estabelecida.\n");
    
    // Preparar JSON payload
    char json_payload[512];
    int json_len = snprintf(json_payload, sizeof(json_payload),
        "{\"temperature\":%.2f,\"concentration\":%.2f, \"flow\":%.2f}",
        temperature, concentration, flow);
    
    if (json_len >= sizeof(json_payload)) {
        printf("[ERRO] JSON payload muito grande\n");
        close(socket_num);
        return -1;
    }
    
    printf("[DADOS] Preparando o seguinte JSON para envio:\n");
    printf("       -> %s\n", json_payload);
    
    // Montar requisição HTTP completa (cabeçalhos + corpo JSON)
    int header_len = snprintf((char*)http_request_buf, HTTP_REQUEST_BUF_SIZE,
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        uri,
        TARGET_SERVER_IP, // Usar o IP como string para o Host
        json_len,
        json_payload);

    if (header_len >= HTTP_REQUEST_BUF_SIZE) {
        printf("[ERRO] Requisição HTTP muito grande\n");
        close(socket_num);
        return -1;
    }
    
    printf("[INFO] Enviando requisição POST...\n");
    
    // Enviar requisição
    ret = send(socket_num, http_request_buf, header_len);
    if (ret < 0) {
        printf("[ERRO] Falha ao enviar requisição HTTP. Código: %ld\n", ret);
        close(socket_num);
        return ret;
    }
    
    printf("[OK]   Requisição enviada (%ld bytes). Aguardando resposta...\n", ret);
    
    // Aguardar e ler resposta
    uint32_t timeout = 0;
    while (getSn_RX_RSR(socket_num) == 0 && timeout < HTTP_TIMEOUT_MS) {
        sleep_ms(10);
        timeout += 10;
    }
    
    if (timeout >= HTTP_TIMEOUT_MS) {
        printf("[AVISO] Timeout na resposta do servidor\n");
    } else {
        int32_t response_len = recv(socket_num, http_response_buf, HTTP_RESPONSE_BUF_SIZE - 1);
        if (response_len > 0) {
            http_response_buf[response_len] = '\0';
            printf("[INFO] Resposta do servidor (%ld bytes):\n", response_len);
            printf("       %s\n", (char*)http_response_buf);
        }
    }
    
    // Fechar conexão
    disconnect(socket_num);
    close(socket_num);
    
    printf("[INFO] Conexão fechada.\n");
    printf("[INFO] Ciclo de envio concluído. Liberando recursos.\n");
    
    return json_len;
}