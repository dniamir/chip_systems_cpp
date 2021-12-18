# include <icm20649.h>
# include <Arduino.h>
# include <arduino_i2c.h>

ICM20649::ICM20649(ArduinoI2C &comm_protocol){

    // static Protocol comm_protocol = comm_protocol;

};

bool ICM20649::initialize(void) {

    // Check device_id
    uint8_t icm_check = comm_protocol.read_register(0x68, 0x00);

    bool icm_ok = false;

    if (icm_check == 0xE1) {
        Serial.println("ICM20649 connection was successful");
        icm_ok = true;
    }
    else{
        Serial.println("ICM20649 connection was NOT successful");
        icm_ok = false;
    }

    return icm_ok;
}
