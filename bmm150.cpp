# include <bmm150.h>
# include <arduino_i2c.h>
# include <Arduino.h>

int TwosComp(int decimal, int bits=8) {

    // If MSB is 1
    if ((decimal & (1 << (bits - 1))) != 0) {
        decimal = decimal - (1 << bits);
    }  
	return decimal;
}

BMM150::BMM150(ArduinoI2C input_protocol){

    // static Protocol comm_protocol = comm_protocol;
    comm_protocol = input_protocol;

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

    read_trim_registers();

    return bmm_ok;
}

void BMM150::default_mode(void) {

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

void BMM150::write_field(String field, int field_val) {

    // Check register map for register
    Field field_to_write = field_map[field];
    write_field(field_to_write.address, field_val, field_to_write.offset, field_to_write.length);
}

void BMM150::write_field(int field, int field_val, int offset, int field_length) {

    // Do masking if this is a true field and not a register
    // Serial.println();
    // Serial.println("Write Field");
    // Serial.print("Field: ");
    // Serial.println(field);
    // Serial.print("field_val: ");
    // Serial.println(field_val);
    // Serial.print("offset: ");
    // Serial.println(offset);
    // Serial.print("field_length: ");
    // Serial.println(field_length);

    if (field_length != 8) {
        int curr_field_val = read_field(field);

        // Serial.print("curr_field_val: ");
        // Serial.println(curr_field_val);

        // Zero-ing mask
        int mask1 = pow(2, field_length) - 1;
        mask1 = mask1 << offset;
        mask1 = 0b11111111 - mask1;

        // Serial.print("mask1: ");
        // Serial.println(mask1);

        // Mask adding actual field_val
        int mask2 = field_val << offset;

        // Serial.print("mask2: ");
        // Serial.println(mask2);
        
        // Final masking
        field_val = (curr_field_val & mask1) | mask2;

        // Serial.print("field_val: ");
        // Serial.println(field_val);
        // Serial.println();
    }

    write_field(field, field_val);
}

void BMM150::write_field(int field, int field_val) {

    comm_protocol.write_register(i2c_address, field, field_val);
}

int BMM150::read_field(String field) {
    Field field_to_write = field_map[field];
    return(read_field(field_to_write.address, field_to_write.offset, field_to_write.length));
}

int BMM150::read_field(int field, int offset, int field_length) {

    int field_out = read_field(field);

    // Serial.println();
    // Serial.println("Read Register:");
    // Serial.print("field: ");
    // Serial.println(field);
    // Serial.print("offset: ");
    // Serial.println(offset);
    // Serial.print("field_length: ");
    // Serial.println(field_length);
    // Serial.print("field_out_pre: ");
    // Serial.println(field_out);

    // Do masking if this is a true field and not a register
    if (field_length != 8) {

        // 1 Masking
        int mask1 = pow(2, field_length) - 1;
        mask1 = mask1 << offset;
        
        // Final masking
        field_out = (field_out & mask1) >> offset;
    }

    // Serial.print("field_out_post: ");
    // Serial.println(field_out);
    // Serial.println();

    return field_out;

}

int BMM150::read_field(int field) {

    return(comm_protocol.read_register(i2c_address, field));
}

// int BMM150::read_field(int field, int bytes_to_read) {

//     return(comm_protocol.read_register(i2c_address, field));
// }

void BMM150::read_mxyz() {

    int8_t register_out[8];
    comm_protocol.read_register(i2c_address, 0x42, 8, register_out);

    // Temperature resistor
    int8_t rhall_lsb = register_out[6] >> 2;
    uint16_t rhall_msb = register_out[7] << 6;
    uint16_t rhall = rhall_msb + rhall_lsb;

    int8_t mx_lsb = register_out[0] >> 3;
    int16_t mx_msb = register_out[1] << 5;
    int16_t mx = mx_msb + mx_lsb;

    int8_t my_lsb = register_out[2] >> 3;
    int16_t my_msb = register_out[3] << 5;
    int16_t my = my_msb + my_lsb;

    int8_t mz_lsb = register_out[4] >> 1;
    int16_t mz_msb = register_out[5] << 7;
    int16_t mz = mz_msb + mz_lsb;

    float mx_float = compensate_x(mx, rhall) * 0.3173828125;    // uT / LSB
    float my_float = compensate_x(my, rhall) * 0.3173828125;    // uT / LSB
    float mz_float = compensate_x(mz, rhall) * 0.30517578125;   // uT / LSB

    Serial.println(mx_float);
    Serial.println(my_float);
    Serial.println(mz_float);

    // Serial.println("BMM Read");
    // Serial.print("LSB Register: ");
    // Serial.println(register_out[0]);
    // Serial.print("MSB Register: ");
    // Serial.println(register_out[1]);
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

    process_comp_x0 = rhall;
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

    process_comp_y0 = rhall;
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

    return retval;
}

	


