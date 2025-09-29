#include "flow.h"
#include "../adc_manager/adc_manager.h"

// Mapeia o sensor de vazão para o canal 2 do ADC.
#define FLOW_ADC_CHANNEL ADS1115_MUX_SINGLE_2

static const float MAX_VOLTAGE = 3.3;
static const float MAX_FLOW = 1.0;

float flow_voltage_read() {
    return adc_module_read_voltage(FLOW_ADC_CHANNEL);
}

float flow_read() {
    float volts = adc_module_read_voltage(FLOW_ADC_CHANNEL);
    return (volts / MAX_VOLTAGE) * MAX_FLOW;
}