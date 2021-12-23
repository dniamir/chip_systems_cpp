# include <icm20649.h>
# include <Arduino.h>
# include <arduino_i2c.h>
# include <cmath>

ICM20649::ICM20649(int i2c_address_in, ArduinoI2C input_protocol) : Chip(i2c_address_in, input_protocol) {
    Chip::field_map = field_map;
    Chip::who_am_i_reg = who_am_i_reg;
};

ICM20649::ICM20649(ArduinoI2C input_protocol) : Chip(input_protocol){
    Chip::field_map = field_map;
    Chip::who_am_i_reg = who_am_i_reg;
    Chip::i2c_address = i2c_address;
};

bool ICM20649::initialize() {

    // Check device_id
    uint8_t icm_check = read_field("WHO_AM_I");

    bool icm_ok = false;

    if (icm_check == who_am_i_val) {
        Serial.println("ICM20649 connection was successful");
        icm_ok = true;
    }
    else{
        Serial.println("ICM20649 connection was NOT successful");
        icm_ok = false;
    }

    return icm_ok;
}

void ICM20649::default_mode() {
    
    write_field("DEVICE_RESET", 1);
    delay(200);
    
    write_field("USER_BANK", 0);
    write_field("SLEEP", 0);
    write_field("DISABLE_ACCEL", 0);
    write_field("DISABLE_GYRO", 0);
    delay(200);
    
    setup_gyro();
    setup_accel();
    setup_temperature();

}

void ICM20649::setup_gyro() {

    write_field("USER_BANK", 2);

    // Full Scale 
    write_field("GYRO_FS_SEL", 0);  // +/- 500dps

    // Low Pass Filter
    write_field("GYRO_FCHOICE", 1); // Low noise mode, DLPF eneabled
    write_field("GYRO_DLPFCFG", 1); // 3dB bandwidth of 150 Hz

    // Averaging - 2 ^ x averaging
    write_field("GYRO_AVGCFG", 3);  // 8x averaging

    // ODR 1125/(1+GYRO_SMPLRT_DIV)Hz 
    write_field("GYRO_SMPLRT_DIV", 10); // Roughly 102Hz ODR
    delay(200);
}

void ICM20649::setup_accel() {

    write_field("USER_BANK", 2);

    // Full Scale
    write_field("ACCEL_FS_SEL", 0);  // +/- 4G

    // Low Pass Filter
    write_field("ACCEL_FCHOICE", 1);    // Low noise mode, enable DLPF
    write_field("ACCEL_DLPFCFG", 7);    // 3dB bandwidth of 473 Hz 
    // write_field("ACCEL_DLPFCFG", 1);    // 3dB bandwidth of 246 Hz 

    // Averages
    write_field("DEC3_CFG", 3);  // 32x averages 

    // ODR 1125/(1+ACCEL_SMPLRT_DIV)Hz - Comes out to about 102Hz
    write_field("ACCEL_SMPLRT_DIV_1", 0);  // 4 MSB bits of ACCEL_SMPLRT_DIV
    write_field("ACCEL_SMPLRT_DIV_2", 10);  // 8 LSB bits of ACCEL_SMPLRT_DIV
    delay(200);
}

void ICM20649::setup_temperature() {
    write_field("TEMP_DLPFCFG", 2); // 1.125kHz ODr and 123Hz BW
}

void ICM20649::setup_fifo_6axis() {

    write_field("USER_BANK", 0);

    // Turn on all 6-axes
    write_field("ACCEL_FIFO_EN", 1);
    write_field("GYRO_Z_FIFO_EN", 1);
    write_field("GYRO_Y_FIFO_EN", 1);
    write_field("GYRO_X_FIFO_EN", 1);
    write_field("TEMP_FIFO_EN", 1);

    write_field("FIFO_MODE", 0);  // Will continue writing to FIFO when full

    write_field("FIFO_EN", 1);  // Enable FIFO

}

uint16_t ICM20649::read_fifo_count() {

    uint8_t fifo_count_array[2];

    write_field("USER_BANK", 0);
    read_field("FIFO_COUNTH", 2, fifo_count_array);

    uint16_t fifo_count = (fifo_count_array[0] << 8) | fifo_count_array[1];

    return fifo_count;
}

void ICM20649::read_fifo() {

    // Read the number of entries in the FIFO
    uint16_t fifo_count = read_fifo_count();
    fifo_count = fifo_count / 14;

    // Initialize data to read from FIFO
    uint8_t register_out[fifo_count];
    float imu_data[7][fifo_count];

    // Batch read from the FIFO and process the data
    for (int i = 0; i < fifo_count; i++) {
        read_field("FIFO_R_W", 14, register_out);

        imu_data[0][i] = process_accel(register_out[0], register_out[1], 4000);
        imu_data[1][i] = process_accel(register_out[2], register_out[3], 4000);
        imu_data[2][i] = process_accel(register_out[4], register_out[5], 4000);

        imu_data[3][i] = process_gyro(register_out[6], register_out[7], 50);
        imu_data[4][i] = process_gyro(register_out[8], register_out[9], 50);
        imu_data[5][i] = process_gyro(register_out[10], register_out[11], 50);

        imu_data[6][i] = process_temperature(register_out[12], register_out[13]);
    }

    // Average data from FIFO
    float ax_average = average(imu_data[0], (int)fifo_count);
    float ay_average = average(imu_data[1], (int)fifo_count);
    float az_average = average(imu_data[2], (int)fifo_count);
    float gx_average = average(imu_data[3], (int)fifo_count);
    float gy_average = average(imu_data[4], (int)fifo_count);
    float gz_average = average(imu_data[5], (int)fifo_count);
    float temperature_average = average(imu_data[6], (int)fifo_count);
    
    Serial.println();
    Serial.print(ax_average); 
    Serial.print("    ");
    Serial.print(ay_average);
    Serial.print("    ");
    Serial.print(az_average); 
    Serial.print("    ");
    Serial.print(gx_average); 
    Serial.print("    ");
    Serial.print(gy_average);
    Serial.print("    ");
    Serial.print(gz_average); 
    Serial.print("    ");
    Serial.print(temperature_average); 
    Serial.print("    ");
    Serial.print((int)fifo_count); 

}

void ICM20649::read_axyz_gxyz() {

    uint8_t register_out[14];

    // Read all registers
    write_field("USER_BANK", 0);
    read_field("ACCEL_XOUT_H", 13, register_out);

    float ax_mgee = process_accel(register_out[0], register_out[1], 4000);
    float ay_mgee = process_accel(register_out[2], register_out[3], 4000);
    float az_mgee = process_accel(register_out[4], register_out[5], 4000);

    float gx_dps = process_gyro(register_out[6], register_out[7], 50);
    float gy_dps = process_gyro(register_out[8], register_out[9], 50);
    float gz_dps = process_gyro(register_out[10], register_out[11], 50);

    float temperature_degc = process_temperature(register_out[12], register_out[13]);

    Serial.println();
    Serial.print(ax_mgee); 
    Serial.print("    ");
    Serial.print(ay_mgee);
    Serial.print("    ");
    Serial.print(az_mgee); 
    Serial.print("    ");
    Serial.print(gx_dps); 
    Serial.print("    ");
    Serial.print(gy_dps);
    Serial.print("    ");
    Serial.print(gz_dps); 
    Serial.print("    ");
    Serial.print(temperature_degc); 

}

float ICM20649::process_accel(uint8_t accel_msb, uint8_t accel_lsb, int afs_fsr) {

    int16_t accel_lsbs = (accel_msb << 8) | accel_lsb;
    float accel_mgee = accel_lsbs * afs_fsr / pow(2, 15);
    return accel_mgee;
}

float ICM20649::process_gyro(uint8_t gyro_msb, uint8_t gyro_lsb, int gfs_fsr) {

    int16_t gyro_lsbs = (gyro_msb << 8) | gyro_lsb;
    float gyro_dps = gyro_lsbs * gfs_fsr / pow(2, 15);
    return gyro_dps;
}

float ICM20649::process_temperature(uint8_t temperature_msb, uint8_t temperature_lsb) {

    uint16_t temperature_lsbs = (temperature_msb << 8) + temperature_lsb;
    float temperature_degc = (temperature_lsbs - 21) / 333.87 + 15;
    return temperature_degc;
}
