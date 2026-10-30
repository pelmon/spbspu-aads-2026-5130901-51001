#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <cstddef>

namespace swapper {

enum class CellState
{
  Hidden,
  Opened,
  Flag,
  MineMark,
  RandomMark,
  EnemyMark
};

struct Cell_t
{
  CellState state;
  bool hasMine;
  int adjacentMines;
};

struct Point_t
{
  int x;
  int y;
};

struct MapData_t
{
  std::string name;
  int width;
  int height;
  int mines;
  int originX;
  int originY;
  int seed;
};

struct SaveState_t
{
  std::string mapName;
  int cursorX;
  int cursorY;
  int moves;
};

struct Record_t
{
  std::string mapName;
  std::string result;
  int moves;
  long long timeMs;
};

}

#endif