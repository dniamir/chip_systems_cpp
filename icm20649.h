# include <protocol.h>
# include <arduino_i2c.h>

class ICM20649 {

  public:
    ICM20649(ArduinoI2C &comm_protocol);
    ArduinoI2C comm_protocol;
    /**
        \brief initialze device

    */
    bool initialize(void);

};