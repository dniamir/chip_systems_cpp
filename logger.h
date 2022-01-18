# include <vector>
# include "SD.h"

class LOGGER {

  public:
   // Constructors
    LOGGER(const char *log_filename_input);
    LOGGER();

    void create_log();
    void create_log(const char *log_filename_input);

    // Overloading log write functions
    void write_to_log(int timestamp, const char log_type[4], const char* log_value);
    void write_to_log(int timestamp, const char log_type[4], int log_value);
    void write_to_log(int timestamp, const char log_type[4], float log_value);

    void write_to_log(const char log_type[4], const char* log_value);
    void write_to_log(const char log_type[4], int log_value);
    void write_to_log(const char log_type[4], float log_value);

    void write_to_log(std::vector<int> timestamps, const char log_type[4], std::vector<float> log_values);
    void write_to_log(std::vector<int> timestamps, const char log_type[4], std::vector<int16_t> log_values);

    void close_log();
    void open_log();
    void flush_log();

    const char * log_filename;

  private:

    File log_file;

};