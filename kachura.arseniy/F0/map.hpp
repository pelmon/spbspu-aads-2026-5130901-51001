#ifndef MAP_HPP
#define MAP_HPP

#include <string>
#include "types.hpp"
#include "vector.hpp"
#include "hash.hpp"

namespace swapper {

class Map
{
public:
  Map();
  explicit Map(const MapData_t & data);

  const MapData_t & getData() const;
  MapData_t & getData();

  bool inBounds(int x, int y) const;
  Cell_t & cellAt(int x, int y);
  const Cell_t & cellAt(int x, int y) const;

  void reveal(int x, int y);
  void revealAround(int x, int y);
  bool isCleared() const;

private:
  MapData_t data_;
  Vector<Vector<Cell_t > > grid_;
};

class MapStorage
{
public:
  static bool createMap(const MapData_t & data, std::string & message);
  static bool deleteMap(const std::string & name, std::string & message);
  static Map & getMap(const std::string & name);
  static bool hasMap(const std::string & name);
  static Vector<std::string > listMapNames();
  static void printMap(const std::string & name);

private:
  static HashTable<std::string, Map > & storage();
};

}

#endif