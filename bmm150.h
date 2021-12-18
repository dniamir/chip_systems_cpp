# include <protocol.h>
# include <Arduino.h>
# include <arduino_i2c.h>
# include <register_map.h>

struct bmm150_trim_registers {
      int8_t dig_x1;
      int8_t dig_y1;
      int8_t dig_x2;
      int8_t dig_y2;
      uint16_t dig_z1;
      int16_t dig_z2;
      int16_t dig_z3;
      int16_t dig_z4;
      uint8_t dig_xy1;
      int8_t dig_xy2;
      uint16_t dig_xyz1;
    };

class BMM150 {

  public:
  // Different constructors
    BMM150(ArduinoI2C input_protocol, int i2c_address_in, int who_am_i_reg_in);
    BMM150(ArduinoI2C input_protocol, int i2c_address_in, String who_am_i_reg_in);
    BMM150(ArduinoI2C input_protocol, int i2c_address_in, Field who_am_i_reg_in);
    BMM150(ArduinoI2C input_protocol, int i2c_address_in);
    BMM150(ArduinoI2C input_protocol);
    BMM150();

    // Dfault I2C address
    int i2c_address = 0x10;

    // WHO AM I register - register the check upon startup
    Field who_am_i_reg = Field{0x40, 0, 8, false};

    ArduinoI2C comm_protocol;

    // Initialize the device
    bool initialize(void);

    // Put the BMM150 in default mode
    // 10Hz, 9 XY repititions, 15 Z repitions
    void default_mode(void);

    // Read all Magnetometer Axes
    void read_mxyz();

    // Write value to field
    void write_field(int field, int field_val);
    void write_field(int field, int field_val, int offset, int length);
    void write_field(String field, int field_val);
 
    int read_field(int field);
    // int read_field(int field, int bytes_to_read, int &field_out);
    int read_field(int field, int offset, int length);
    // int read_field(int field, int offset, int length, int bytes_to_read);
    int read_field(String field);
    int read_field(String field, int bytes_to_read);

    // Register Map
    std::map<String, Field> field_map {
      {"Chip ID",             Field{0x40, 0, 8, false}},
      {"DATA X LSB",          Field{0x42, 3, 5, false}},
      {"REPZ",                Field{0x52, 0, 8, false}},
      {"Power Control Bit",   Field{0x4B, 0, 1, false}},
      {"Opmode",              Field{0x4C, 1, 2, false}},
      {"Data Rate",           Field{0x4C, 3, 3, false}},
      {"Channel X",           Field{0x4E, 3, 1, false}},
      {"Channel Y",           Field{0x4E, 4, 1, false}},
      {"Channel Z",           Field{0x4E, 5, 1, false}},
      {"REPXY",               Field{0x51, 0, 8, false}},
      {"BMM150_DIG_X1",       Field{0x5D, 0, 8, false}},
      {"BMM150_DIG_Y1",       Field{0x5E, 0, 8, false}},
      {"BMM150_DIG_Z4_LSB",   Field{0x62, 0, 8, false}},
      {"BMM150_DIG_Z4_MSB",   Field{0x63, 0, 8, false}},
      {"BMM150_DIG_X2",       Field{0x64, 0, 8, false}},
      {"BMM150_DIG_Y2",       Field{0x65, 0, 8, false}},
      {"BMM150_DIG_Z2_LSB",   Field{0x68, 0, 8, false}},
      {"BMM150_DIG_Z2_MSB",   Field{0x69, 0, 8, false}},
      {"BMM150_DIG_Z1_LSB",   Field{0x6A, 0, 8, false}},
      {"BMM150_DIG_Z1_MSB",   Field{0x6B, 0, 8, false}},
      {"BMM150_DIG_XYZ1_LSB", Field{0x6C, 0, 8, false}},
      {"BMM150_DIG_XYZ1_MSB", Field{0x6D, 0, 8, false}},
      {"BMM150_DIG_Z3_LSB",   Field{0x6E, 0, 8, false}},
      {"BMM150_DIG_Z3_MSB",   Field{0x6F, 0, 8, false}},
      {"BMM150_DIG_XY2",      Field{0x70, 0, 8, false}},
      {"BMM150_DIG_XY1",      Field{0x71, 0, 8, false}},
    };

  private:

    // Value in default WHO AM I register - value to match during startup check
    int who_am_i_val = 0x32;

    // Function to read all trim registers
    void read_trim_registers();

    // Trim registers
    struct bmm150_trim_registers trim_data;

    // Compensate data
    int16_t compensate_x(int16_t mx, uint16_t rhall);
    int16_t compensate_y(int16_t my, uint16_t rhall);
    int16_t compensate_z(int16_t mz, uint16_t rhall);

};