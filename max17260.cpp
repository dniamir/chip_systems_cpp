# include <max17260.h>
# include <Arduino.h>
# include <arduino_i2c.h>

MAX17260::MAX17260(int i2c_address_in, ArduinoI2C input_protocol) : Chip(i2c_address_in, input_protocol) {
    Chip::field_map = field_map;
    Chip::who_am_i_reg = who_am_i_reg;
};

MAX17260::MAX17260(ArduinoI2C input_protocol) : Chip(input_protocol) {
    Chip::field_map = field_map;
    Chip::who_am_i_reg = who_am_i_reg;
    Chip::i2c_address = MAX17260::i2c_address;
};

// void MAX17260::soft_reset() {
//     MAX17260::read_field16("reset", 0xB6);
// }

bool MAX17260::check_por() {
    return (bool)MAX17260::read_field(0x00);
}

void MAX17260::configure_system() {

    float ChargeVoltage = 3.7;

    // https://pdfserv.maximintegrated.com/en/an/ug6595-modelgauge-m5-host-side-software-implementation-guide.pdf

    // Make sure POR is 1
    if (!MAX17260::check_por()) {
        while(MAX17260::read_field(0x3D) & 1) {delay(10);}
    }

    uint16_t HibCFG = MAX17260::read_field(0xBA);
    MAX17260::write_field(0x60, 0x90); // Exit Hibernate Mode step 1
    MAX17260::write_field(0xBA, (uint8_t)0x0); // Exit Hibernate Mode step 2
    MAX17260::write_field(0x60, (uint8_t)0x0); // Exit Hibernate Mode step 3

    MAX17260::write_field(0x18, DesignCap); // Write DesignCap
    MAX17260::write_field(0x1E, IchgTerm); // Write IchgTerm
    MAX17260::write_field(0x3A, VEmpty); // Write VEmpty
    if (ChargeVoltage > 4.275) {
        MAX17260::write_field(0xDB, 0x8400); // Write ModelCFG
    }
    else {
        MAX17260::write_field(0xDB, 0x8000); // Write ModelCFG
    }
    //Poll ModelCFG.Refresh(highest bit),
    //proceed to Step 3 when ModelCFG.Refresh=0.
    while (MAX17260::read_field(0xDB) & 0x8000) {delay(10);}

    MAX17260::write_field(0xBA, HibCFG); // Restore Original HibCFG value

    // Clear POR Bit
    uint16_t Status = MAX17260::read_field("Status");
    MAX17260::write_field(0x00, Status & 0xFFFD);

}

float MAX17260::read_batt_voltage() {
    return MAX17260::read_field("VCell") * MAX17260::v_per_lsb;
}

float MAX17260::read_level_percent() {
    return MAX17260::read_field(0x06) * MAX17260::per_per_lsb;
}

float MAX17260::read_level_mahrs() {
    return MAX17260::read_field(0x05) * MAX17260::mah_per_lsb;
}

float MAX17260::read_tte() {
    return MAX17260::read_field(0x11) * MAX17260::sec_per_lsb;
}

void MAX17260::read_data(bool print_data) {

    float level_percent = MAX17260::read_level_percent();
    float level_mah = MAX17260::read_level_mahrs();
    float time_to_empty = MAX17260::read_tte();

    if (!print_data) {return;}

    Serial.print("Fuel Gauge: ");
    Serial.print(level_percent);
    Serial.print("%, ");
    Serial.print(level_mah);
    Serial.print("mAh, ");
    Serial.print(time_to_empty);
    Serial.print("s");
    Serial.println();

}

uint16_t MAX17260::read_field16(String field) {
    Field field_to_write = MAX17260::field_map[field];
    return(MAX17260::read_field16(field_to_write.address, field_to_write.offset, field_to_write.length));
}

uint16_t MAX17260::read_field16(uint8_t field) {
    return(MAX17260::read_field16(field, 0, 16));
}

uint16_t MAX17260::read_field16(uint8_t field, uint8_t offset, uint8_t field_length) {

    uint8_t field_bytes[2];
    MAX17260::read_field(field, 2, field_bytes);
    uint16_t field_out = (field_bytes[1] << 8) | field_bytes[0];

    // Masking if field is not 2 byte long
    if (field_length != 16) {

        // 1 Masking
        int mask1 = pow(2, field_length) - 1;
        mask1 = mask1 << offset;
        
        // Final masking
        field_out = (field_out & mask1) >> offset;
    }
    return field_out;
}

void MAX17260::write_field16(String field, uint16_t field_val) {
    Field field_to_write = MAX17260::field_map[field];
    MAX17260::write_field16(field_to_write.address, field_val, field_to_write.offset, field_to_write.length);
}

void MAX17260::write_field16(uint8_t field, uint16_t field_val) {
    MAX17260::write_field16(field, field_val, 0, 16);
}

void MAX17260::write_field16(uint8_t field, uint16_t field_val, uint8_t offset, uint8_t field_length) {

    uint16_t curr_field_val = MAX17260::read_field16(field);

    // Zero-ing mask
    uint16_t mask1 = pow(2, field_length) - 1;
    mask1 = mask1 << offset;

    mask1 = (uint16_t)(pow(2, 16) - 1) & ~mask1;
    
    // Mask adding actual field_val
    uint16_t mask2 = field_val << offset;
    
    // Final masking
    field_val = (curr_field_val & mask1) | mask2;
    
    uint8_t field_write[2];
	field_write[0] = field_val & 0b11111111;
    field_write[1] = field_val >> 8;

    MAX17260::write_field(field, field_write);
}
