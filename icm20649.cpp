# include <icm20649.h>
# include <i2c.h>

ICM20649::ICM20649() {
}

bool ICM20649::initialize(void) {
    Wire.begin();

    // Check device_id
    uint8_t icm_check = i2c_read(0x68, 0x00);

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
