#include "bme280.h"

/*
 * Helper methods for the BME280 weather sensor chip
 */


/*
 * Set the i2c address of the device
 */
void Bme280::set_addr(uint8_t addr) {
  i2c_addr = addr;
}


/*
 * Set the i2c address of the device
 */
uint8_t Bme280::get_addr() {
  return i2c_addr;
}

/*
 * During power-on-reset the calibration data is copied
 * to the non volatile memory.
 * 
 * Returns:
 * True - No copy is currently performed
 * False - Data is currently being copied
 */
bool Bme280::nvm_ready(uint8_t status) {
  return BME280_STATUS_NVM_READY(status);
}


/*
 * Get the config register byte
 * 
 * Arguments:
 * - sleep: Sleep time in normal mode (000-111 see datasheet)
 * - filter: IIR filter time constant (000-100 see datasheet)
 * - NOT IMPLEMENTED: spi3w_en, enable 3 wire SPI
 */
uint8_t Bme280::build_config(uint8_t sleep, uint8_t filter) {
  uint8_t spi3w_en = 0;
  return sleep<<5 | filter<<2 | spi3w_en;
}


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
uint8_t Bme280::build_ctrl_meas(uint8_t osrs_t, uint8_t osrs_p, uint8_t mode) {
  return osrs_t<<5 | osrs_p<<2 | mode;
}


/*
 * Set the oversampling for humidity readings to reduce noise
 * 
 * Arguments:
 * - osrs_h: Oversampling for temperature readings (000-101 see datasheet)
 */
uint8_t Bme280::build_ctrl_hum(uint8_t osrs_h) {
  return osrs_h;
}


/*
 * Calculates the calibration values from raw BME280-NVM values
 * 
 * Data is located in 0x88-0xA1 (26 bytes) and 0xE1 to 0xF0 (16 bytes)
 * All 42 bytes is assumed to be located continiusly from 0 to 41
 * 
 * Arguments:
 * - raw_data: 42 byte array with raw data
 * 
 */
void Bme280::set_calibration(uint8_t *data) {
  dig_T1 = ((uint16_t)data[1]<<8) + data[0];
  dig_T2 = (int16_t)(((uint16_t)data[3]<<8) + data[2]);
  dig_T3 = (int16_t)(((uint16_t)data[5]<<8) + data[4]);
}


/*
 * Calculates the temperature in centi degrees, ie. 5123 represents 51.23 degrees
 * 
 * Arguments:
 * - adc_t: The ADC temperature value from a bme280 sensor
 * 
 * Returns:
 * Temperature in centi degrees
 */
int32_t Bme280::temperature(uint32_t adc_t) {
  int32_t var1, var2;
  adc_t >>= 4; // Shift down to 20-bit resulution
  var1 = ((((adc_t>>3) - ((int32_t)dig_T1<<1))) * ((int32_t)dig_T2)) >> 11;
  var2  = (((((adc_t>>4) - ((int32_t)dig_T1)) * \
          ((adc_t>>4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  tfine = var1 + var2; // Save for pressure compensation later on
  return (tfine * 5 + 128) >> 8;
}


