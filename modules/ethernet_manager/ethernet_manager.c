#include "ethernet_manager.h"
#include "w5500_config.h"
#include "socket.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include <string.h>
#include <stdio.h>

// Configuração dos pinos SPI para W5500
#define SPI_PORT spi0
#define PIN_MISO 16
#define PIN_CS   17
#define PIN_SCK  18
#define PIN_MOSI 19 
#define PIN_RST  20

// Variáveis globais
static ethernet_config_t current_config;
static ethernet_status_t current_status = ETHERNET_DISCONNECTED;

// Função de reset do W5500
static void wizchip_reset(void) {
    gpio_put(PIN_RST, 0);
    sleep_ms(10);
    gpio_put(PIN_RST, 1);
    sleep_ms(150);
}

static int init_spi_and_pins(void) {
    printf("[INFO] Inicializando SPI e pinos do W5500...\n");
    
    // Inicializa SPI a 50MHz
    spi_init(SPI_PORT, 50 * 1000 * 1000);
    
    // Configura pinos SPI
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Configura pino CS
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    
    // Configura pino RST
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    gpio_put(PIN_RST, 1);
    
    // Reset do W5500
    wizchip_reset();
    
    return 0;
}

int ethernet_init(ethernet_config_t* config) {
    if (!config) {
        printf("[ERRO] Configuração de rede inválida\n");
        return -1;
    }
    
    printf("[INFO] Inicializando módulo Ethernet...\n");
    current_status = ETHERNET_CONNECTING;
    
    // Copia configuração
    memcpy(&current_config, config, sizeof(ethernet_config_t));
    
    // Inicializa SPI e pinos
    if (init_spi_and_pins() != 0) {
        printf("[ERRO] Falha na inicialização do SPI\n");
        current_status = ETHERNET_ERROR;
        return -1;
    }
    
    // Registra callbacks do SPI
    reg_wizchip_cs_cbfunc(w5500_cs_select, w5500_cs_deselect);
    reg_wizchip_spi_cbfunc(w5500_spi_readbyte, w5500_spi_writebyte);
    reg_wizchip_spiburst_cbfunc(w5500_spi_readburst, w5500_spi_writeburst);
    
    // Inicializa buffers de socket (8 sockets de 2KB cada)
    uint8_t tx_size[] = {2, 2, 2, 2, 2, 2, 2, 2};
    uint8_t rx_size[] = {2, 2, 2, 2, 2, 2, 2, 2};
    
    if (wizchip_init(tx_size, rx_size) != 0) {
        printf("[ERRO] Falha na inicialização do WizChip\n");
        current_status = ETHERNET_ERROR;
        return -1;
    }
    
    // Configura informações de rede
    wiz_NetInfo net_info;
    memcpy(net_info.mac, config->mac, 6);
    memcpy(net_info.ip, config->ip, 4);
    memcpy(net_info.sn, config->subnet, 4);
    memcpy(net_info.gw, config->gateway, 4);
    memcpy(net_info.dns, config->dns, 4);
    net_info.dhcp = config->dhcp;
    
    wizchip_setnetinfo(&net_info);
    
    // Verifica se a configuração foi aplicada
    wiz_NetInfo check_info;
    wizchip_getnetinfo(&check_info);
    
    printf("[OK]   W5500 inicializado com sucesso!\n");
    printf("[INFO] MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", 
           check_info.mac[0], check_info.mac[1], check_info.mac[2],
           check_info.mac[3], check_info.mac[4], check_info.mac[5]);
    printf("[INFO] IP: %d.%d.%d.%d\n", 
           check_info.ip[0], check_info.ip[1], check_info.ip[2], check_info.ip[3]);
    printf("[INFO] Gateway: %d.%d.%d.%d\n", 
           check_info.gw[0], check_info.gw[1], check_info.gw[2], check_info.gw[3]);


    printf("[INFO] Aguardando link físico...\n");
    int retries = 5; // Tenta por 2.5 segundos
    while (retries > 0 && wizphy_getphylink() != PHY_LINK_ON) {
        sleep_ms(500);
        retries--;
    }

    if (wizphy_getphylink() != PHY_LINK_ON) {
        printf("[ERRO] Link Ethernet não detectado após a inicialização. Verifique o cabo.\n");
        current_status = ETHERNET_DISCONNECTED;
        return -1; // Sinaliza a falha
    }
    
    printf("[OK] Link físico detectado.\n");
    current_status = ETHERNET_CONNECTED;
    return 0;
}

ethernet_status_t ethernet_get_status(void) {
    // Verifica se há link físico
    uint8_t phy_status = wizphy_getphylink();
    
    if (phy_status == PHY_LINK_OFF) {
        if (current_status == ETHERNET_CONNECTED) {
            printf("[INFO] Link Ethernet desconectado\n");
            current_status = ETHERNET_DISCONNECTED;
        }
    } else if (phy_status == PHY_LINK_ON) {
        if (current_status == ETHERNET_DISCONNECTED) {
            printf("[INFO] Link Ethernet conectado\n");
            current_status = ETHERNET_CONNECTED;
        }
    }
    
    return current_status;
}

void ethernet_get_network_info(wiz_NetInfo* net_info) {
    if (net_info) {
        wizchip_getnetinfo(net_info);
    }
}

void ethernet_cleanup(void) {
    printf("[INFO] Limpando recursos do módulo Ethernet...\n");
    current_status = ETHERNET_DISCONNECTED;
}

int ethernet_restart(void) {
    printf("[INFO] Reiniciando conexão Ethernet...\n");
    
    // Reset do W5500
    wizchip_reset();
    
    // Reaplica configuração
    return ethernet_init(&current_config);
}