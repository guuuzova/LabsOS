#include "game_logic.h"
#include <algorithm>

GameBoard::GameBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            cells[i][j] = EMPTY;
        }
    }
    shipsAlive = 20;
}

void GameBoard::placeShipsRandomly() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            cells[i][j] = EMPTY;
        }
    }
    int shipSizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};
    for (int size : shipSizes) {
        bool placed = false;
        while (!placed) {
            int r = rand() % BOARD_SIZE;
            int c = rand() % BOARD_SIZE;
            int dir = rand() % 2;
            bool canPlace = true;
            for (int i = 0; i < size; i++) {
                int currR = r + (dir == 1 ? i : 0);
                int currC = c + (dir == 0 ? i : 0);
                if (currR >= BOARD_SIZE || currC >= BOARD_SIZE || cells[currR][currC] != EMPTY) {
                    canPlace = false;
                    break;
                }
                for (int dr = -1; dr <= 1; dr++) {
                    for (int dc = -1; dc <= 1; dc++) {
                        int nr = currR + dr;
                        int nc = currC + dc;
                        if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                            if (cells[nr][nc] == SHIP) {
                                canPlace = false;
                            }
                        }
                    }
                }
            }
            if (canPlace) {
                for (int i = 0; i < size; i++) {
                    cells[r + (dir == 1 ? i : 0)][c + (dir == 0 ? i : 0)] = SHIP;
                }
                placed = true;
            }
        }
    }
}

Result GameBoard::processShot(int r, int c) {
    if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) {
        return RES_REPEAT;
    }
    if (cells[r][c] == SHIP) {
        cells[r][c] = HIT;
        shipsAlive--;
        if (shipsAlive == 0) {
            return RES_LOSE;
        }
        return RES_HIT;
    } else if (cells[r][c] == EMPTY) {
        cells[r][c] = MISS;
        return RES_MISS;
    }
    return RES_REPEAT;
}

Board GameBoard::getRawBoard() const {
    Board b;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            b.cells[i][j] = cells[i][j];
        }
    }
    return b;
}

char GameBoard::cellToChar(CellState state, bool showShips) {
    switch (state) {
        case SHIP: return showShips ? 'K' : ' ';
        case HIT: return 'X';
        case MISS: return '*';
        case EMPTY: return '.';
        default: return ' ';
    }
}