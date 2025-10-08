# ====================================================================
# Arquivo de Configuração de Comportamento do Firmware
# ====================================================================

# --- Configurações de Rede e Comunicação ---
set(HTTP_TIMEOUT_MS 10000)

# --- Configurações de Calibração de Sensores ---

# -- Temperatura --
set(SENSOR_TEMPERATURE_MAX_VOLTAGE 3.3)
set(SENSOR_TEMPERATURE_MAX_VALUE 100.0)
set(SENSOR_TEMPERATURE_MIN_VALUE 0.0)

# -- Condutividade --
set(SENSOR_CONCENTRATION_MAX_VOLTAGE 3.3)
set(SENSOR_CONCENTRATION_MAX_VALUE 15000.0)
set(SENSOR_CONCENTRATION_MIN_VALUE 0.0)

# -- Vazão --
set(SENSOR_FLOW_MAX_VOLTAGE 3.3)
set(SENSOR_FLOW_MAX_VALUE 100.0)
set(SENSOR_FLOW_MIN_VALUE 0.0)

# --- Configurações da Lógica Principal ---
set(MAIN_TASK_CYCLE_INTERVAL_MS 1000)
set(SYSTEM_WATCHDOG_TIMEOUT_MS 5000)