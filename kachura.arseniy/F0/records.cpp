#include "records.hpp"
#include <iostream>

namespace swapper {

Vector<Record_t > & Records::storage()
{
  static Vector<Record_t > records;
  return records;
}

void Records::sortByTime()
{
  Vector<Record_t > & recs = storage();
  for (std::size_t i = 0; i + 1 < recs.size(); ++i) {
    for (std::size_t j = 0; j + 1 < recs.size() - i; ++j) {
      if (recs[j].timeMs > recs[j + 1].timeMs) {
        Record_t tmp = recs[j];
        recs[j] = recs[j + 1];
        recs[j + 1] = tmp;
      }
    }
  }
}

void Records::addRecord(const Record_t & record)
{
  storage().pushBack(record);
  sortByTime();
}

void Records::printRecords()
{
  if (storage().empty()) {
    std::cout << "No games finished yet. Start your first game!" << std::endl;
    return;
  }
  std::cout << "Records:" << std::endl;
  for (std::size_t i = 0; i < storage().size(); ++i) {
    const Record_t & r = storage()[i];
    std::cout << (i + 1) << ". " << r.mapName
              << " | " << r.result
              << " | moves: " << r.moves
              << " | time: " << r.timeMs << "ms" << std::endl;
  }
}

bool Records::removeRecord(const std::string & id, std::string & message)
{
  std::size_t index = 0;
  try {
    index = static_cast<std::size_t>(std::stoul(id));
  } catch (...) {
    message = "Invalid record id.";
    return false;
  }
  if (index == 0 || index > storage().size()) {
    message = "Record not found.";
    return false;
  }
  Vector<Record_t > & recs = storage();
  for (std::size_t i = index - 1; i + 1 < recs.size(); ++i) {
    recs[i] = recs[i + 1];
  }
  recs.popBack();
  message = "Record removed.";
  return true;
}

int Records::count()
{
  return static_cast<int>(storage().size());
}

}