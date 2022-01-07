# include <logger.h>
# include "SD.h"
# include "Arduino.h"
# include "stdio.h"

LOGGER::LOGGER() {}

void LOGGER::create_log() {

    // Initialize SD writing
    const int chipSelect = BUILTIN_SDCARD;

    // Initialize SD card slot
    Serial.print("Initializing SD card...");
    
    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    Serial.println("card initialized.");

    log_file = SD.open(filename, FILE_WRITE);
    log_file.println("Timestamp,Type,Value");
    Serial.println("Filename is");
    Serial.println(filename);
    log_file.close();

}

void LOGGER::write_to_log(int timestamp, char* log_type, char* log_value) {

    bool close_log = false;
    if (!log_file) { log_file = SD.open(filename, FILE_WRITE); close_log = true;}

    char log_value_char [40];
    sprintf (log_value_char, "%i,%s,%s", timestamp, log_type, log_value);

    log_file.println(log_value_char);

    if (close_log) { log_file.close(); }

}

void LOGGER::write_to_log(int timestamp, char* log_type, int log_value) {
    char log_value_char [10];
    sprintf (log_value_char, "%i", log_value);
    write_to_log(timestamp, log_type, log_value_char);
}

void LOGGER::write_to_log(int timestamp, char* log_type, float log_value) {
    char log_value_char [10];
    sprintf (log_value_char, "%.4f", log_value);
    write_to_log(timestamp, log_type, log_value_char);
}

void LOGGER::write_to_log(char* log_type, char* log_value) {
    write_to_log(millis(), log_type, log_value);
}

void LOGGER::write_to_log(char* log_type, int log_value) {
    write_to_log(millis(), log_type, log_value);
}

void LOGGER::write_to_log(char* log_type, float log_value) {
    write_to_log(millis(), log_type, log_value);
}

void LOGGER::write_to_log(std::vector<int> timestamps, char* log_type, std::vector<float> log_values) {

    bool close_log = false;
    if (!log_file) { log_file = SD.open(filename, FILE_WRITE); close_log = true;}

    for (std::size_t i = 0; i != log_values.size(); ++i) {
        write_to_log(timestamps[i], log_type, log_values[i]);
    }      

    if (close_log) { log_file.close(); }
}

void LOGGER::write_to_log(std::vector<int> timestamps, char* log_type, std::vector<int16_t> log_values) {

    bool close_log = false;
    if (!log_file) { log_file = SD.open(filename, FILE_WRITE); close_log = true;}

    for (std::size_t i = 0; i != log_values.size(); ++i) {
        write_to_log(timestamps[i], log_type, (int)log_values[i]);
    }      

    if (close_log) { log_file.close(); }
}   