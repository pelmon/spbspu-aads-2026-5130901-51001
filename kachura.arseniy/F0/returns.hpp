#ifndef RETURNS_HPP
#define RETURNS_HPP

#include <string>

namespace swapper {

enum class ResultKind
{
  Ok,
  Warning,
  Error
};

struct Result_t
{
  ResultKind kind;
  std::string message;
};

Result_t makeOk(const std::string & message);
Result_t makeWarning(const std::string & message);
Result_t makeError(const std::string & message);

}

#endif
