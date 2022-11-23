# include <bme680.h>
# include <Arduino.h>
# include <arduino_i2c.h>

BME680::BME680(int i2c_address_in, ArduinoI2C input_protocol) : Chip(i2c_address_in, input_protocol) {
    Chip::field_map = field_map;
    Chip::who_am_i_reg = who_am_i_reg;
};

BME680::BME680(ArduinoI2C input_protocol) : Chip(input_protocol) {
    Chip::field_map = field_map;
    Chip::who_am_i_reg = who_am_i_reg;
    Chip::i2c_address = BME680::i2c_address;
};

void BME680::soft_reset() {
    BME680::write_field("reset", 0xB6);
}

void BME680::read_cal_codes() {
    BME680::cal_codes.par_t1 = BME680::read_field("par_t1") | (BME680::read_field(0xea) << 8);
    BME680::cal_codes.par_t2 = BME680::read_field("par_t2") | (BME680::read_field(0x8b) << 8);
    BME680::cal_codes.par_t3 = BME680::read_field("par_t3");

    BME680::cal_codes.par_p1 = BME680::read_field("par_p1") | (BME680::read_field(0x8f) << 8);
    BME680::cal_codes.par_p2 = BME680::read_field("par_p2") | (BME680::read_field(0x91) << 8);
    BME680::cal_codes.par_p3 = BME680::read_field("par_p3");
    BME680::cal_codes.par_p4 = BME680::read_field("par_p4") | (BME680::read_field(0x95) << 8);
    BME680::cal_codes.par_p5 = BME680::read_field("par_p5") | (BME680::read_field(0x97) << 8);
    BME680::cal_codes.par_p6 = BME680::read_field("par_p6");
    BME680::cal_codes.par_p7 = BME680::read_field("par_p7");
    BME680::cal_codes.par_p8 = BME680::read_field("par_p8") | (BME680::read_field(0x9d) << 8);
    BME680::cal_codes.par_p9 = BME680::read_field("par_p9") | (BME680::read_field(0x9f) << 8);
    BME680::cal_codes.par_p10 = BME680::read_field("par_p10");

    BME680::cal_codes.par_h1 = BME680::read_field("par_h1") | (BME680::read_field(0x91) << 8);
    BME680::cal_codes.par_h2 = BME680::read_field("par_h2") | (BME680::read_field(0x91) << 8);
    BME680::cal_codes.par_h3 = BME680::read_field("par_h3");
    BME680::cal_codes.par_h4 = BME680::read_field("par_h4");
    BME680::cal_codes.par_h5 = BME680::read_field("par_h5");
    BME680::cal_codes.par_h6 = BME680::read_field("par_h6");
    BME680::cal_codes.par_h7 = BME680::read_field("par_h7");

    BME680::cal_codes.par_g1 = BME680::read_field("par_g1");
    BME680::cal_codes.par_g2 = BME680::read_field("par_g2") | (BME680::read_field(0xec) << 8);
    BME680::cal_codes.par_g3 = BME680::read_field("par_g3");
}

int32_t BME680::read_temperature() {

    BME680::write_field("mode", 0b01);

    uint8_t temp_out[3];
    BME680::read_field("temp_msb", 3, temp_out);
    uint32_t temp_adc = (temp_out[0] << 12) | (temp_out[1] << 4) | (temp_out[2] >> 4);

    // Get Calibration Codes
    int16_t par_t1 = BME680::cal_codes.par_t1;
    int16_t par_t2 = BME680::cal_codes.par_t2;
    uint16_t par_t3 = BME680::cal_codes.par_t3;

    int64_t var1;
    int64_t var2;
    int64_t var3;
    int16_t calc_temp;

    // Calibrate temperature measurements
    var1 = ((int32_t)temp_adc >> 3) - ((int32_t)par_t1 << 1);
    var2 = (var1 * (int32_t)par_t2) >> 11;
    var3 = ((((var1 >> 1) * (var1 >> 1)) >> 12) * ((int32_t)par_t3 << 4)) >> 14;
    int32_t t_fine = var2 + var3;
    int32_t temp_comp = ((t_fine * 5) + 128) >> 8;

    return temp_comp;
}