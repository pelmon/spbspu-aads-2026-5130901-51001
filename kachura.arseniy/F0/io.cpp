#include "io.hpp"
#include <iostream>
#include <sstream>
#include "returns.hpp"
#include "error_handler.hpp"
#include "map.hpp"
#include "game.hpp"
#include "records.hpp"
#include "helper.hpp"

namespace swapper {

namespace {

using CommandHandler_t = Result_t (*)(std::istringstream &);

Result_t processGenMap(std::istringstream & in)
{
  std::string name;
  int w = 0;
  int h = 0;
  int mines = 0;
  if (!(in >> name >> w >> h >> mines)) {
    return makeError("Usage: gen-map <name> <x> <y> <mines>");
  }
  MapData_t data;
  data.name = name;
  data.width = w;
  data.height = h;
  data.mines = mines;
  data.originX = -(w / 2);
  data.originY = -(h / 2);
  data.seed = 42;
  std::string msg;
    bool ok = MapStorage::createMap(data, msg);
  if (ok) {
    return makeOk(msg);
  }
  return makeError(msg);
}

Result_t processViewMap(std::istringstream & in)
{
  std::string name;
  in >> name;
  if (name.empty()) {
    Vector<std::string > maps = MapStorage::listMapNames();
    if (maps.empty()) {
      return makeOk("No maps. Create a new one.");
    }
    std::string result;
    for (std::size_t i = 0; i < maps.size(); ++i) {
      result += maps[i];
      if (i + 1 < maps.size()) {
        result += '\n';
      }
    }
    return makeOk(result);
  }
  if (!MapStorage::hasMap(name)) {
    return makeError(name + " not found!");
  }
  MapStorage::printMap(name);
  return makeOk("");
}

Result_t processMapEdit(std::istringstream & in)
{
  std::string mapName;
  std::string action;
  int x = 0;
  int y = 0;
  if (!(in >> mapName >> action >> x >> y)) {
    return makeError("Usage: map-edit <name> <action> <x> <y>");
  }
  if (!MapStorage::hasMap(mapName)) {
    return makeError("Map not found.");
  }
  Map & map = MapStorage::getMap(mapName);
  if (!map.inBounds(x, y)) {
    return makeError("Point out of map.");
  }
  Cell_t & cell = map.cellAt(x, y);
  if (action == "add-mine") {
    if (cell.state != CellState::Hidden) {
      cell.state = CellState::MineMark;
      cell.hasMine = true;
      return makeWarning("Cell already marked, replacing with mine.");
    }
    cell.state = CellState::MineMark;
    cell.hasMine = true;
    return makeOk("Mine mark added.");
  }
  if (action == "add-random") {
    if (cell.state != CellState::Hidden) {
      cell.state = CellState::RandomMark;
      return makeWarning("Cell already marked, replacing with random mark.");
    }
    cell.state = CellState::RandomMark;
    return makeOk("Random mark added.");
  }
  if (action == "add-space") {
    if (cell.state != CellState::Hidden) {
      return makeWarning("Cell already marked.");
    }
    return makeOk("Space added.");
  }
  if (action == "add-enemy") {
    if (cell.state != CellState::Hidden) {
      cell.state = CellState::EnemyMark;
      return makeWarning("Cell already marked, replacing with enemy mark.");
    }
    cell.state = CellState::EnemyMark;
    return makeOk("Enemy mark added.");
  }
  if (action == "del-mine") {
    if (cell.state != CellState::MineMark) {
      return makeWarning("Nothing to delete.");
    }
    cell.state = CellState::Hidden;
    cell.hasMine = false;
    return makeOk("Mine removed.");
  }
  if (action == "del-random") {
    if (cell.state != CellState::RandomMark) {
      return makeWarning("Nothing to delete.");
    }
    cell.state = CellState::Hidden;
    return makeOk("Random mark removed.");
  }
  return makeError("Unknown action: " + action);
}

Result_t processStart(std::istringstream & in)
{
  std::string name;
  if (!(in >> name)) {
    return makeError("Usage: start <name>");
  }
  std::string msg;
  if (!Game::instance().start(name, msg)) {
    return makeError(msg);
  }
  Game::instance().printField();
  return makeOk("Game started on " + name);
}

Result_t processMove(std::istringstream & in)
{
  Game & game = Game::instance();
  if (!game.isActive()) {
    return makeError("No active game. Use start.");
  }
  std::string dir;
  in >> dir;
  std::string msg;
  bool ok = game.move(dir, msg);
  if (ok) {
    game.printField();
  }
  return ok ? makeOk(msg) : makeError(msg);
}

Result_t processDig(std::istringstream & in)
{
  Game & game = Game::instance();
  if (!game.isActive()) {
    return makeError("No active game. Use start.");
  }
  std::string dir;
  in >> dir;
  std::string msg;
  bool ok = game.dig(dir, msg);
  std::cout << msg << std::endl;
  if (ok) {
    game.printField();
  }
  return ok ? makeOk(msg) : makeError(msg);
}

Result_t processFlag(std::istringstream & in)
{
  Game & game = Game::instance();
  if (!game.isActive()) {
    return makeError("No active game. Use start.");
  }
  std::string dir;
  in >> dir;
  std::string msg;
  bool ok = game.flag(dir, msg);
  std::cout << msg << std::endl;
  if (ok) {
    game.printField();
  }
  return ok ? makeOk(msg) : makeError(msg);
}

Result_t processSave(std::istringstream & in)
{
  std::string name;
  if (!(in >> name)) {
    return makeError("Usage: save <name>");
  }
  std::string msg;
  bool ok = Game::instance().save(name, msg);
  return ok ? makeOk(msg) : makeError(msg);
}

Result_t processLoad(std::istringstream & in)
{
  std::string name;
  if (!(in >> name)) {
    return makeError("Usage: load <name>");
  }
  std::string msg;
  if (!Game::instance().load(name, msg)) {
    return makeError(msg);
  }
  Game::instance().printField();
  return makeOk(msg);
}

Result_t processRecords(std::istringstream & in)
{
  std::string action;
  in >> action;
  if (action.empty() || action == "view") {
    Records::printRecords();
    return makeOk("");
  }
  if (action == "r") {
    std::string id;
    in >> id;
    std::string msg;
    bool ok = Records::removeRecord(id, msg);
    return ok ? makeOk(msg) : makeError(msg);
  }
  return makeError("Unknown records action: " + action);
}

Result_t processDeleteMap(std::istringstream & in)
{
  std::string name;
  if (!(in >> name)) {
    return makeError("Usage: delete-map <name>");
  }
  std::string msg;
  bool ok = MapStorage::deleteMap(name, msg);
  return ok ? makeOk(msg) : makeError(msg);
}

Result_t processHint(std::istringstream & in)
{
  (void)in;
  printHint();
  return makeOk("");
}

HashTable<std::string, CommandHandler_t > & commandTable()
{
  static HashTable<std::string, CommandHandler_t > table;
  static bool initialized = false;
  if (!initialized) {
    table.insert("gen-map",     &processGenMap);
    table.insert("create-map",  &processGenMap);
    table.insert("view-map",    &processViewMap);
    table.insert("viewmap",     &processViewMap);
    table.insert("map-edit",    &processMapEdit);
    table.insert("start",       &processStart);
    table.insert("move",        &processMove);
    table.insert("dig",         &processDig);
    table.insert("flag",        &processFlag);
    table.insert("save",        &processSave);
    table.insert("load",        &processLoad);
    table.insert("records",     &processRecords);
    table.insert("delete-map",  &processDeleteMap);
    table.insert("hint",        &processHint);
    initialized = true;
  }
  return table;
}

}

Result_t runApp()
{
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) {
      continue;
    }
    std::istringstream in(line);
    std::string cmd;
    in >> cmd;
    if (!commandTable().contains(cmd)) {
      ErrorHandler::report(makeError("Unknown command: " + cmd));
      continue;
    }
    CommandHandler_t handler = commandTable().at(cmd);
    Result_t result = handler(in);
    ErrorHandler::report(result);
  }
  return makeOk("");
}

}
