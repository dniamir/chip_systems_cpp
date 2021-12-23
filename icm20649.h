# include <protocol.h>
# include <Arduino.h>
# include <arduino_i2c.h>
# include <chip.h>

struct icm20649_trim_registers {
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

class ICM20649 : public Chip {

  public:
  // Different constructors
    ICM20649(int i2c_address_in, ArduinoI2C input_protocol);
    ICM20649(ArduinoI2C input_protocol);

    // Dfault I2C address
    int i2c_address = 0b1101000;

    // WHO AM I register - register the check upon startup
    Field who_am_i_reg = Field{0x40, 0, 8, false};

    // Initialize the device
    bool initialize();

    // Put the IMU in default mode
    void default_mode();

    // Setup the different on-chip sensors
    void setup_gyro();
    void setup_accel();
    void setup_temperature();

    // Fifo functions
    void setup_fifo_6axis();
    uint16_t read_fifo_count();
    void read_fifo();

    // Read all Accel and Gyro Axes
    void read_axyz_gxyz();

    // Process data
    float process_accel(uint8_t accel_msb, uint8_t accel_lsb, int afs_fsr);
    float process_gyro(uint8_t gyro_msb, uint8_t gyro_lsb, int gfs_fsr);
    float process_temperature(uint8_t temperature_msb, uint8_t temperature_lsb);

  private:

    // Value in default WHO AM I register - value to match during startup check
    int who_am_i_val = 0xE1;

  protected:

  // Register map
  public:

  // Register Map
  std::map<String, Field> field_map {
    
    {"WHO_AM_I", Field{0x00, 0, 8, false}},
    {"USER_CTRL", Field{0x03, 0, 8, false}},
    {"DMP_EN", Field{0x03, 7, 1, false}},
    {"FIFO_EN", Field{0x03, 6, 1, false}},
    {"I2C_MST_EN", Field{0x03, 5, 1, false}},
    {"I2C_IF_DIS", Field{0x03, 4, 1, false}},
    {"DMP_RST", Field{0x03, 3, 1, false}},
    {"SRAM_RST", Field{0x03, 2, 1, false}},
    {"I2C_MST_RST", Field{0x03, 1, 1, false}},
    {"LP_CONFIG", Field{0x05, 0, 8, false}},
    {"I2C_MST_CYCLE", Field{0x05, 6, 1, false}},
    {"ACCEL_CYCLE", Field{0x05, 5, 1, false}},
    {"GYRO_CYCLE", Field{0x05, 4, 1, false}},
    {"PWR_MGMT_1", Field{0x06, 0, 8, false}},
    {"DEVICE_RESET", Field{0x06, 7, 1, false}},
    {"SLEEP", Field{0x06, 6, 1, false}},
    {"LP_EN", Field{0x06, 5, 1, false}},
    {"TEMP_DIS", Field{0x06, 3, 1, false}},
    {"CLKSEL", Field{0x06, 0, 3, false}},
    {"PWR_MGMT_2", Field{0x07, 0, 8, false}},
    {"DISABLE_ACCEL", Field{0x07, 3, 3, false}},
    {"DISABLE_GYRO", Field{0x07, 0, 3, false}},
    {"ACCEL_XOUT_H", Field{0x2D, 0, 8, false}},
    {"ACCEL_XOUT_L", Field{0x2E, 0, 8, false}},
    {"ACCEL_YOUT_H", Field{0x2F, 0, 8, false}},
    {"ACCEL_YOUT_L", Field{0x30, 0, 8, false}},
    {"ACCEL_ZOUT_H", Field{0x31, 0, 8, false}},
    {"ACCEL_ZOUT_L", Field{0x32, 0, 8, false}},
    {"GYRO_XOUT_H", Field{0x33, 0, 8, false}},
    {"GYRO_XOUT_L", Field{0x34, 0, 8, false}},
    {"GYRO_YOUT_H", Field{0x35, 0, 8, false}},
    {"GYRO_YOUT_L", Field{0x36, 0, 8, false}},
    {"GYRO_ZOUT_H", Field{0x37, 0, 8, false}},
    {"GYRO_ZOUT_L", Field{0x38, 0, 8, false}},
    {"TEMP_OUT_H", Field{0x39, 0, 8, false}},
    {"TEMP_OUT_L", Field{0x3A, 0, 8, false}},
    {"GYRO_CONFIG_1", Field{0x01, 0, 8, false}},
    {"GYRO_SMPLRT_DIV", Field{0x00, 0, 8, false}},
    {"GYRO_DLPFCFG", Field{0x01, 3, 3, false}},
    {"GYRO_FS_SEL", Field{0x01, 1, 2, false}},
    {"GYRO_FCHOICE", Field{0x01, 0, 1, false}},
    {"GYRO_CONFIG_2", Field{0x02, 0, 8, false}},
    {"XGYRO_CTEN", Field{0x02, 5, 1, false}},
    {"YGYRO_CTEN", Field{0x02, 4, 1, false}},
    {"ZGYRO_CTEN", Field{0x02, 3, 1, false}},
    {"GYRO_AVGCFG", Field{0x02, 0, 3, false}},
    {"ACCEL_SMPLRT_DIV_1", Field{0x10, 0, 3, false}},
    {"ACCEL_SMPLRT_DIV_2", Field{0x11, 0, 8, false}},
    {"ACCEL_CONFIG", Field{0x14, 0, 8, false}},
    {"ACCEL_DLPFCFG", Field{0x14, 3, 3, false}},
    {"ACCEL_FS_SEL", Field{0x14, 1, 2, false}},
    {"ACCEL_FCHOICE", Field{0x14, 0, 1, false}},
    {"ACCEL_CONFIG_2", Field{0x15, 0, 8, false}},
    {"AX_ST_EN_REG", Field{0x15, 4, 1, false}},
    {"AY_ST_EN_REG", Field{0x15, 3, 1, false}},
    {"AZ_ST_EN_REG", Field{0x15, 2, 1, false}},
    {"DEC3_CFG", Field{0x15, 0, 2, false}},
    {"TEMP_CONFIG", Field{0x53, 0, 8, false}},
    {"TEMP_DLPFCFG", Field{0x53, 0, 3, false}},
    {"REG_BANK_SEL", Field{0x7F, 0, 8, false}},
    {"USER_BANK", Field{0x7F, 4, 2, false}},

    // FIFO
    {"FIFO_EN_2", Field{0x67, 0, 8, false}},
    {"ACCEL_FIFO_EN", Field{0x67, 4, 1, false}},
    {"GYRO_Z_FIFO_EN", Field{0x67, 3, 1, false}},
    {"GYRO_Y_FIFO_EN", Field{0x67, 2, 1, false}},
    {"GYRO_X_FIFO_EN", Field{0x67, 1, 1, false}},
    {"TEMP_FIFO_EN", Field{0x67, 0, 1, false}},
    {"FIFO_RST", Field{0x68, 0, 8, false}},
    {"FIFO_RESET", Field{0x68, 0, 4, false}},
    {"FIFO_MODE", Field{0x69, 0, 4, false}},
    {"FIFO_COUNTH", Field{0x70, 0, 4, false}},
    {"FIFO_COUNTL", Field{0x71, 0, 8, false}},
    {"FIFO_R_W", Field{0x72, 0, 8, false}},
    {"WOF_STATUS", Field{0x74, 8, 1, false}},
    {"RAW_DATA_RDY", Field{0x74, 0, 4, false}},
    {"FIFO_CFG", Field{0x76, 0, 1, false}},

  };

};