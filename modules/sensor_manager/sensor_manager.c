#include "sensor_manager.h"
#include <stdio.h>
#include "../temperature/temperature.h"
#include "../concentration/concentration.h"
#include "../flow/flow.h"
#include "../adc_manager/adc_manager.h"

int sensors_init(void) {
    printf("[INFO] Inicializando sensores...\n");

    // Inicializa o módulo ADC ADS1115
    adc_module_init();

    printf("[OK] Sensores inicializados com sucesso.\n");
    return 0;
}

int sensors_read_all(sensors_reading_t* reading) {
    if (!reading) {
        printf("[ERRO] Ponteiro para leitura dos sensores é nulo.\n");
        return 1;
    }
    
    // Realiza a leitura das voltagens e calcula os parâmetros
    reading->temperature_c = temperature_read();
    float temperature_volts = temperature_voltage_read();

    reading->concentration_percent = concentration_read();
    float concentration_volts = concentration_voltage_read();

    reading->flow_liter = flow_read();
    float flow_volts = flow_voltage_read();
    
    printf("[DADOS] Leitura dos sensores -> Temp: %.2f °C, Temp_Volts: %5f V  || Conc: %.2f Percent | Conc_Volts: %5f V || Vaz: %.2f | Vaz_Volts: %5f V\n", 
           reading->temperature_c, 
           temperature_volts,

           reading->concentration_percent,
           concentration_volts,

           reading->flow_liter,
           flow_volts
        );
    return 0;
}