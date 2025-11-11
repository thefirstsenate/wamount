#ifndef LOGGING_H
#define LOGGING_H

#include <iostream>
#include <sstream>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static void setLevel(LogLevel level) {
        min_level_ = level;
    }

    static void debug(const std::string& msg) {
        log(LogLevel::DEBUG, msg);
    }

    static void info(const std::string& msg) {
        log(LogLevel::INFO, msg);
    }

    static void warning(const std::string& msg) {
        log(LogLevel::WARNING, msg);
    }

    static void error(const std::string& msg) {
        log(LogLevel::ERROR, msg);
    }

private:
    static void log(LogLevel level, const std::string& msg) {
        if (level < min_level_) return;

        const char* prefix = "";
        switch (level) {
            case LogLevel::DEBUG:   prefix = "[DEBUG] "; break;
            case LogLevel::INFO:    prefix = "[INFO]  "; break;
            case LogLevel::WARNING: prefix = "[WARN]  "; break;
            case LogLevel::ERROR:   prefix = "[ERROR] "; break;
        }

        std::cerr << prefix << msg << std::endl;
    }

    static LogLevel min_level_;
};

#endif // LOGGING_H
