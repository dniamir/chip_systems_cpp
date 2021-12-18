# include <arduino_i2c.h>
# include <map>

struct Field{
  unsigned int address;
  unsigned char offset = 0;
  unsigned char length = 8; 
  bool twos_comp = false;
};

class Chip {   
    // Data members of class
public:
    Chip(ArduinoI2C protocol_in);
    Chip(int i2c_address_in, ArduinoI2C protocol_in);

    // I2C Address
    int i2c_address;

    // Register / Field MAP
    std::map<String, Field> field_map;

    // WHO AM I register - register the check upon startup
    Field who_am_i_reg;

    // Communication protocol
    ArduinoI2C comm_protocol; 

    // Overloading write_field
    void write_field(String field, int field_val);
    void write_field(int field, int field_val, int offset, int field_length);
    void write_field(int field, int field_val);

    // Overloading read_field
    int read_field(String field);
    int read_field(int field, int offset, int field_length);
    int read_field(int field);
    void read_field(int field, int bytes_to_read, int8_t field_out[]);

};