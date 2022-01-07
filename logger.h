# include <vector>
# include "SD.h"

class LOGGER {

  public:
   // Constructors
    LOGGER(const char *filename);
    LOGGER();

    void create_log();

    // Overloading log write functions
    void write_to_log(int timestamp, char* log_type, char* log_value);
    void write_to_log(int timestamp, char* log_type, int log_value);
    void write_to_log(int timestamp, char* log_type, float log_value);

    void write_to_log(char* log_type, char* log_value);
    void write_to_log(char* log_type, int log_value);
    void write_to_log(char* log_type, float log_value);

    void write_to_log(std::vector<int> timestamps, char* log_type, std::vector<float> log_values);
    void write_to_log(std::vector<int> timestamps, char* log_type, std::vector<int16_t> log_values);

  private:

    File log_file;
    const char * filename = "DarienTest13.txt";

};