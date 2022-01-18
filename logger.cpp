# include <logger.h>
# include "SD.h"
# include "Arduino.h"
# include "stdio.h"

LOGGER::LOGGER() {}

LOGGER::LOGGER(const char *log_filename) {
    create_log(log_filename);
}

void LOGGER::create_log(const char *log_filename_input) {

    log_filename = log_filename_input;

    // Initialize SD writing
    const int chipSelect = BUILTIN_SDCARD;

    // Initialize SD card slot
    Serial.print("Initializing SD card...");
    
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("1. is a card inserted?");
        Serial.println("2. is your wiring correct?");
        Serial.println("3. did you change the chipSelect pin to match your shield or module?");
        Serial.println("Note: press reset or reopen this Serial Monitor after fixing your issue!");
        while (true);
    }
    Serial.println("card initialized.");

    log_file = SD.open(log_filename, FILE_WRITE);
    delay(500);

    log_file.println("Timestamp,Type,Value");
    Serial.println("Filename is");
    Serial.println(log_filename);
    // log_file.close();

}

void LOGGER::close_log() {
    log_file.close();
}

void LOGGER::open_log() {
     log_file = SD.open(log_filename, FILE_WRITE);
}

void LOGGER::flush_log() {
     log_file.flush();
}

void LOGGER::write_to_log(int timestamp, const char log_type[4], const char* log_value) {

    char log_value_char [40];
    sprintf (log_value_char, "%i,%s,%s", timestamp, log_type, log_value);

    log_file.println(log_value_char);

}

void LOGGER::write_to_log(int timestamp, const char log_type[4], int log_value) {
    char log_value_char [10];
    sprintf (log_value_char, "%i", log_value);
    write_to_log(timestamp, log_type, log_value_char);
}

void LOGGER::write_to_log(int timestamp, const char log_type[4], float log_value) {
    char log_value_char [10];
    sprintf (log_value_char, "%.4f", log_value);
    write_to_log(timestamp, log_type, log_value_char);
}

void LOGGER::write_to_log(const char log_type[4], const char* log_value) {
    write_to_log(millis(), log_type, log_value);
}

void LOGGER::write_to_log(const char log_type[4], int log_value) {
    write_to_log(millis(), log_type, log_value);
}

void LOGGER::write_to_log(const char log_type[4], float log_value) {
    write_to_log(millis(), log_type, log_value);
}

void LOGGER::write_to_log(std::vector<int> timestamps, const char log_type[4], std::vector<float> log_values) {

    for (std::size_t i = 0; i != log_values.size(); ++i) {
        write_to_log(timestamps[i], log_type, log_values[i]);
    }      
}

void LOGGER::write_to_log(std::vector<int> timestamps, const char log_type[4], std::vector<int16_t> log_values) {

    for (std::size_t i = 0; i != log_values.size(); ++i) {
        write_to_log(timestamps[i], log_type, (int)log_values[i]);
    }      
}   