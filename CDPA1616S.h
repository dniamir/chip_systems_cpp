/** Includes */
# include <Adafruit_GPS.h>
#define GPSSerial Serial1

class CDPA1616S {

  public:
    CDPA1616S();

    bool initialize(void);

    bool readGPS(void);

    void printBlock(void);

    // Connect to the GPS on the hardware port
    Adafruit_GPS gps;

};