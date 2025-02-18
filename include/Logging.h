#pragma once

// Using spdlog's daily file logging
#include <sstream>
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/spdlog.h"

extern bool g_isDebugEnabled;

class LoggerStream {
public:
    LoggerStream(spdlog::level::level_enum level)
        : m_logLevel(level) {}

    template <typename T>
    LoggerStream& operator<<(const T& msg) {
        m_buffer << msg;
        return *this;
    }

    ~LoggerStream() {
        auto logger = spdlog::get("main_logger");
        if (logger && g_isDebugEnabled) {
            logger->log(m_logLevel, m_buffer.str());
            logger->flush(); // For real-time logging
        }
    }

private:
    std::ostringstream m_buffer;
    spdlog::level::level_enum m_logLevel;
};

#define LOG(_level) LoggerStream(spdlog::level::level_enum::_level)

class Logger {
public:
    static void init(const std::string& filename = "logfile.log") {
        if (!spdlog::get("main_logger")) {
            auto logger = spdlog::daily_logger_mt("main_logger", filename, 0, 0);
            logger->set_level(spdlog::level::trace);
            spdlog::set_pattern("[%T]-[%-8l]->> %v");

            spdlog::set_default_logger(logger);
            spdlog::info(">>>>>>>>>> LOG START <<<<<<<<<<");
        }
    }
};
