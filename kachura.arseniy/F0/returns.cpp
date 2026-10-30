#include "returns.hpp"

namespace swapper {

Result_t makeOk(const std::string & message)
{
  Result_t r;
  r.kind = ResultKind::Ok;
  r.message = message;
  return r;
}

Result_t makeWarning(const std::string & message)
{
  Result_t r;
  r.kind = ResultKind::Warning;
  r.message = message;
  return r;
}

Result_t makeError(const std::string & message)
{
  Result_t r;
  r.kind = ResultKind::Error;
  r.message = message;
  return r;
}

}