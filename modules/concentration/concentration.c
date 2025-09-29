#include "concentration.h"
#include "../adc_manager/adc_manager.h"

// Mapeia o sensor de temperatura para o canal 1 do ADC.
#define CONCENTRATION_ADC_CHANNEL ADS1115_MUX_SINGLE_1

static const float MAX_VOLTAGE = 3.3;
static const float MAX_CONCENTRATION = 100; // 0-100%

float concentration_voltage_read() {
    return adc_module_read_voltage(CONCENTRATION_ADC_CHANNEL);
}

float concentration_read() {
    float volts = adc_module_read_voltage(CONCENTRATION_ADC_CHANNEL);
    return (volts / MAX_VOLTAGE) * MAX_CONCENTRATION;
}