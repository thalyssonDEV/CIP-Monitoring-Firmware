#include "temperature.h"
#include "../adc_manager/adc_manager.h"

// Mapeia o sensor de temperatura para o canal 0 do ADC.
#define TEMPERATURE_ADC_CHANNEL ADS1115_MUX_SINGLE_0

static const float MAX_VOLTAGE = 3.3;
static const float MAX_TEMPERATURE = 200.0; // 0-200°C

float temperature_voltage_read() {
    return adc_module_read_voltage(TEMPERATURE_ADC_CHANNEL);
}

float temperature_read() {
    float volts = adc_module_read_voltage(TEMPERATURE_ADC_CHANNEL);
    return (volts / MAX_VOLTAGE) * MAX_TEMPERATURE;
}