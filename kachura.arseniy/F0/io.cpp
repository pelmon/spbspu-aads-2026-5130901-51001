#include "io.hpp"
#include <iostream>
#include <sstream>
#include "map.hpp"
#include "game.hpp"
#include "helper.hpp"
#include "records.hpp"

namespace swapper {

namespace {

void processGenMap(std::istringstream & in)
{
  std::string name;
  int w = 0;
  int h = 0;
  int mines = 0;
  if (!(in >> name >> w >> h >> mines)) {
    std::cout << "Usage: gen-map <name> <x> <y> <mines>" << std::endl;
    return;
  }
  MapData_t data;
  data.name = name;
  data.width = w;
  data.height = h;
  data.mines = mines;
  data.originX = 0;
  data.originY = 0;
  data.seed = 42;
  std::string msg;
  MapStorage::createMap(data, msg);
  std::cout << msg << std::endl;
}

void processMapEdit(std::istringstream & in)
{
  std::string mapName;
  std::string action;
  int x = 0;
  int y = 0;
  if (!(in >> mapName >> action >> x >> y)) {
    std::cout << "Usage: map-edit <name> <action> <x> <y>" << std::endl;
    return;
  }
  if (!MapStorage::hasMap(mapName)) {
    std::cout << "Map not found." << std::endl;
    return;
  }
  Map & map = MapStorage::getMap(mapName);
  if (!map.inBounds(x, y)) {
    std::cout << "Point out of map." << std::endl;
    return;
  }
  Cell_t & cell = map.cellAt(x, y);
  if (action == "add-mine") {
    if (cell.state != CellState::Hidden) {
      std::cout << "Warning: cell already marked, replacing." << std::endl;
    }
    cell.state = CellState::MineMark;
    cell.hasMine = true;
    std::cout << "Mine mark added." << std::endl;
  } else if (action == "add-random") {
    if (cell.state != CellState::Hidden) {
      std::cout << "Warning: cell already marked, replacing." << std::endl;
    }
    cell.state = CellState::RandomMark;
    std::cout << "Random mark added." << std::endl;
  } else if (action == "add-space") {
    if (cell.state != CellState::Hidden) {
      std::cout << "Warning: cell already marked." << std::endl;
    } else {
      std::cout << "Space added." << std::endl;
    }
  } else if (action == "add-enemy") {
    if (cell.state != CellState::Hidden) {
      std::cout << "Warning: replacing mark." << std::endl;
    }
    cell.state = CellState::EnemyMark;
    std::cout << "Enemy mark added." << std::endl;
  } else if (action == "del-mine") {
    if (cell.state == CellState::MineMark) {
      cell.state = CellState::Hidden;
      cell.hasMine = false;
      std::cout << "Mine removed." << std::endl;
    } else {
      std::cout << "Nothing to delete." << std::endl;
    }
  } else if (action == "del-random") {
    if (cell.state == CellState::RandomMark) {
      cell.state = CellState::Hidden;
      std::cout << "Random mark removed." << std::endl;
    } else {
      std::cout << "Nothing to delete." << std::endl;
    }
  } else {
    std::cout << "Unknown action." << std::endl;
  }
}

void processStart(std::istringstream & in)
{
  std::string name;
  if (!(in >> name)) {
    std::cout << "Usage: start <name>" << std::endl;
    return;
  }
  std::string msg;
  if (Game::instance().start(name, msg)) {
    std::cout << "Game started on " << name << std::endl;
  } else {
    std::cout << msg << std::endl;
  }
}

void processGameCommand(const std::string & cmd, std::istringstream & in)
{
  Game & game = Game::instance();
  std::string msg;
  if (!game.isActive()) {
    std::cout << "No active game. Use start." << std::endl;
    return;
  }
  std::string dir;
  in >> dir;
  if (cmd == "move") {
    game.move(dir, msg);
  } else if (cmd == "dig") {
    game.dig(dir, msg);
  } else if (cmd == "flag") {
    game.flag(dir, msg);
  }
  std::cout << msg << std::endl;
}

void processSave(std::istringstream & in)
{
  std::string name;
  if (!(in >> name)) {
    std::cout << "Usage: save <name>" << std::endl;
    return;
  }
  std::string msg;
  Game::instance().save(name, msg);
  std::cout << msg << std::endl;
}

void processLoad(std::istringstream & in)
{
  std::string name;
  if (!(in >> name)) {
    std::cout << "Usage: load <name>" << std::endl;
    return;
  }
  std::string msg;
  Game::instance().load(name, msg);
  std::cout << msg << std::endl;
}

void processRecords(std::istringstream & in)
{
  std::string action;
  in >> action;
  if (action.empty() || action == "view") {
    Records::printRecords();
  } else if (action == "r") {
    std::string id;
    in >> id;
    std::string msg;
    Records::removeRecord(id, msg);
    std::cout << msg << std::endl;
  } else {
    std::cout << "Unknown records action." << std::endl;
  }
}

void processDeleteMap(std::istringstream & in)
{
  std::string name;
  if (!(in >> name)) {
    std::cout << "Usage: delete-map <name>" << std::endl;
    return;
  }
  std::string msg;
  MapStorage::deleteMap(name, msg);
  std::cout << msg << std::endl;
}

void processViewMap(std::istringstream & in)
{
  std::string name;
  in >> name;
  if (name.empty()) {
    Vector<std::string > maps = MapStorage::listMapNames();
    if (maps.empty()) {
      std::cout << "No maps. Create a new one." << std::endl;
    } else {
      for (std::size_t i = 0; i < maps.size(); ++i) {
        std::cout << maps[i] << std::endl;
      }
    }
    return;
  }
  MapStorage::printMap(name);
}

}

int runApp()
{
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    std::istringstream in(line);
    std::string cmd;
    in >> cmd;
    if (cmd == "gen-map" || cmd == "create-map") {
      processGenMap(in);
    } else if (cmd == "view-map" || cmd == "viewmap") {
      processViewMap(in);
    } else if (cmd == "map-edit") {
      processMapEdit(in);
    } else if (cmd == "start") {
      processStart(in);
    } else if (cmd == "move" || cmd == "dig" || cmd == "flag") {
      processGameCommand(cmd, in);
    } else if (cmd == "save") {
      processSave(in);
    } else if (cmd == "load") {
      processLoad(in);
    } else if (cmd == "records") {
      processRecords(in);
    } else if (cmd == "delete-map") {
      processDeleteMap(in);
    } else if (cmd == "hint") {
      printHint();
    } else {
      std::cout << "Unknown command: " << cmd << std::endl;
    }
  }
  return 0;
}

}