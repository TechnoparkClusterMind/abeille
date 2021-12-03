#include "logger.hpp"

std::string timestamp() {
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

void LOG_(LOG_LEVEL log_level, const char *file, const char *func, const char *format, ...) {
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
