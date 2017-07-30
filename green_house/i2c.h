#ifndef __I2C_H
#define __I2C_H

#include <Arduino.h>

#define I2C_DATA D2
#define I2C_CLOCK D1

// I2C address
#define BME280_ADDR ((uint8_t)0x76)

// Chip register addresses
#define BME280_REG_ID ((uint8_t)0xD0)
#define BME280_REG_DATA_START ((uint8_t)0xF7)
#define BME280_REG_RESTART ((uint8_t)0xE0)

// Data length
#define BME280_PRESS_LEN 3
#define BME280_TEMP_LEN 3
#define BME280_HUM_LEN 2


bool i2c_init();
bool i2c_read_weather(int *temperature, int *humidity, int *pressure);

static int read8(uint8_t i2c_addr, uint8_t reg_addr);
static void readX(uint8_t i2c_addr, uint8_t reg_addr, int len, uint8_t *data);
static void write8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);

#endif
