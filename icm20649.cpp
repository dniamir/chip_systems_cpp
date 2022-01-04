# include <icm20649.h>
# include <Arduino.h>
# include <arduino_i2c.h>
# include <cmath>
# include <vector>
# include <elapsedMillis.h>


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
    // Should give about 0.2 dps-rms

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
    // Should give about 2.4 mgee-rms

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
   
    // Initialize output vectors
    std::vector<int16_t> ax_lsb(fifo_count);
    std::vector<int16_t> ay_lsb(fifo_count);
    std::vector<int16_t> az_lsb(fifo_count);
    std::vector<int16_t> gx_lsb(fifo_count);
    std::vector<int16_t> gy_lsb(fifo_count);
    std::vector<int16_t> gz_lsb(fifo_count);
    std::vector<int16_t> temp_lsb(fifo_count);
    
    std::vector<float> ax_mgee(fifo_count);
    std::vector<float> ay_mgee(fifo_count);
    std::vector<float> az_mgee(fifo_count);
    std::vector<float> gx_dps(fifo_count);
    std::vector<float> gy_dps(fifo_count);
    std::vector<float> gz_dps(fifo_count);
    std::vector<float> temp_degc(fifo_count);

    std::vector<int> reading_time_ms(fifo_count);

    // Batch read from the FIFO and process the data
    for (int i = 0; i < fifo_count; i++) {

        // Initialize data and read from FIFO
        uint8_t register_out[fifo_count];
        read_field("FIFO_R_W", 14, register_out);

        // Save reading time
        reading_time_ms[i] = millis();

        // Save raw data
        ax_lsb[i] = (register_out[0] << 8) | register_out[1];
        ay_lsb[i] = (register_out[2] << 8) | register_out[3];
        az_lsb[i] = (register_out[4] << 8) | register_out[5];

        gx_lsb[i] = (register_out[6] << 8) | register_out[7];
        gy_lsb[i] = (register_out[8] << 8) | register_out[9];
        gz_lsb[i] = (register_out[10] << 8) | register_out[11];

        temp_lsb[i] = (register_out[12] << 8) | register_out[13];

        // Save processed data
        ax_mgee[i] = process_accel(register_out[0], register_out[1], 4000);
        ay_mgee[i] = process_accel(register_out[2], register_out[3], 4000);
        az_mgee[i] = process_accel(register_out[4], register_out[5], 4000);

        gx_dps[i] = process_gyro(register_out[6], register_out[7], 50);
        gy_dps[i] = process_gyro(register_out[8], register_out[9], 50);
        gz_dps[i] = process_gyro(register_out[10], register_out[11], 50);

        temp_degc[i] = process_temperature(register_out[12], register_out[13]);
    }

    // Save readings to FIFO
    last_fifo_reading.count = fifo_count;
    last_fifo_reading.reading_time_ms = reading_time_ms;

    last_fifo_reading.ax_lsb = ax_lsb;
    last_fifo_reading.ay_lsb = ay_lsb;
    last_fifo_reading.az_lsb = az_lsb;
    last_fifo_reading.gx_lsb = gx_lsb;
    last_fifo_reading.gy_lsb = gy_lsb;
    last_fifo_reading.gz_lsb = gz_lsb;
    last_fifo_reading.temp_lsb = temp_lsb;

    last_fifo_reading.ax_mgee = ax_mgee;
    last_fifo_reading.ay_mgee = ay_mgee;
    last_fifo_reading.az_mgee = az_mgee;
    last_fifo_reading.gx_dps = gx_dps;
    last_fifo_reading.gy_dps = gy_dps;
    last_fifo_reading.gz_dps = gz_dps;
    last_fifo_reading.temp_degc = temp_degc;

}

void ICM20649::read_axyz_gxyz() {

    uint8_t register_out[14];

    // Read all registers
    write_field("USER_BANK", 0);
    read_field("ACCEL_XOUT_H", 14, register_out);

    // Save timestamp of reading
    last_os_reading.reading_time_ms = millis();

    // Save raw readings
    last_os_reading.ax_lsb = (register_out[0] << 8) | register_out[1];
    last_os_reading.ay_lsb = (register_out[2] << 8) | register_out[3];
    last_os_reading.az_lsb = (register_out[4] << 8) | register_out[5];

    last_os_reading.gx_lsb = (register_out[6] << 8) | register_out[7];
    last_os_reading.gy_lsb = (register_out[8] << 8) | register_out[9];
    last_os_reading.gz_lsb = (register_out[10] << 8) | register_out[11];

    last_os_reading.temp_lsb = (register_out[12] << 8) | register_out[13];

    // Save processed readings
    last_os_reading.ax_mgee = process_accel(register_out[0], register_out[1], 4000);
    last_os_reading.ay_mgee = process_accel(register_out[2], register_out[3], 4000);
    last_os_reading.az_mgee = process_accel(register_out[4], register_out[5], 4000);

    last_os_reading.gx_dps = process_gyro(register_out[6], register_out[7], 50);
    last_os_reading.gy_dps = process_gyro(register_out[8], register_out[9], 50);
    last_os_reading.gz_dps = process_gyro(register_out[10], register_out[11], 50);

    last_os_reading.temp_degc = process_temperature(register_out[12], register_out[13]);

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
