#pragma once

#include <iostream>
#include <string>
#include <cstring>

#define BOARD_SIZE 10
#define MAX_LOGIN 64
#define STATS_FILE "stats.txt"

#define SERVER_MAIN_FIFO "/tmp/sb_server_in"
#define CLIENT_FIFO_PREFIX "/tmp/sb_client_"

enum CellState { EMPTY = 0, SHIP = 1, HIT = 2, MISS = 3 };
enum Result { RES_MISS, RES_HIT, RES_SUNK, RES_REPEAT, RES_LOSE };

enum class RequestType {
    LOGIN,
    CREATE_GAME,
    JOIN_GAME,
    GET_STATS,
    PLACE_SHIPS,
    SHOT,
    QUIT
};

struct Board {
    CellState cells[BOARD_SIZE][BOARD_SIZE];
};

struct Player {
    char login[MAX_LOGIN];
    Board board;
    int wins;
    int losses;
};

struct GameSession {
    char name[MAX_LOGIN];
    char player1[MAX_LOGIN];
    char player2[MAX_LOGIN];
    bool isFull;
};

struct Packet {
    RequestType cmd;
    char login[MAX_LOGIN];
    char target[MAX_LOGIN];
    int row;
    int col;
    bool is_hit;
    Result res;
    Board board;
    bool your_turn;
};