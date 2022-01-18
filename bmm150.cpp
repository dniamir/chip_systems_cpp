# include <bmm150.h>
# include <arduino_i2c.h>
# include <Arduino.h>

BMM150::BMM150(int i2c_address_in, ArduinoI2C input_protocol) : Chip(i2c_address_in, input_protocol) {
    Chip::field_map = field_map;
    Chip::who_am_i_reg = who_am_i_reg;
};

BMM150::BMM150(ArduinoI2C input_protocol) : Chip(input_protocol){
    Chip::field_map = field_map;
    Chip::who_am_i_reg = who_am_i_reg;
    Chip::i2c_address = i2c_address;
};

bool BMM150::initialize(void) {

    // Flip power on
    write_field("Power Control Bit", 1);
    delay(200);

    // Check device_id
    int bmm_check = read_field("Chip ID");

    bool bmm_ok = false;

    if (bmm_check == who_am_i_val) {
        Serial.println("BMM connection was successful");
        bmm_ok = true;
    }
    else{
        Serial.println("BMM connection was NOT successful");
        bmm_ok = false;
    }

    // Read Trim registers
    read_trim_registers();

    return bmm_ok;
}

void BMM150::default_mode() {

    // Exit sleep mode
    write_field("Opmode", 0b00);

    // Set data rate to 10Hz
    write_field("Data Rate", 0b000);

    // Set X/Y repititions
    write_field("REPXY", 9);

    // Set Z repititions
    write_field("REPZ", 15);

    // Turn on all 3 axes (Enable is 0, not 1)
    write_field("Channel X", 0);
    write_field("Channel Y", 0);
    write_field("Channel Z", 0);
}

void BMM150::high_accuracy_mode() {

    // Exit sleep mode
    write_field("Opmode", 0b00);

    // Set data rate to 15Hz
    write_field("Data Rate", 0b100);

    // Set X/Y repititions
    write_field("REPXY", 47);

    // Set Z repititions
    write_field("REPZ", 83);

    // Turn on all 3 axes (Enable is 0, not 1)
    write_field("Channel X", 0);
    write_field("Channel Y", 0);
    write_field("Channel Z", 0);
}

void BMM150::read_mxyz() {

    // Read data registers
    uint8_t register_out[8];
    read_field(0x42, 8, register_out);

    // Save reading time
    last_os_reading.reading_time_ms = millis();

    // Temperature resistor
    int16_t mx_lsb = (int16_t)((int16_t)(int8_t)register_out[1] << 5) | (register_out[0] >> 3);
    int16_t my_lsb = (int16_t)((int16_t)(int8_t)register_out[3] << 5) | (register_out[2] >> 3);
    int16_t mz_lsb = (int16_t)((int16_t)(int8_t)register_out[5] << 7) | (register_out[4] >> 1);
    uint16_t rhall = (register_out[7] << 6) | (register_out[6] >> 2);

    // Save processed data
    last_os_reading.mx_ut = compensate_x(mx_lsb, rhall);
    last_os_reading.my_ut = compensate_y(my_lsb, rhall);
    last_os_reading.mz_ut = compensate_z(mz_lsb, rhall);

    // Save raw data
    last_os_reading.mx_lsb = mx_lsb;
    last_os_reading.my_lsb = my_lsb;
    last_os_reading.mz_lsb = mz_lsb;
    last_os_reading.rhall = rhall;

}

void BMM150::read_trim_registers() {

    uint16_t temp_msb;

    // Trim register value is read
    uint8_t BMM150_DIG_X1 = read_field("BMM150_DIG_X1");
    uint8_t BMM150_DIG_Y1 = read_field("BMM150_DIG_Y1");
    uint8_t BMM150_DIG_Z4_LSB = read_field("BMM150_DIG_Z4_LSB");
    uint8_t BMM150_DIG_Z4_MSB = read_field("BMM150_DIG_Z4_MSB");
    uint8_t BMM150_DIG_X2 = read_field("BMM150_DIG_X2");
    uint8_t BMM150_DIG_Y2 = read_field("BMM150_DIG_Y2");
    uint8_t BMM150_DIG_Z2_LSB = read_field("BMM150_DIG_Z2_LSB");
    uint8_t BMM150_DIG_Z2_MSB = read_field("BMM150_DIG_Z2_MSB");
    uint8_t BMM150_DIG_Z1_LSB = read_field("BMM150_DIG_Z1_LSB");
    uint8_t BMM150_DIG_Z1_MSB = read_field("BMM150_DIG_Z1_MSB");
    uint8_t BMM150_DIG_XYZ1_LSB = read_field("BMM150_DIG_XYZ1_LSB");
    uint8_t BMM150_DIG_XYZ1_MSB = read_field("BMM150_DIG_XYZ1_MSB");
    uint8_t BMM150_DIG_Z3_LSB = read_field("BMM150_DIG_Z3_LSB");
    uint8_t BMM150_DIG_Z3_MSB = read_field("BMM150_DIG_Z3_MSB");
    uint8_t BMM150_DIG_XY2 = read_field("BMM150_DIG_XY2");
    uint8_t BMM150_DIG_XY1 = read_field("BMM150_DIG_XY1");

    // Trim data which is read is updated in the device structure
    trim_data.dig_x1 = (int8_t)BMM150_DIG_X1;
    trim_data.dig_y1 = (int8_t)BMM150_DIG_Y1;
    trim_data.dig_x2 = (int8_t)BMM150_DIG_X2;
    trim_data.dig_y2 = (int8_t)BMM150_DIG_Y2;

    temp_msb = ((uint16_t)BMM150_DIG_Z1_MSB) << 8;
    trim_data.dig_z1 = (uint16_t)(temp_msb | BMM150_DIG_Z1_LSB);

    temp_msb = ((uint16_t)BMM150_DIG_Z2_MSB) << 8;
    trim_data.dig_z2 = (int16_t)(temp_msb | BMM150_DIG_Z2_LSB);

    temp_msb = ((uint16_t)BMM150_DIG_Z3_MSB) << 8;
    trim_data.dig_z3 = (int16_t)(temp_msb | BMM150_DIG_Z3_LSB);

    temp_msb = ((uint16_t)BMM150_DIG_Z4_MSB) << 8;
    trim_data.dig_z4 = (int16_t)(temp_msb | BMM150_DIG_Z4_LSB);

    trim_data.dig_xy1 = BMM150_DIG_XY1;
    trim_data.dig_xy2 = (int8_t)BMM150_DIG_XY2;

    temp_msb = ((uint16_t)(BMM150_DIG_XYZ1_MSB & 0x7F)) << 8;
    trim_data.dig_xyz1 = (uint16_t)(temp_msb | BMM150_DIG_XYZ1_LSB);

}

int16_t BMM150::compensate_x(int16_t mx, uint16_t rhall) {

    int16_t retval;
    uint16_t process_comp_x0 = 0;
    int32_t process_comp_x1;
    uint16_t process_comp_x2;
    int32_t process_comp_x3;
    int32_t process_comp_x4;
    int32_t process_comp_x5;
    int32_t process_comp_x6;
    int32_t process_comp_x7;
    int32_t process_comp_x8;
    int32_t process_comp_x9;
    int32_t process_comp_x10;

    if (rhall != 0) {
        /* Availability of valid data*/
        process_comp_x0 = rhall;
    } else if (trim_data.dig_xyz1 != 0) {
        process_comp_x0 = trim_data.dig_xyz1;
    } else {
        process_comp_x0 = 0;
    }

    process_comp_x1 = ((int32_t)trim_data.dig_xyz1) * 16384;
    process_comp_x2 = ((uint16_t)(process_comp_x1 / process_comp_x0)) - ((uint16_t)0x4000);
    retval = ((int16_t)process_comp_x2);
    process_comp_x3 = (((int32_t)retval) * ((int32_t)retval));
    process_comp_x4 = (((int32_t)trim_data.dig_xy2) * (process_comp_x3 / 128));
    process_comp_x5 = (int32_t)(((int16_t)trim_data.dig_xy1) * 128);
    process_comp_x6 = ((int32_t)retval) * process_comp_x5;
    process_comp_x7 = (((process_comp_x4 + process_comp_x6) / 512) + ((int32_t)0x100000));
    process_comp_x8 = ((int32_t)(((int16_t)trim_data.dig_x2) + ((int16_t)0xA0)));
    process_comp_x9 = ((process_comp_x7 * process_comp_x8) / 4096);
    process_comp_x10 = ((int32_t)mx) * process_comp_x9;
    retval = ((int16_t)(process_comp_x10 / 8192));
    retval = (retval + (((int16_t)trim_data.dig_x1) * 8)) / 16;

    return retval;
}

int16_t BMM150::compensate_y(int16_t my, uint16_t rhall) {

    int16_t retval;
    uint16_t process_comp_y0 = 0;
    int32_t process_comp_y1;
    uint16_t process_comp_y2;
    int32_t process_comp_y3;
    int32_t process_comp_y4;
    int32_t process_comp_y5;
    int32_t process_comp_y6;
    int32_t process_comp_y7;
    int32_t process_comp_y8;
    int32_t process_comp_y9;

    if (rhall != 0) {
        /* Availability of valid data*/
        process_comp_y0 = rhall;
    } else if (trim_data.dig_xyz1 != 0) {
        process_comp_y0 = trim_data.dig_xyz1;
    } else {
        process_comp_y0 = 0;
    }

    process_comp_y1 = (((int32_t)trim_data.dig_xyz1) * 16384) / process_comp_y0;
    process_comp_y2 = ((uint16_t)process_comp_y1) - ((uint16_t)0x4000);
    retval = ((int16_t)process_comp_y2);
    process_comp_y3 = ((int32_t) retval) * ((int32_t)retval);
    process_comp_y4 = ((int32_t)trim_data.dig_xy2) * (process_comp_y3 / 128);
    process_comp_y5 = ((int32_t)(((int16_t)trim_data.dig_xy1) * 128));
    process_comp_y6 = ((process_comp_y4 + (((int32_t)retval) * process_comp_y5)) / 512);
    process_comp_y7 = ((int32_t)(((int16_t)trim_data.dig_y2) + ((int16_t)0xA0)));
    process_comp_y8 = (((process_comp_y6 + ((int32_t)0x100000)) * process_comp_y7) / 4096);
    process_comp_y9 = (((int32_t)my) * process_comp_y8);
    retval = (int16_t)(process_comp_y9 / 8192);
    retval = (retval + (((int16_t)trim_data.dig_y1) * 8)) / 16;

    return retval;
}

int16_t BMM150::compensate_z(int16_t mz, uint16_t rhall) {

    int32_t retval;
    int16_t process_comp_z0;
    int32_t process_comp_z1;
    int32_t process_comp_z2;
    int32_t process_comp_z3;
    int16_t process_comp_z4;

    process_comp_z0 = ((int16_t)rhall) - ((int16_t) trim_data.dig_xyz1);
    process_comp_z1 = (((int32_t)trim_data.dig_z3) * ((int32_t)(process_comp_z0))) / 4;
    process_comp_z2 = (((int32_t)(mz - trim_data.dig_z4)) * 32768);
    process_comp_z3 = ((int32_t)trim_data.dig_z1) * (((int16_t)rhall) * 2);
    process_comp_z4 = (int16_t)((process_comp_z3 + (32768)) / 65536);
    retval = ((process_comp_z2 - process_comp_z1) / (trim_data.dig_z2 + process_comp_z4));

    retval = retval / 16;

    return (int16_t)retval;
}

	


