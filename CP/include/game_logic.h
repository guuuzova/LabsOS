#pragma once

#include <cstdlib>
#include <ctime>
#include <vector>
#include "common.h"

class GameBoard {
public:
  GameBoard();

  void placeShipsRandomly();

  Result processShot(int x, int y);
  Board getRawBoard() const;
  static char cellToChar(CellState state, bool showShips);
private:
  CellState cells[BOARD_SIZE][BOARD_SIZE];
  int shipsAlive;
};