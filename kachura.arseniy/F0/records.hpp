#ifndef RECORDS_HPP
#define RECORDS_HPP

#include <string>
#include "types.hpp"
#include "vector.hpp"

namespace swapper {

class Records
{
public:
  static void addRecord(const Record_t & record);
  static void printRecords();
  static bool removeRecord(const std::string & id, std::string & message);
  static int count();

private:
  static Vector<Record_t > & storage();
  static void sortByTime();
};

}

#endif