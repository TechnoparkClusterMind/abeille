#ifndef ABEILLE_UTILS_LOGGER_H_
#define ABEILLE_UTILS_LOGGER_H_

#include <cstdarg>
#include <ctime>
#include <iostream>
#include <string>

#define DEBUG

enum LOG_LEVEL { LOG_LEVEL_ERROR, LOG_LEVEL_WARN, LOG_LEVEL_INFO, LOG_LEVEL_TRACE, LOG_LEVEL_DEBUG };

#ifdef DEBUG
static LOG_LEVEL LOG_LEVEL_ = LOG_LEVEL_DEBUG;
#else
static LOG_LEVEL LOG_LEVEL_ = LOG_LEVEL_INFO;
#endif

// Colors for printing
static constexpr const char *COLOR_NORM = "\x1B[1m\x1B[0m";     // Bold Normal
static constexpr const char *COLOR_ERROR = "\x1B[1m\x1B[31m";   // Bold Red
static constexpr const char *COLOR_WARN = "\x1B[1m\x1B[33m";    // Bold Yellow
static constexpr const char *COLOR_INFO = "\x1B[1m\x1B[36m";    // Bold Cyan
static constexpr const char *COLOR_TRACE = "\x1B[1m\x1B[35m";   // Bold Magenta
static constexpr const char *COLOR_DEBUG = "\x1B[1m\x1B[34m";   // Bold Blue
static constexpr const char *RESET_COLOR = "\x1B[39m\x1B[22m";  // Reset color and font weight

// Log levels and colors
static constexpr const char *LOG_LEVEL_PREFIX[] = {"ERROR", "WARN", "INFO", "TRACE", "DEBUG"};
static constexpr const char *LOG_LEVEL_COLOR[] = {COLOR_ERROR, COLOR_WARN, COLOR_INFO, COLOR_TRACE, COLOR_DEBUG};

static std::string timestamp() noexcept {
  std::time_t now = std::time(nullptr);

  struct tm *tm;
  tm = localtime(&now);
  std::string s, m, h, D, M, Y;
  s = std::to_string(tm->tm_sec);
  m = std::to_string(tm->tm_min);
  h = std::to_string(tm->tm_hour);
  D = std::to_string(tm->tm_mday);
  M = std::to_string(tm->tm_mon + 1);
  Y = std::to_string(tm->tm_year + 1900);

  if (tm->tm_sec < 10) s = "0" + s;
  if (tm->tm_min < 10) m = "0" + m;
  if (tm->tm_hour < 10) h = "0" + h;
  if (tm->tm_mday < 10) D = "0" + D;
  if (tm->tm_mon + 1 < 10) M = "0" + M;

  std::string ret = '[' + Y + '-' + M + '-' + D + 'T' + h + ':' + m + ':' + s + ']';

  return ret;
}

static void LOG_(LOG_LEVEL log_level, const char *file, const char *func, const char *format, ...) {
  if (log_level > LOG_LEVEL_) {
    return;
  }

  va_list args;
  va_start(args, format);
  char message[512];
  vsnprintf(message, 512, format, args);
  va_end(args);

  std::cout << LOG_LEVEL_COLOR[log_level] << timestamp() << '[' << LOG_LEVEL_PREFIX[log_level] << ']';
  std::cout << '[' << file << ':' << func << ']';
  if (log_level != LOG_LEVEL_TRACE) {
    std::cout << ": " << message;
  }
  std::cout << RESET_COLOR << std::endl;
}

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define LOG(log_level, format, ...) LOG_(log_level, __FILENAME__, __FUNCTION__, format, ##__VA_ARGS__)

#define LOG_ERROR(format, ...) LOG(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)

#define LOG_WARN(format, ...) LOG(LOG_LEVEL_WARN, format, ##__VA_ARGS__)

#define LOG_INFO(format, ...) LOG(LOG_LEVEL_INFO, format, ##__VA_ARGS__)

#define LOG_TRACE() LOG(LOG_LEVEL_TRACE, "")

#define LOG_DEBUG(format, ...) LOG(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)

#endif  // ABEILLE_UTILS_LOGGER_H_