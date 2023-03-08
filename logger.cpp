# include <logger.h>
# include <SD.h>
# include <Arduino.h>
# include <stdio.h>

LOGGER::LOGGER() {}

LOGGER::LOGGER(const char *log_filename, const int sd_card) {
    create_usd_log(log_filename, sd_card);
}

void LOGGER::create_usd_log(const char *log_filename_input, const int sd_card) {

    // Initialize SD writing - This is what works for a teensy
    // const int chipSelect = BUILTIN_SDCARD;

    // Initialize SD card slot
    Serial.print("Initializing SD card...");
    
    // see if the card is present and can be initialized:
    if (!SD.begin(sd_card)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("1. is a card inserted?");
        Serial.println("2. is your wiring correct?");
        Serial.println("3. did you change the chipSelect pin to match your shield or module?");
        Serial.println("Note: press reset or reopen this Serial Monitor after fixing your issue!");
        while (true);
    }
    Serial.println("card initialized.");

    LOGGER::log_file = SD.open(log_filename_input, FILE_WRITE);
    delay(500);

    log_file.println("Timestamp,Type,Value");
    Serial.println("Filename is");
    Serial.println(log_filename_input);
    // log_file.close();

}

void LOGGER::close_log() {
    LOGGER::log_file.close();
}

void LOGGER::open_log() {
     LOGGER::log_file = SD.open(log_filename, FILE_WRITE);
}

void LOGGER::flush_log() {
     LOGGER::log_file.flush();
}

// void LOGGER::write_to_log(int timestamp, const char log_type[4], const char* log_value) {

//     char log_value_char [40];
    
//     sprintf(log_value_char, "%i,%s,%s", timestamp, log_type, log_value);
//     LOGGER::log_file.println(log_value_char);

// }

void LOGGER::write_to_log(uint32_t timestamp, const std::string &log_type, const std::string &log_value) {
    char log_value_char[40] = "";
    snprintf(log_value_char, 40, "LOG: %i :: %s :: %s", timestamp, log_type.c_str(), log_value.c_str());
    Serial.println(log_value_char);
}

void LOGGER::write_to_log(uint32_t timestamp, const std::string &log_type, const int32_t &log_value) {
    char log_value_char[10] = "";
    snprintf (log_value_char, 10, "%i", log_value);
    LOGGER::write_to_log(timestamp, log_type, log_value_char);
}

// void LOGGER::write_to_log(int timestamp, const char log_type[4], const std::string log_value) {
//     LOGGER::write_to_log(timestamp, log_type, log_value.c_str());
// }

void LOGGER::write_to_log(const std::string &log_type, const int32_t &log_value) {
    LOGGER::write_to_log((uint32_t)millis(), log_type, log_value);
}

void LOGGER::write_to_log(const std::string &log_type, const int16_t &log_value) {
    LOGGER::write_to_log((uint32_t)millis(), log_type, (int32_t&)log_value);
}

void LOGGER::write_to_log(const std::string &log_type, const int8_t &log_value) {
    LOGGER::write_to_log((uint32_t)millis(), log_type, (int32_t&)log_value);
}

void LOGGER::write_to_log(const std::string &log_type, const uint32_t &log_value) {
    LOGGER::write_to_log((uint32_t)millis(), log_type, (int32_t&)log_value);
}

void LOGGER::write_to_log(const std::string &log_type, const uint16_t &log_value) {
    LOGGER::write_to_log((uint32_t)millis(), log_type, (int32_t&)log_value);
}

void LOGGER::write_to_log(const std::string &log_type, const uint8_t &log_value) {
    LOGGER::write_to_log((uint32_t)millis(), log_type, (int32_t&)log_value);
}

void LOGGER::write_to_log(const std::string &log_type, const std::string &log_value) {
    LOGGER::write_to_log((uint32_t)millis(), log_type, log_value);
}

void LOGGER::write_to_log(const std::string &log_type, const char *log_value) {
    LOGGER::write_to_log((uint32_t)millis(), log_type, log_value);
}

void LOGGER::write_to_log(const std::string &log_type, const bool &log_value) {
    if(log_value) {
        LOGGER::write_to_log((uint32_t)millis(), log_type, (int8_t)1);
    }
    else {
        LOGGER::write_to_log((uint32_t)millis(), log_type, (int8_t)0);
    }
}

// void LOGGER::write_to_log(std::vector<int> timestamps, const char log_type[4], std::vector<int32_t> log_values) {

//     for (std::size_t i = 0; i != log_values.size(); ++i) {
//         LOGGER::write_to_log(timestamps[i], log_type, log_values[i]);
//     }      
// }   