# include <arduino_i2c.h>
# include <protocol.h>
# include <Arduino.h>
# include <Wire.h>

ArduinoI2C::ArduinoI2C(){};

void ArduinoI2C::write_register(int address, int reg, int data) {

    Wire.beginTransmission(address) ;
    Wire.write(reg);
    Wire.write(data);
    Wire.endTransmission();
}

void ArduinoI2C::write_register16(int address, int reg, int data) {

    Wire.beginTransmission(address) ;
    Wire.write(reg);
    Wire.write(data & 0b11111111);
    Wire.write(data >> 8);
    Wire.endTransmission();
}

int ArduinoI2C::read_register(int address, int reg) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(address, 1);
    uint8_t byte = Wire.read();

    return byte;
}

uint16_t ArduinoI2C::read_register16(int address, int reg) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(address, 2);
    
    uint16_t byte = Wire.read();
    uint16_t double_byte = (Wire.read() << 8) | byte;

    return double_byte;
}

void ArduinoI2C::read_register(int address, int reg, int bytes_to_read, int8_t reg_out[]) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(address, bytes_to_read);

    for (int i=0; i<bytes_to_read; ++i) {
        reg_out[i] = Wire.read();
    }
}

void ArduinoI2C::read_register(int address, int reg, int bytes_to_read, uint8_t reg_out[]) {
    Wire.beginTransmission(address);
    Wire.write(reg);
    Wire.endTransmission();
    Wire.requestFrom(address, bytes_to_read);

    for (int i=0; i<bytes_to_read; ++i) {
        reg_out[i] = Wire.read();
    }
}