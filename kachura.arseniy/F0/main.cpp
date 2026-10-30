#include <iostream>
#include "io.hpp"
#include "error_handler.hpp"

int main()
{
  swapper::runApp();
  return (swapper::ErrorHandler::errorCount() == 0) ? 0 : 1;
}
//1) Заменить if на хэш таблицы для выбора нужной функции 2) обработка ошибок в одном месте с помощью исключений