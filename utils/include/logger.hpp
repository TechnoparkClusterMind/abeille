#ifndef ABEILLE_UTILS_LOGGER_H_
#define ABEILLE_UTILS_LOGGER_H_

#include <cstdarg>
#include <string>

enum LOG_LEVEL { LOG_LEVEL_ERROR, LOG_LEVEL_WARN, LOG_LEVEL_INFO, LOG_LEVEL_TRACE, LOG_LEVEL_DEBUG };

#ifdef DEBUG
static constexpr const LOG_LEVEL LOG_LEVEL_ = LOG_LEVEL_DEBUG;
#else
static constexpr const LOG_LEVEL LOG_LEVEL_ = LOG_LEVEL_INFO;
#endif

std::string timestamp();

void LOG_(LOG_LEVEL log_level, const char *file, const char *func, const char *format, ...);

// Colors for printing
static constexpr const char *COLOR_ERROR = "\x1B[1m\x1B[31m";  // Bold Red
static constexpr const char *COLOR_WARN = "\x1B[1m\x1B[33m";   // Bold Yellow
static constexpr const char *COLOR_INFO = "\x1B[1m\x1B[36m";   // Bold Cyan
static constexpr const char *COLOR_TRACE = "\x1B[7m\x1B[1m";   // Reversed Bold
static constexpr const char *COLOR_DEBUG = "\x1B[7m\x1B[1m";   // Reversed Bold
static constexpr const char *RESET_FONT = "\x1B[0m";           // Reset font settings

// Log levels and colors
static constexpr const char *LOG_LEVEL_PREFIX[] = {"ERROR", "WARN", "INFO", "TRACE", "DEBUG"};
static constexpr const char *LOG_LEVEL_COLOR[] = {COLOR_ERROR, COLOR_WARN, COLOR_INFO, COLOR_TRACE, COLOR_DEBUG};

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG(log_level, format, ...) LOG_(log_level, __FILENAME__, __FUNCTION__, format, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) LOG(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)

#define LOG_WARN(format, ...) LOG(LOG_LEVEL_WARN, format, ##__VA_ARGS__)

#define LOG_INFO(format, ...) LOG(LOG_LEVEL_INFO, format, ##__VA_ARGS__)

#define LOG_TRACE() LOG(LOG_LEVEL_TRACE, "")

#define LOG_DEBUG(format, ...) LOG(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)

#endif  // ABEILLE_UTILS_LOGGER_H_