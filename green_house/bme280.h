#ifndef __BME280_H
#define __BME280_H

#include <Arduino.h>

// Chip register addresses
#define BME280_REG_ID ((uint8_t)0xD0)
#define BME280_REG_DATA_START ((uint8_t)0xF7)
#define BME280_REG_CALIB_0_25_START ((uint8_t)0x88)
#define BME280_REG_CALIB_26_41_START ((uint8_t)0xE1)
#define BME280_REG_STATUS ((uint8_t)0xF3)
#define BME280_REG_RESET ((uint8_t)0xE0)
#define BME280_REG_CONFIG ((uint8_t)0xF5)
#define BME280_REG_CTRL_MEAS ((uint8_t)0xF4)
#define BME280_REG_CTRL_HUM ((uint8_t)0xF2)

// Data length
#define BME280_PRESS_LEN 3
#define BME280_TEMP_LEN 3
#define BME280_HUM_LEN 2

// Flags
#define BME280_STATUS_NVM_READY(a) (!(a & 0x01))

class Bme280{

  public:
   /*
     * Set the i2c address of the device
     */
    void set_addr(uint8_t addr);
    
    
    /*
     * Set the i2c address of the device
     */
    uint8_t get_addr();

    /*
     * During power-on-reset the calibration data is copied
     * to the non volatile memory.
     * 
     * Returns:
     * True - No copy is currently performed
     * False - Data is currently being copied
     */
    bool nvm_ready(uint8_t status);
  
    /*
     * Get the config register byte
     * 
     * Arguments:
     * - sleep: Sleep time in normal mode (000-111 see datasheet)
     * - filter: IIR filter time constant (000-100 see datasheet)
     * - NOT IMPLEMENTED: spi3w_en, enable 3 wire SPI
     */
    uint8_t build_config(uint8_t sleep, uint8_t filter);
    
    
    /*
     * Set the mode and oversampling for pressure and temperature
     * 
     * If IIR is used both temperature and pressure is 20 bit otherwise
     * given by 16 + (osrs - 1), eg an oversampling of 3 results in 18 bit precision
     * 
     * Arguments:
     * - osrs_t: Oversampling for temperature readings (000-101 see datasheet)
     * - osrs_p: Oversampling for pressure readings (000-101 see datasheet)
     * - mode: Chip mode (00 - sleep mode, 01 and 10 - forced mode, 11 - normal mode)
     */
    uint8_t build_ctrl_meas(uint8_t osrs_t, uint8_t osrs_p, uint8_t mode);
    
    
    /*
     * Set the oversampling for humidity readings to reduce noise
     * 
     * Arguments:
     * - osrs_h: Oversampling for temperature readings (000-101 see datasheet)
     */
    uint8_t build_ctrl_hum(uint8_t osrs_h);

    
    /*
     * Calculates the calibration values from raw BME280-NVM values
     * 
     * Arguments:
     * - raw_data: 15 byte array with raw data
     * 
     */
     void set_calibration(uint8_t *data);

    
    /*
     * Calculates the temperature in centdegrees, ie. 5123 represents 51.23 degrees
     * 
     * Arguments:
     * - dev: The bme280 struct containing the calibration parameters
     * - adc_t: The ADC temperature value from a bme280 sensor
     * 
     * Returns:
     * Temperature in centi degrees
     */
    int32_t temperature(uint32_t adc_t);

  private:
    uint8_t i2c_addr;
    
    uint8_t calib_raw[15];
    int32_t tfine;
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
};

#endif
