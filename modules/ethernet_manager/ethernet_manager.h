#ifndef ETHERNET_MANAGER_H
#define ETHERNET_MANAGER_H

#include <stdint.h>
#include "wizchip_conf.h"

// Estrutura para configuração de rede
typedef struct {
    uint8_t mac[6];       // Endereço MAC
    uint8_t ip[4];        // IP do dispositivo
    uint8_t subnet[4];    // Máscara de sub-rede
    uint8_t gateway[4];   // Gateway
    uint8_t dns[4];       // Servidor DNS
    dhcp_mode dhcp;       // Modo DHCP (DHCP ou IP estático)
} ethernet_config_t;

// Status da conexão Ethernet
typedef enum {
    ETHERNET_DISCONNECTED,
    ETHERNET_CONNECTING,
    ETHERNET_CONNECTED,
    ETHERNET_ERROR
} ethernet_status_t;

/**
 * @brief Inicializa o módulo Ethernet com W5500
 * @param config Configuração de rede
 * @return 0 se sucesso, -1 se erro
 */
int ethernet_init(ethernet_config_t* config);

/**
 * @brief Verifica o status da conexão Ethernet
 * @return Status atual da conexão
 */
ethernet_status_t ethernet_get_status(void);

/**
 * @brief Obtém informações da rede atual
 * @param net_info Ponteiro para estrutura que receberá as informações
 */
void ethernet_get_network_info(wiz_NetInfo* net_info);

/**
 * @brief Limpa recursos do módulo Ethernet
 */
void ethernet_cleanup(void);

/**
 * @brief Reinicia a conexão Ethernet
 * @return 0 se sucesso, -1 se erro
 */
int ethernet_restart(void);

#endif // ETHERNET_MANAGER_H