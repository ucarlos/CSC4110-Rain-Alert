/*
 * ------------------------------------------------------------------------------
 * Created by Ulysses Carlos on 11/22/2021 at 03:37 PM
 *
 * DebugLog.h
 *
 * ------------------------------------------------------------------------------
 */

#ifndef CSC4110_PROJECT_DEBUGLOG_H
#define CSC4110_PROJECT_DEBUGLOG_H

#include <fstream>
#include <mutex>
#include <unordered_map>

enum class DebugLevel { INFO = 0, DEBUG, WARNING, ERROR };


class DebugLog final: public std::ofstream {
public:
	DebugLog(): std::ofstream{default_debug_path, std::ios_base::out} { }
	explicit DebugLog(const std::string &file_path): std::ofstream{file_path, std::ios_base::out} { }
    static DebugLog& instance();
	void write_log(const DebugLevel &debug_level, const std::string &string);

private:
    std::mutex debuglog_mutex{};
    const static inline std::string default_debug_path{"../Debug.txt"};
};

std::ostream& operator<<(std::ostream &os, const DebugLevel &debuglevel);

#endif //CSC4110_PROJECT_DEBUGLOG_H
