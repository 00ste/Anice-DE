#include "AUtil.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <algorithm>


const char* ALogger::tagToString(ALogger::Tag tag) {
    switch(tag) {
        case INFO: return "[INFO]";
        case DEBUG: return "[DEBUG]";
        case WARNING: return "[WARNING]";
        case ERROR: return "[ERROR]";
        default: return "";
    }
}

void ALogger::logMessageToFile(ALogger::Tag tag, const std::string& message) {
    std::ofstream file;
    file.open(LOG_FILE_PATH, std::ios::app);
    time_t now;
    time(&now);
    std::string time_string = ctime(&now);
    file << time_string << " " << ALogger::tagToString(tag) << ": " << message << std::endl;
    file.close();
}

