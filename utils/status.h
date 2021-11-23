#ifndef ABEILLE_STATUS_H_
#define ABEILLE_STATUS_H_

namespace Abeille {

class Status {
   public:
    // add other status code if you need
    enum class Code { OK, UNIMPLEMENTED, FAILURE };

    Status() : code(Code::OK) {}
    Status(Code code, const std::string& msg) : _code(code), _error_msg(msg) {}
    Code get_error_code() { return _code; }
    std::string get_error_msg() { return _error_msg; }
    bool ok() const { return _code == Code::OK; }

   private:
    Code _code;
    std::string _error_msg;
}
}  // namespace Abeille

#endif  // ABEILLE_STATUS_H_
