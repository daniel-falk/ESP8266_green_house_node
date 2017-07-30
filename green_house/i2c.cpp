#include "i2c.h"
#include <Wire.h>
#include "log.h"

bool i2c_init() {
  Wire.begin(I2C_DATA, I2C_CLOCK);

  // Make sure that the ID of BME280 is correct
  int id = read8(BME280_ADDR, BME280_REG_ID);  
  if (id != 0x60) {
    char buf[255];
    snprintf(buf, 255, 
      "Expected BME280 ID to be 0x%02X, found 0x%02X",
      0x60,
      id);
      fatal(buf);
      return false;
  }

  // soft-reset device to make sure we're in a good state
  write8(BME280_ADDR, BME280_REG_ID, 0xB6);  
  return true;
}


bool i2c_read_weather(int *temperature, int *humidity, int *pressure){
  uint8_t data[8];
  readX(BME280_REG_ID, BME280_REG_DATA_START, 8, data);
  *temperature = data[0]<<16 + data[1]<<8 + data[2];
  Serial.print("Temperature: ");
  Serial.println(*temperature);
  return true;
}

static int read8(uint8_t i2c_addr, uint8_t reg_addr) {
  Wire.beginTransmission(i2c_addr);
  Wire.write(reg_addr);
  Wire.endTransmission();
  Wire.requestFrom(i2c_addr, (byte)1);
  return Wire.read();
}

static void readX(uint8_t i2c_addr, uint8_t reg_addr, int len, uint8_t *data) {
  Wire.beginTransmission(i2c_addr);
  Wire.write(reg_addr);
  Wire.endTransmission();
  Wire.requestFrom(i2c_addr, (byte)len);
  for(int i=0; i<len; i++) {
    data[i] = Wire.read();
  }
}

static void write8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data) {
  Wire.beginTransmission(i2c_addr);
  Wire.write(reg_addr);
  Wire.write(data);
  Wire.endTransmission();
}

