/**
* @file sensor_manager.h
* @brief Interface for managing sensor readings
*/
#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

/**
* @brief Error value used in the fields of the reading structure
*
* Indicates that the reading of a specific sensor failed
* (e.g., communication failure with the ADC or invalid raw value)
*
* Used to populate the 'sensors_reading_t' fields when
* 'sensors_read_all()' returns an error code
*/
#define SENSOR_READ_ERROR -1.0f

/**
 * @brief Structure for sensor reading data
 * 
 * This structure is populated by the 'sensors_read_all()' function
 * In case of reading failure, the fields will contain SENSOR_READ_ERROR
 */
typedef struct {
    float temperature; /**< Temperature value in degrees Celsius */
    float conductivity; /**< Conductivity value in microsiemens per centimeter */
    float flow; /**< Flow rate value in liters */
} sensors_reading_t;

/**
 * @brief Initializes all sensors and hardware modules
 * 
 * Must be called successfully before any read function
 * 
 * @return 0 on success
 * @return 1 on failure (e.g., failure to initialize the ADC module)
 */
int sensors_init(void);

/**
 * @brief Reads all channels of the ADC module and fills the data structure
 * 
 * @param reading [out] Pointer to the structure where the data was read
 * 
 * @return 0 on success (all data was read)
 * @return 1 on error (e.g., 'reading' pointer is NULL or communication failure
 * with the ADC hardware)
 * 
 * @warning In case of an error (return 1),
 * all fields of the 'reading' structure will be
 * filled with the value SENSOR_READ_ERROR
 */
int sensors_read_all(sensors_reading_t* reading);

#endif // SENSOR_MANAGER_H
