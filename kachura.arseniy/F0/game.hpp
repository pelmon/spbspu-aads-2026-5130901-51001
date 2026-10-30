#ifndef GAME_HPP
#define GAME_HPP

#include <string>
#include "map.hpp"
#include "hash.hpp"

namespace swapper {

class Game
{
public:
  static Game & instance();

  bool isActive() const;
  bool start(const std::string & mapName, std::string & message);
  void stop();

  bool move(const std::string & direction, std::string & message);
  bool dig(const std::string & direction, std::string & message);
  bool flag(const std::string & direction, std::string & message);
  bool save(const std::string & saveName, std::string & message);
  bool load(const std::string & saveName, std::string & message);
  bool placeMines(std::string & message);

  int cursorX() const;
  int cursorY() const;
  int moves() const;

private:
  Game();

  bool applyDirection(const std::string & direction, int & dx, int & dy) const;
  void computeAdjacency();

  bool active_;
  std::string mapName_;
  int cursorX_;
  int cursorY_;
  int moves_;
  int seed_;
};

class SaveStorage
{
public:
  static bool save(const std::string & name, const SaveState_t & state, std::string & message);
  static bool load(const std::string & name, SaveState_t & state, std::string & message);
  static bool hasSave(const std::string & name);
  static Vector<std::string > listSaveNames();
  static void remove(const std::string & name);

private:
  static HashTable<std::string, SaveState_t > & storage();
};

}

#endif