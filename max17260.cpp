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

void MAX17260::soft_reset() {
    // Requires 45s of wait
     MAX17260::write_field16("HibCfg", 0);
     MAX17260::write_field16("SHDN", 1);
}

bool MAX17260::check_por() {
    return (bool)MAX17260::read_field16("POR");
}

void MAX17260::configure_system() {

    uint16_t DesignCap = 420;  // mAh
    uint16_t IchgTerm = 420;  // mA
    uint16_t VEmpty = 3.0;   // Minimum Voltage
    float VRecovery = 3.7;
    float ChargeVoltage = 4.2;

    // Scale factor for VEmpty
    uint16_t VE = uint16_t(VEmpty / MAX17260::v_empty_per_lsb);  // Empty Voltage - resolution is 10mV / LSB
    uint16_t VR = uint16_t(VRecovery / MAX17260::v_recovery_per_lsb);  // Recovery Voltage (voltage for clearing empty detection) - resolution is 40mV / LSB
    VEmpty = (VE << 7) + VR;

    // Scale factor for IchgTerm
    IchgTerm = uint16_t(IchgTerm / MAX17260::i_term_per_lsb);

    // Make sure POR is 1
    if (MAX17260::read_field16("POR")) {
    
        while(MAX17260::read_field16("FStat") & 1) {delay(10);}
        
        
        uint16_t HibCFG = MAX17260::read_field16("HibCfg");
        MAX17260::write_field16(0x60, 0x90); // Exit Hibernate Mode step 1
        MAX17260::write_field16("HibCfg", 0x0); // Exit Hibernate Mode step 2
        MAX17260::write_field16(0x60, 0x0); // Exit Hibernate Mode step 3

        MAX17260::write_field16("DesignCap", DesignCap); // Write DesignCap
        MAX17260::write_field16("IChgTerm", IchgTerm); // Write IchgTerm
        MAX17260::write_field16("VEmpty", VEmpty); // Write VEmpty
        if (ChargeVoltage > 4.275) {
            MAX17260::write_field16("ModelCFG", 0x8400); // Write ModelCFG
        }
        else {
            MAX17260::write_field16("ModelCFG", 0x8000); // Write ModelCFG
        }
        //Poll ModelCFG.Refresh(highest bit),
        //proceed to Step 3 when ModelCFG.Refresh=0.
        while (MAX17260::read_field16("Refresh")) {delay(10);}

        MAX17260::write_field16("HibCfg", HibCFG); // Restore Original HibCFG value

        // Clear POR Bit
        MAX17260::write_field16("POR", 0);

    }
}

float MAX17260::read_batt_voltage() {
    return MAX17260::read_field16("VCell") * MAX17260::v_per_lsb;
}

float MAX17260::read_level_percent() {
    return MAX17260::read_field16("RepSOC") * MAX17260::per_per_lsb;
}

float MAX17260::read_level_mahrs() {
    return MAX17260::read_field16("RepCap") * MAX17260::mah_per_lsb;
}

float MAX17260::read_tte() {
    return MAX17260::read_field16("TTE") * MAX17260::sec_per_lsb;
}

void MAX17260::read_data(bool print_data) {

    float level_percent = MAX17260::read_level_percent();
    float level_mah = MAX17260::read_level_mahrs();
    float time_to_empty = MAX17260::read_tte();
    float batt_voltage = MAX17260::read_batt_voltage();

    if (!print_data) {return;}

    Serial.print("Fuel Gauge: ");
    Serial.print(level_percent);
    Serial.print("%, ");
    Serial.print(level_mah);
    Serial.print("mAh, ");
    Serial.print(time_to_empty);
    Serial.print("s, ");
    Serial.print(batt_voltage);
    Serial.print("V");
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
