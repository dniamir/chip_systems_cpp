# include <chip.h>
# include <arduino_i2c.h>

Chip::Chip(ArduinoI2C protocol_in) {

    // static Protocol comm_protocol = comm_protocol;
    comm_protocol = protocol_in;

};

Chip::Chip(int i2c_address_in, ArduinoI2C protocol_in) {

    comm_protocol = protocol_in;
    i2c_address = i2c_address_in;

};

void Chip::write_field(String field, int field_val) {

    // Check register map for register
    Field field_to_write = field_map[field];
    write_field(field_to_write.address, field_val, field_to_write.offset, field_to_write.length);
}

void Chip::write_field(int field, int field_val, int offset, int field_length) {

    // Serial.println();
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

        // Zero-ing mask
        int mask1 = pow(2, field_length) - 1;
        mask1 = mask1 << offset;
        mask1 = 0b11111111 - mask1;

        // Mask adding actual field_val
        int mask2 = field_val << offset;
        
        // Final masking
        field_val = (curr_field_val & mask1) | mask2;
    }

    write_field(field, field_val);
}

void Chip::write_field(int field, int field_val) {

    comm_protocol.write_register(i2c_address, field, field_val);
}

int Chip::read_field(String field) {
    Field field_to_write = field_map[field];
    return(read_field(field_to_write.address, field_to_write.offset, field_to_write.length));
}

int Chip::read_field(int field, int offset, int field_length) {

    int field_out = read_field(field);

    // Do masking if this is a true field and not a register
    if (field_length != 8) {

        // 1 Masking
        int mask1 = pow(2, field_length) - 1;
        mask1 = mask1 << offset;
        
        // Final masking
        field_out = (field_out & mask1) >> offset;
    }

    return field_out;

}

int Chip::read_field(int field) {

    return(comm_protocol.read_register(i2c_address, field));
}

void Chip::read_field(int field, int bytes_to_read, int8_t field_out[]) {
    comm_protocol.read_register(i2c_address, field, bytes_to_read, field_out);
}
