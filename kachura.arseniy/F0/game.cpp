#include "game.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>

namespace swapper {

Game::Game():
  active_(false),
  mapName_(),
  cursorX_(0),
  cursorY_(0),
  moves_(0),
  seed_(0)
{
}

Game & Game::instance()
{
  static Game game;
  return game;
}

bool Game::isActive() const
{
  return active_;
}

bool Game::start(const std::string & mapName, std::string & message)
{
  if (!MapStorage::hasMap(mapName)) {
    message = mapName + " not found!";
    return false;
  }
  active_ = true;
  mapName_ = mapName;
  Map & map = MapStorage::getMap(mapName_);
  cursorX_ = map.getData().originX;
  cursorY_ = map.getData().originY;
  moves_ = 0;
  placeMines(message);
  return true;
}

void Game::printField() const
{
  if (!active_) {
    return;
  }
  if (!MapStorage::hasMap(mapName_)) {
    return;
  }
  const Map & map = MapStorage::getMap(mapName_);
  map.printWithCursor(cursorX_, cursorY_);
}

void Game::stop()
{
  active_ = false;
  mapName_.clear();
  moves_ = 0;
}

bool Game::applyDirection(const std::string & direction, int & dx, int & dy) const
{
  if (direction == "up" || direction == "u") {
    dx = 0;
    dy = -1;
    return true;
  } else if (direction == "down" || direction == "d") {
    dx = 0;
    dy = 1;
    return true;
  } else if (direction == "left" || direction == "l") {
    dx = -1;
    dy = 0;
    return true;
  } else if (direction == "right" || direction == "r") {
    dx = 1;
    dy = 0;
    return true;
  }
  return false;
}

bool Game::move(const std::string & direction, std::string & message)
{
  int dx = 0;
  int dy = 0;
  if (!applyDirection(direction, dx, dy)) {
    message = "Invalid direction.";
    return false;
  }
  Map & map = MapStorage::getMap(mapName_);
  int nx = cursorX_ + dx;
  int ny = cursorY_ + dy;
  if (!map.inBounds(nx, ny)) {
    message = "Cursor out of map!";
    return false;
  }
  cursorX_ = nx;
  cursorY_ = ny;
  ++moves_;
  message = "Moved to (" + std::to_string(cursorX_) + ", " + std::to_string(cursorY_) + ")";
  return true;
}

bool Game::dig(const std::string & direction, std::string & message)
{
  int dx = 0;
  int dy = 0;
  applyDirection(direction, dx, dy);
  Map & map = MapStorage::getMap(mapName_);
  int tx = cursorX_ + dx;
  int ty = cursorY_ + dy;
  if (!map.inBounds(tx, ty)) {
    message = "Out of map!";
    return false;
  }
  Cell_t & cell = map.cellAt(tx, ty);
  if (cell.state != CellState::Hidden) {
    message = "Cell already opened or flagged.";
    return false;
  }
  if (cell.hasMine) {
    cell.state = CellState::Opened;
    message = "Mine exploded! Game over.";
    active_ = false;
    return true;
  }
  map.reveal(tx, ty);
  ++moves_;
  if (map.isCleared()) {
    message = "Map cleared! Victory.";
    active_ = false;
  } else {
    message = "Dug at (" + std::to_string(tx) + ", " + std::to_string(ty) + ")";
  }
  return true;
}

bool Game::flag(const std::string & direction, std::string & message)
{
  int dx = 0;
  int dy = 0;
  applyDirection(direction, dx, dy);
  Map & map = MapStorage::getMap(mapName_);
  int tx = cursorX_ + dx;
  int ty = cursorY_ + dy;
  if (!map.inBounds(tx, ty)) {
    message = "Out of map!";
    return false;
  }
  Cell_t & cell = map.cellAt(tx, ty);
  if (cell.state == CellState::Opened) {
    message = "Cannot flag opened cell.";
    return false;
  }
  if (cell.state == CellState::Flag) {
    cell.state = CellState::Hidden;
    message = "Flag removed.";
  } else {
    cell.state = CellState::Flag;
    message = "Flag placed.";
  }
  return true;
}

bool Game::placeMines(std::string & message)
{
  Map & map = MapStorage::getMap(mapName_);
  const MapData_t & data = map.getData();
  std::mt19937 rng(static_cast<unsigned>(data.seed));
  int placed = 0;
  while (placed < data.mines) {
    int rx = static_cast<int>(rng() % static_cast<unsigned>(data.width));
    int ry = static_cast<int>(rng() % static_cast<unsigned>(data.height));
    Cell_t & cell = map.cellAt(data.originX + rx, data.originY + ry);
    if (!cell.hasMine) {
      cell.hasMine = true;
      ++placed;
    }
  }
  computeAdjacency();
  message = "Mines placed.";
  return true;
}

void Game::computeAdjacency()
{
  Map & map = MapStorage::getMap(mapName_);
  const MapData_t & data = map.getData();
  for (int y = 0; y < data.height; ++y) {
    for (int x = 0; x < data.width; ++x) {
      int count = 0;
      for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
          int cx = data.originX + x + dx;
          int cy = data.originY + y + dy;
          if (map.inBounds(cx, cy) && map.cellAt(cx, cy).hasMine) {
            ++count;
          }
        }
      }
      map.cellAt(data.originX + x, data.originY + y).adjacentMines = count;
    }
  }
}

bool Game::save(const std::string & saveName, std::string & message)
{
  SaveState_t state;
  state.mapName = mapName_;
  state.cursorX = cursorX_;
  state.cursorY = cursorY_;
  state.moves = moves_;
  return SaveStorage::save(saveName, state, message);
}

bool Game::load(const std::string & saveName, std::string & message)
{
  SaveState_t state;
  if (!SaveStorage::load(saveName, state, message)) {
    return false;
  }
  if (!MapStorage::hasMap(state.mapName)) {
    message = "Map from save not found.";
    return false;
  }
  active_ = true;
  mapName_ = state.mapName;
  cursorX_ = state.cursorX;
  cursorY_ = state.cursorY;
  moves_ = state.moves;
  message = "Game loaded.";
  return true;
}

int Game::cursorX() const
{
  return cursorX_;
}

int Game::cursorY() const
{
  return cursorY_;
}

int Game::moves() const
{
  return moves_;
}

HashTable<std::string, SaveState_t > & SaveStorage::storage()
{
  static HashTable<std::string, SaveState_t > table;
  return table;
}

bool SaveStorage::hasSave(const std::string & name)
{
  return storage().contains(name);
}

bool SaveStorage::save(const std::string & name, const SaveState_t & state, std::string & message)
{
  storage().insert(name, state);
  std::ofstream out(name + ".sav");
  if (!out) {
    message = "Cannot write save file.";
    return false;
  }
  out << state.mapName << ' '
      << state.cursorX << ' '
      << state.cursorY << ' '
      << state.moves << '\n';
  out.close();
  message = name + " saved.";
  return true;
}

bool SaveStorage::load(const std::string & name, SaveState_t & state, std::string & message)
{
  if (storage().contains(name)) {
    state = storage().at(name);
    message = "Loaded.";
    return true;
  }
  std::ifstream in(name + ".sav");
  if (!in) {
    message = "Save not found.";
    return false;
  }
  in >> state.mapName >> state.cursorX >> state.cursorY >> state.moves;
  in.close();
  storage().insert(name, state);
  message = "Loaded.";
  return true;
}

Vector<std::string > SaveStorage::listSaveNames()
{
  return storage().keys();
}

void SaveStorage::remove(const std::string & name)
{
  if (storage().contains(name)) {
    storage().erase(name);
  }
}

const std::string & Game::getMapName() const
{
  return mapName_;
}

}