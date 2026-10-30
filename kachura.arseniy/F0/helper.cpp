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
      << "Commands: gen-map, map-view, start" << std::endl; // не забыть дописать!!!!
    return;
  }
  int sx = 0;
  int sy = 0;
  double prob = 0.0;
  Map & map = MapStorage::getMap(game.getMapName());
  if (!map.findSafeCell(sx, sy, prob)) {
    std::cout << "Hint: good luck" << std::endl;
    return;
  }
  const MapData_t & data = map.getData();
  int relX = sx - data.originX;
  int relY = sy - data.originY;
  if (prob == 0.0) {
    std::cout << "Hint: mine is definitely NOT at ("
              << relX << ", " << relY << ")." << std::endl;
  } else {
    std::cout << "Hint: least likely mine at ("
              << relX << ", " << relY
              << "), probability ~ " << prob << '.' << std::endl;
  }
}

}
