#include "helper.hpp"
#include <iostream>
#include "game.hpp"
#include "map.hpp"

namespace swapper {

void printHint()
{
  Game & game = Game::instance();
  if (!game.isActive()) {
    std::cout
      << "Commands: gen-map, view-map, start
      Game: move, dig, flag, save, load
      Enter command to check it's arguments." << std::endl;
    return;
  }
  Map & map = MapStorage::getMap(game.getMapName());
  const MapData_t & data = map.getData();

  int sx = 0;
  int sy = 0;
  double prob = 0.0;

  if (map.findSafeCell(sx, sy, prob) && prob == 0.0) {
    int relX = sx;
    int relY = sy;
    std::cout << "Hint: mine is definitely NOT at ("
              << relX << ", " << relY << ")." << std::endl;
    return;
  }

  sx = 0;
  sy = 0;
  if (map.findDefiniteMine(mx, my)) {
    int relX = sx;
    int relY = sy;
    std::cout << "Hint: there is definitely a MINE at ("
              << relX << ", " << relY << ")." << std::endl;
    return;
  }

  if (prob >= 0.0 && prob < 2.0) {
    int relX = sx;
    int relY = sy;
    std::cout << "Hint: least likely mine at ("
              << relX << ", " << relY
              << "), probability ~ " << prob << '.' << std::endl;
    return;
  }

  std::cout << "Hint: no safe cells found. Open a new area." << std::endl;
}

}
