#include "map.hpp"
#include <iostream>

namespace swapper {

Map::Map():
  data_()
{
}

Map::Map(const MapData_t & data):
  data_(data)
{
  grid_.reserve(static_cast<std::size_t>(data_.height));
  for (int y = 0; y < data_.height; ++y) {
    Vector<Cell_t > row;
    row.reserve(static_cast<std::size_t>(data_.width));
    for (int x = 0; x < data_.width; ++x) {
      Cell_t cell;
      cell.state = CellState::Hidden;
      cell.hasMine = false;
      cell.adjacentMines = 0;
      row.pushBack(cell);
    }
    grid_.pushBack(row);
  }
}

const MapData_t & Map::getData() const
{
  return data_;
}

MapData_t & Map::getData()
{
  return data_;
}

bool Map::inBounds(int x, int y) const
{
  int dx = x - data_.originX;
  int dy = y - data_.originY;
  return (dx >= 0 && dx < data_.width && dy >= 0 && dy < data_.height);
}

Cell_t & Map::cellAt(int x, int y)
{
  int dx = x - data_.originX;
  int dy = y - data_.originY;
  return grid_[dy][dx];
}

const Cell_t & Map::cellAt(int x, int y) const
{
  int dx = x - data_.originX;
  int dy = y - data_.originY;
  return grid_[dy][dx];
}

void Map::reveal(int x, int y)
{
  if (!inBounds(x, y)) {
    return;
  }
  Cell_t & cell = cellAt(x, y);
  if (cell.state != CellState::Hidden) {
    return;
  }
  cell.state = CellState::Opened;
  if (cell.hasMine || cell.adjacentMines != 0) {
    return;
  }
  for (int dy = -1; dy <= 1; ++dy) {
    for (int dx = -1; dx <= 1; ++dx) {
      if (dx == 0 && dy == 0) {
        continue;
      }
      reveal(x + dx, y + dy);
    }
  }
}

void Map::revealAround(int x, int y)
{
  for (int dy = -1; dy <= 1; ++dy) {
    for (int dx = -1; dx <= 1; ++dx) {
      if (dx == 0 && dy == 0) {
        continue;
      }
      if (inBounds(x + dx, y + dy)) {
        Cell_t & cell = cellAt(x + dx, y + dy);
        if (cell.state == CellState::Hidden) {
          cell.state = CellState::Opened;
        }
      }
    }
  }
}

bool Map::isCleared() const
{
  for (int y = 0; y < data_.height; ++y) {
    for (int x = 0; x < data_.width; ++x) {
      if (grid_[y][x].state == CellState::Hidden) {
        return false;
      }
    }
  }
  return true;
}

HashTable<std::string, Map > & MapStorage::storage()
{
  static HashTable<std::string, Map > table;
  return table;
}

bool MapStorage::hasMap(const std::string & name)
{
  return storage().contains(name);
}

bool MapStorage::createMap(const MapData_t & data, std::string & message)
{
  if (data.width < 3 || data.height < 3) {
    message = data.name + " was not created! Reason: invalid size.";
    return false;
  }
  if (data.mines < 1 || data.mines >= data.width * data.height) {
    message = data.name + " was not created! Reason: Map is too small or too many mines.";
    return false;
  }
  if (storage().contains(data.name)) {
    message = data.name + " was not created! Reason: map already exists.";
    return false;
  }
  storage().insert(data.name, Map(data));
  message = data.name + " created successfully!";
  return true;
}

bool MapStorage::deleteMap(const std::string & name, std::string & message)
{
  if (!storage().contains(name)) {
    message = name + " was not deleted! Reason: map not found.";
    return false;
  }
  storage().erase(name);
  message = name + " deleted successfully!";
  return true;
}

Map & MapStorage::getMap(const std::string & name)
{
  return storage().at(name);
}

Vector<std::string > MapStorage::listMapNames()
{
  return storage().keys();
}

void MapStorage::printMap(const std::string & name)
{
  if (!storage().contains(name)) {
    std::cout << name << " not found!" << std::endl;
    return;
  }
  Map & map = storage().at(name);
  const MapData_t & data = map.getData();
  std::cout << "Map " << data.name << " (" << data.width << "x" << data.height << ")" << std::endl;
  for (int y = 0; y < data.height; ++y) {
    for (int x = 0; x < data.width; ++x) {
      const Cell_t & cell = map.cellAt(data.originX + x, data.originY + y);
      char sym = '?';
      if (cell.state == CellState::Opened) {
        sym = cell.hasMine ? '*' : (cell.adjacentMines == 0 ? '.' : ('0' + cell.adjacentMines));
      } else if (cell.state == CellState::Flag) {
        sym = 'F';
      } else if (cell.state == CellState::MineMark) {
        sym = '*';
      } else if (cell.state == CellState::EnemyMark) {
        sym = '!';
      }
      std::cout << sym << ' ';
    }
    std::cout << std::endl;
  }
}

void Map::printWithCursor(int cursorX, int cursorY) const
{
  const MapData_t & data = getData();
  std::cout << "Map " << data.name
            << " (" << data.width << "x" << data.height << ")"
            << " mines: " << data.mines << std::endl;
  for (int y = 0; y < data.height; ++y) {
    for (int x = 0; x < data.width; ++x) {
      int gx = data.originX + x;
      int gy = data.originY + y;
      const Cell_t & cell = cellAt(gx, gy);
      char sym = '?';
      if (cell.state == CellState::Opened) {
        if (cell.hasMine) {
          sym = '*';
        } else if (cell.adjacentMines == 0) {
          sym = '.';
        } else {
          sym = static_cast<char>('0' + cell.adjacentMines);
        }
      } else if (cell.state == CellState::Flag) {
        sym = 'F';
      } else if (cell.state == CellState::MineMark) {
        sym = '*';
      } else if (cell.state == CellState::EnemyMark) {
        sym = '!';
      } else if (cell.state == CellState::RandomMark) {
        sym = '?';
      }
      bool isCursor = (gx == cursorX && gy == cursorY);
      if (isCursor) {
        std::cout << '[' << sym << ']';
      } else {
        std::cout << ' ' << sym << ' ';
      }
    }
    std::cout << std::endl;
  }
}

double Map::mineProbability(int x, int y) const
{
  if (!inBounds(x, y)) {
    return -1.0;
  }
  const Cell_t & cell = cellAt(x, y);
  if (cell.state == CellState::Opened) {
    return -1.0;
  }
  if (cell.state == CellState::Flag) {
    return -1.0;
  }

  bool forcedSafe = false;
  bool forcedMine = false;
  double bestProb = 0.0;
  bool hasConstraint = false;

  for (int dy = -1; dy <= 1; ++dy) {
    for (int dx = -1; dx <= 1; ++dx) {
      if (dx == 0 && dy == 0) {
        continue;
      }
      int nx = x + dx;
      int ny = y + dy;
      if (!inBounds(nx, ny)) {
        continue;
      }
      const Cell_t & neighbour = cellAt(nx, ny);
      if (neighbour.state != CellState::Opened) {
        continue;
      }
      if (neighbour.hasMine) {
        continue;
      }
      int flags = 0;
      int hidden = 0;
      for (int fy = -1; fy <= 1; ++fy) {
        for (int fx = -1; fx <= 1; ++fx) {
          if (fx == 0 && fy == 0) {
            continue;
          }
          int cx = nx + fx;
          int cy = ny + fy;
          if (!inBounds(cx, cy)) {
            continue;
          }
          const Cell_t & n2 = cellAt(cx, cy);
          if (n2.state == CellState::Flag) {
            ++flags;
          } else if (n2.state != CellState::Opened) {
            ++hidden;
          }
        }
      }
      int remaining = neighbour.adjacentMines - flags;
      if (hidden == 0) {
        continue;
      }
      hasConstraint = true;
      if (remaining == 0) {
        forcedSafe = true;
      } else if (remaining == hidden) {
        forcedMine = true;
      } else {
        double local = static_cast<double>(remaining) / static_cast<double>(hidden);
        if (local > bestProb) {
          bestProb = local;
        }
      }
    }
  }

  if (forcedMine) {
    return 1.0;
  }
  if (forcedSafe) {
    return 0.0;
  }
  if (!hasConstraint) {
    return -1.0;
  }
  return bestProb;
}

bool Map::findSafeCell(int & outX, int & outY, double & outProb) const
{
  double best = 2.0;
  bool found = false;
  for (int y = 0; y < data_.height; ++y) {
    for (int x = 0; x < data_.width; ++x) {
      int gx = data_.originX + x;
      int gy = data_.originY + y;
      const Cell_t & cell = cellAt(gx, gy);
      if (cell.state == CellState::Opened || cell.state == CellState::Flag) {
        continue;
      }
      double prob = mineProbability(gx, gy);
      if (prob < 0.0) {
        continue;
      }
      if (prob < best) {
        best = prob;
        outX = gx;
        outY = gy;
        found = true;
        if (prob == 0.0) {
          outProb = 0.0;
          return true;
        }
      }
    }
  }
  if (found) {
    outProb = best;
    return true;
  }
  return false;
}

bool Map::findDefiniteMine(int & outX, int & outY) const
{
  for (int y = 0; y < data_.height; ++y) {
    for (int x = 0; x < data_.width; ++x) {
      int gx = data_.originX + x;
      int gy = data_.originY + y;
      const Cell_t & cell = cellAt(gx, gy);
      if (cell.state == CellState::Opened || cell.state == CellState::Flag) {
        continue;
      }
      double prob = mineProbability(gx, gy);
      if (prob == 1.0) {
        outX = gx;
        outY = gy;
        return true;
      }
    }
  }
  return false;
}

}