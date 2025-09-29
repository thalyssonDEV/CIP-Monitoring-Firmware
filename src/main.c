#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"

#include "hardware/watchdog.h"

#include "modules/ethernet_manager/ethernet_manager.h"
#include "modules/http_client/http_client.h"
#include "modules/sensor_manager/sensor_manager.h"

#define CYCLE_INTERVAL_MS 1000
#define WATCHDOG_TIMEOUT_MS 5000

/**
 * @brief Tarefa principal que encapsula a lógica de leitura e envio.
 *
 * Esta tarefa executa um ciclo de leitura de sensores e envio de dados
 * com uma frequência fixa, controlada pelo FreeRTOS.
 */
void main_task(__unused void *params) {
    // 1. Inicialização dos módulos
    ethernet_config_t eth_config = {
        .mac = {ETHERNET_MAC_0, ETHERNET_MAC_1, ETHERNET_MAC_2, 
                ETHERNET_MAC_3, ETHERNET_MAC_4, ETHERNET_MAC_5},
        .ip = {DEVICE_IP_0, DEVICE_IP_1, DEVICE_IP_2, DEVICE_IP_3},
        .subnet = {SUBNET_MASK_0, SUBNET_MASK_1, SUBNET_MASK_2, SUBNET_MASK_3},
        .gateway = {GATEWAY_IP_0, GATEWAY_IP_1, GATEWAY_IP_2, GATEWAY_IP_3},
        .dns = {8, 8, 8, 8},
        .dhcp = NETINFO_STATIC
    };

    if (ethernet_init(&eth_config) != 0) {
        printf("[ERRO] Falha na inicialização do Ethernet. Tarefa Interrompida.\n");
        vTaskDelete(NULL); // Encerra esta tarefa
    }

    if (sensors_init() != 0) {
        printf("[ERRO] Falha na inicializacao dos sensores. Tarefa Interrompida.\n");
        vTaskDelete(NULL); // Encerra esta tarefa
    }

    printf("[INFO] Iniciando ciclos de envio a cada %d segundos.\n", CYCLE_INTERVAL_MS / 1000);

    sensors_reading_t sensor_data;
    TickType_t xLastWakeTime = xTaskGetTickCount();

    // 2. Loop principal da tarefa
    while (1) {
        watchdog_update();

        // Verifica status da conexão Ethernet
        if (ethernet_get_status() != ETHERNET_CONNECTED) {
            printf("[AVISO] Ethernet desconectado. Tentando reconectar...\n");
            if (ethernet_restart() != 0) {
                printf("[ERRO] Falha na reconexão Ethernet. Aguardando...\n");
                // Aguarda um ciclo antes de tentar novamente
                vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CYCLE_INTERVAL_MS));
                continue; // Pula para a próxima iteração do ciclo
            }
        }

        // Lê os dados dos sensores
        if (sensors_read_all(&sensor_data) != 0) {
            printf("[ERRO] Falha na leitura dos sensores. Tentando novamente no próximo ciclo...\n");
            vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CYCLE_INTERVAL_MS));
            continue;
        }
        
        // Envia os dados via HTTP
        int result = http_send_sensor_data(
            sensor_data.temperature_c,
            sensor_data.concentration_percent,
            sensor_data.flow_liter
        );

        if (result < 0) {
            printf("[ERRO] Falha no envio dos dados.\n");
        }
        
        printf("\nAguardando %d segundo(s) para o proximo envio...\n", CYCLE_INTERVAL_MS / 1000);

        // Pausa a tarefa até que o próximo intervalo de 1 segundo seja atingido
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CYCLE_INTERVAL_MS));
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);

    watchdog_enable(WATCHDOG_TIMEOUT_MS, 1);
    printf("[INFO] Watchdog habilitado com timeout de 5000ms.\n");

    // Cria a tarefa principal que executará a lógica do dispositivo.
    xTaskCreate(main_task, "MainTask", 1024, NULL, 1, NULL);

    // Inicia o escalonador do FreeRTOS.
    vTaskStartScheduler();

    while(1);
    return 0;
}