#ifndef ERROR_HANDLER_HPP
#define ERROR_HANDLER_HPP

#include "returns.hpp"

namespace swapper {

class ErrorHandler
{
public:
  static void report(const Result_t & result);
  static void reportRaw(const std::string & message);
  static int errorCount();
  static void reset();

private:
  static int & counter();
};

}

#endif