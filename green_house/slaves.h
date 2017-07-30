#ifndef __SLAVES_H
#define __SLAVES_H

#include <Arduino.h>
#include "bme280.h"
#include "slave.h"

#define I2C_DATA D2
#define I2C_CLOCK D1

Bme280 *slaves_init(int *num_slaves);
bool slaves_read_weather(Bme280 *dev, int *temperature, int *humidity, int *pressure);

#endif
