#include "error_handler.hpp"
#include <iostream>

namespace swapper {

int & ErrorHandler::counter()
{
  static int count = 0;
  return count;
}

void ErrorHandler::report(const Result_t & result)
{
  switch (result.kind) {
  case ResultKind::Ok:
    std::cout << result.message << std::endl;
    break;
  case ResultKind::Warning:
    std::cout << "Warning: " << result.message << std::endl;
    break;
  case ResultKind::Error:
    std::cerr << "Error: " << result.message << std::endl;
    ++counter();
    break;
  }
}

void ErrorHandler::reportRaw(const std::string & message)
{
  std::cout << message << std::endl;
}

int ErrorHandler::errorCount()
{
  return counter();
}

void ErrorHandler::reset()
{
  counter() = 0;
}

}
