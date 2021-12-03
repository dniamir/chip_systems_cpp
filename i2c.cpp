# include <i2c.h>
# include <Arduino.h>
# include <Wire.h>

void i2c_write(short address, short reg, short data) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

void i2c_read(short address, short reg, uint8_t* buffer, short length) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(address, length);

    if (Wire.available() == length) {
        for (uint8_t i = 0; i < length; i++) {
            buffer[i] = Wire.read();
        }
    }
}

void i2c_read(short address, short reg, int8_t* buffer, short length) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(address, length);

    if (Wire.available() == length) {
        for (uint8_t i = 0; i < length; i++) {
            buffer[i] = Wire.read();
        }
    }
}

uint8_t i2c_read(short address, short reg) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(address, 1);
    uint8_t byte = Wire.read();
    return byte;
}