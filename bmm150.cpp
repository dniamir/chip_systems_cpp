# include <bmm150.h>
# include <i2c.h>

BMM150::BMM150() {
}

bool BMM150::initialize(void) {
    Wire.begin();

    // Flip power on
    i2c_write(0x10, 0x4B, 1);
    delay(200);

    // Check device_id
    uint8_t bmm_check = i2c_read(0x10, 0x40);

    bool bmm_ok = false;

    if (bmm_check == 0x32) {
        Serial.println("BMM connection was successful");
        bmm_ok = true;
    }
    else{
        Serial.println("BMM connection was not successful");
        bmm_ok = false;
    }

    return bmm_ok;
}
