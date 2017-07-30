#include <Wire.h>
#include "log.h"
#include "slaves.h"

/* 
 *  Communication with slave devices over i2c.
 *  
 *  Currently including: 
 *  - one BME280 weather sensor (air pressure, temperature and humidity)
 *  
  */
  
static bool init_WeatherSensor(Bme280 *dev);
static int read8(uint8_t i2c_addr, uint8_t reg_addr);
static void readX(uint8_t i2c_addr, uint8_t reg_addr, int len, uint8_t *data);
static void write8(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);


/*
 * Returns a list of slave objects initiated
 * 
 * Modifies the num_slaves to the number of objects initiated
 * 
 */
Bme280 *slaves_init(int *num_slaves) {
  Wire.begin(I2C_DATA, I2C_CLOCK);

  bool all_ok = true;

  Bme280 *bme = new Bme280;
  bme->set_addr(0x76);
  all_ok &= init_WeatherSensor(bme);
  // FIXME: Should return an array with slave structs
  //s[0].type = TYPE_BME280;
  //s[0].description = "Green House Weather Sensor";
  //s[0].object = bme;

  if(!all_ok)
    *num_slaves = 0;

  return bme;
}

bool slaves_read_weather(Bme280 *dev, int *temperature, int *humidity, int *pressure){
  uint8_t data[8];
  readX(dev->get_addr(), BME280_REG_DATA_START, 8, data);
  *temperature = (((((uint32_t)data[3])<<8) + data[4])<<8) + data[5]; // adc temperature
  *temperature = dev->temperature(*temperature); // Calc real temperature
  return true;
}

static bool init_WeatherSensor(Bme280 *dev) {
  // Make sure that the ID of BME280 is correct
  int id = read8(dev->get_addr(), BME280_REG_ID);  
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
  write8(dev->get_addr(), BME280_REG_RESET, 0xB6);

  // Wait for non volatile memory to be filled with calibration data
  // Datasheet does not specify time from POR to start of nvm copy.. Smaller possible?
  delay(1000); 
  while(!dev->nvm_ready(read8(dev->get_addr(), BME280_REG_STATUS)))
    delay(100);

  // Read the calibration data
  uint8_t data[42];
  readX(dev->get_addr(), BME280_REG_CALIB_0_25_START, 26, data);
  readX(dev->get_addr(), BME280_REG_CALIB_26_41_START, 16, data+26);
  dev->set_calibration(data);

  // Set the mode and oversampling
  // Changes to ctrl_hum only goes active after a write to ctrl_meas
  write8(dev->get_addr(), BME280_REG_CONFIG, dev->build_config(6,0)); // 10mS sleep (if normal mode), Deactivate IIR-filter
  write8(dev->get_addr(), BME280_REG_CTRL_HUM, dev->build_ctrl_hum(1)); // oversampming x1
  write8(dev->get_addr(), BME280_REG_CTRL_MEAS, dev->build_ctrl_meas(1,1,3)); // oversampming x1, normal mode

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

