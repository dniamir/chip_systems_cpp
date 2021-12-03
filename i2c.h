# include <Arduino.h>
# include <Wire.h>

void i2c_write(short address, short reg, short data);
void i2c_read(short address, short reg, uint8_t* buffer, short length);
void i2c_read(short address, short reg, int8_t* buffer, short length);
uint8_t i2c_read(short address, short reg);