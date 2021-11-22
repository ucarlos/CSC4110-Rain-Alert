/*
 * ------------------------------------------------------------------------------
 * Created by Ulysses Carlos on 11/22/2021 at 03:37 PM
 *
 * DebugLog.cpp
 *
 * ------------------------------------------------------------------------------
 */

#include "DebugLog.h"

DebugLog& DebugLog::instance() {
    static DebugLog log;
    return log;
}

void DebugLog::write_log(const DebugLevel &debug_level, const std::string &string) {
    std::lock_guard<std::mutex> lock{debuglog_mutex};
    *this << debug_level << ": " << string << std::endl;
}

std::ostream& operator<<(std::ostream &os, const DebugLevel &debuglevel) {
    const static std::unordered_map<DebugLevel, std::string> log_level = {{DebugLevel::INFO, "[INFO]  "},
                                                                                 {DebugLevel::DEBUG, "[DEBUG] "},
                                                                                 {DebugLevel::WARNING, "[WARNING]"},
                                                                                 {DebugLevel::ERROR, "[ERROR] "}};

    return os << log_level.at(debuglevel);
}