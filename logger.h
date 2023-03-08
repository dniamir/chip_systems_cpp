# include <vector>
# include "SD.h"

class LOGGER {

  public:
   // Constructors
    LOGGER(const char *log_filename_input, const int sd_card);
    LOGGER();

    void create_usd_log(const char *log_filename_input, const int sd_card);

    bool usd_log = false;

    // Overloading log write functions
    // static void write_to_log(int timestamp, const char log_type[4], const char* log_value);
    // static void write_to_log(int timestamp, const char log_type[4], const char* log_value);
    static void write_to_log(uint32_t timestamp, const std::string &log_type, const std::string &log_value);
    static void write_to_log(uint32_t timestamp, const std::string &log_type, const int32_t &log_value);
    static void write_to_log(uint32_t timestamp, const std::string &log_type, const int16_t &log_value);

    // static void write_to_log(const char log_type[4], const char* log_value);
    static void write_to_log(const std::string &log_type, const std::string &log_value);
    static void write_to_log(const std::string &log_type, const int32_t &log_value);
    static void write_to_log(const std::string &log_type, const int16_t &log_value);
    static void write_to_log(const std::string &log_type, const int8_t &log_value);
    static void write_to_log(const std::string &log_type, const uint32_t &log_value);
    static void write_to_log(const std::string &log_type, const uint16_t &log_value);
    static void write_to_log(const std::string &log_type, const uint8_t &log_value);
    static void write_to_log(const std::string &log_type, const char *log_value);
    static void write_to_log(const std::string &log_type, const bool &log_value);

    // static void write_to_log(std::vector<int> timestamps, const char log_type[4], std::vector<int32_t> log_values);

    void close_log();
    void open_log();
    void flush_log();

    const char * log_filename;

  private:

    File log_file;

};