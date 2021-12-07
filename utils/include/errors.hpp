#ifndef ABEILLE_ERRORS_H_
#define ABEILLE_ERRORS_H_

#include <string>

class error {
 public:
  enum class Code { OK, UNIMPLEMENTED, FAILURE };

  error() = default;
  explicit error(Code code) noexcept : code_(code) {}
  error(const std::string& message, Code code = Code::FAILURE) noexcept : code_(code), message_(message) {}
  ~error() = default;

  error& operator=(const error& other) = default;

  Code code() const noexcept { return code_; }
  std::string what() const noexcept { return message_; }

  bool ok() const noexcept { return code_ == Code::OK; }
  bool is(const error& other) const noexcept { return code_ == other.code_; }

 private:
  Code code_ = Code::OK;
  std::string message_;
};

#endif  // ABEILLE_ERRORS_H_
