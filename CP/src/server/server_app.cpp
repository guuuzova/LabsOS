#include "server_app.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cstring>

ServerApp::ServerApp() : serverPipe(SERVER_MAIN_FIFO), isRunning(true) {
    pthread_mutex_init(&list_mutex, NULL);
    serverPipe.CreatePipe();
    loadStatusFromFile();
}

void ServerApp::handleCreateGame(Packet &pkt) {
    pthread_mutex_lock(&list_mutex);
    GameSession newGame;
    strncpy(newGame.name, pkt.target, MAX_LOGIN);
    strncpy(newGame.player1, pkt.login, MAX_LOGIN);
    newGame.player2[0] = '\0';
    newGame.isFull = false;
    activeGames.push_back(newGame);
    pthread_mutex_unlock(&list_mutex);
    std::cout << "(server)Game created: " << pkt.target << " by " << pkt.login << std::endl;
}

void ServerApp::handleJoinGame(Packet &pkt) {
    pthread_mutex_lock(&list_mutex);
    for (auto &game : activeGames) {
        if (std::string(game.name) == pkt.target && !game.isFull) {
            strncpy(game.player2, pkt.login, MAX_LOGIN);
            game.isFull = true;
            Packet confirm;
            confirm.cmd = RequestType::JOIN_GAME;
            strncpy(confirm.login, game.player2, MAX_LOGIN); 
            sendToClient(game.player1, confirm);
            strncpy(confirm.login, game.player1, MAX_LOGIN);
            sendToClient(game.player2, confirm);
            break;
        }
    }
    pthread_mutex_unlock(&list_mutex);
}

void ServerApp::loadStatusFromFile() {
    std::ifstream ifs(STATS_FILE);
    if (!ifs.is_open()) {
        return;
    }
    std::string name;
    int w;
    int l;
    while (ifs >> name >> w >> l) {
        Player p;
        memset(&p, 0, sizeof(Player));
        strncpy(p.login, name.c_str(), MAX_LOGIN);
        p.wins = w;
        p.losses = l;
        players.push_back(p);
    }
    ifs.close();
    std::cout << "(server)Loaded stats for " << players.size() << " players." << std::endl;
}

Player* ServerApp::findPlayer(const std::string &login) {
    for (auto &p : players) {
        if (std::string(p.login) == login) {
            return &p;
        }
    }
    return nullptr;
}

void ServerApp::sendToClient(const std::string &login, Packet &pkt) {
    std::string clientPath = CLIENT_FIFO_PREFIX + login;
    NamedPipe clientPipe(clientPath);
    if (clientPipe.OpenPipe(O_WRONLY)) {
        clientPipe.Send(&pkt, sizeof(Packet));
        clientPipe.ClosePipe();
    }
}

void ServerApp::saveStatsToFile() {
    std::ofstream ofs(STATS_FILE);
    if (!ofs.is_open()) {
        return;
    }
    for (const auto& p : players) {
        ofs << p.login << " " << p.wins << " " << p.losses << std::endl;
    }
    ofs.close();
}

void ServerApp::handleLogin(Packet &pkt) {
    pthread_mutex_lock(&list_mutex);
    Player* p = findPlayer(pkt.login);
    if (!p) {
        Player newPlayer;
        memset(&newPlayer, 0, sizeof(Player));
        strncpy(newPlayer.login, pkt.login, MAX_LOGIN);
        players.push_back(newPlayer);
        std::cout << "(login)New player: " << pkt.login << std::endl;
    } else {
        std::cout << "(login))Player returned: " << pkt.login << std::endl;
    }
    pthread_mutex_unlock(&list_mutex);
}

void ServerApp::handlePlaceShips(Packet &pkt) {
    pthread_mutex_lock(&list_mutex);
    Player* p = findPlayer(pkt.login);
    if (p) {
        p->board = pkt.board;
        std::cout << "[BOARD] " << pkt.login << " placed ships.\n";
    }
    pthread_mutex_unlock(&list_mutex);
}

void ServerApp::hanldeShoot(Packet &pkt) {
    pthread_mutex_lock(&list_mutex);
    Player* shooter = findPlayer(pkt.login);
    Player* victim = findPlayer(pkt.target);
    if (shooter && victim) {
        CellState &cell = victim->board.cells[pkt.row][pkt.col];
        if (cell == HIT || cell == MISS) {
            pkt.res = RES_REPEAT;
            pkt.is_hit = false;
            pkt.your_turn = true;
        } else if (cell == SHIP) {
            cell = HIT;
            pkt.is_hit = true;
            pkt.your_turn = true;
            bool any_ships_left = false;
            for(int i=0; i<BOARD_SIZE; i++) {
                for(int j=0; j<BOARD_SIZE; j++){
                    if(victim->board.cells[i][j] == SHIP) {
                        any_ships_left = true;
                    }
                }
            }
            if (!any_ships_left) {
                pkt.res = RES_LOSE;
                shooter->wins++; 
                victim->losses++;
                saveStatsToFile();
            } else {
                bool part_of_ship_alive = false;
                int dr[] = {-1, 1, 0, 0};
                int dc[] = {0, 0, -1, 1};
                for(int i = 0; i < 4; i++) {
                    int nr = pkt.row + dr[i];
                    int nc = pkt.col + dc[i];
                    if(nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                        if(victim->board.cells[nr][nc] == SHIP) {
                            part_of_ship_alive = true;
                            break;
                        }
                    }
                }
                if (part_of_ship_alive) {
                    pkt.res = RES_HIT;
                } else {
                    pkt.res = RES_SUNK;
                }
            }
        } else {
            cell = MISS;
            pkt.is_hit = false;
            pkt.res = RES_MISS;
            pkt.your_turn = false;
        }
        sendToClient(pkt.login, pkt);
        Packet notifyVictim = pkt;
        notifyVictim.your_turn = !pkt.your_turn; 
        sendToClient(pkt.target, notifyVictim);
    }
    pthread_mutex_unlock(&list_mutex);
}

void ServerApp::handleLogout(Packet &pkt) {
    pthread_mutex_lock(&list_mutex);
    std::cout << "(quit)Player disconnected: " << pkt.login << std::endl;
    pthread_mutex_unlock(&list_mutex);
}

void ServerApp::run() {
    std::cout << "(server)Waiting for players..." << std::endl;
    while (isRunning) {
        if (serverPipe.OpenPipe(O_RDONLY)) {
            Packet pkt;
            while (serverPipe.Receive(&pkt, sizeof(Packet))) {
                switch (pkt.cmd) {
                    case RequestType::LOGIN: handleLogin(pkt); break;
                    case RequestType::PLACE_SHIPS: handlePlaceShips(pkt); break;
                    case RequestType::SHOT: hanldeShoot(pkt); break;
                    case RequestType::QUIT: handleLogout(pkt); break;
                    case RequestType::GET_STATS: handleGetStats(pkt); break;
                    case RequestType::CREATE_GAME: handleCreateGame(pkt); break;
                    case RequestType::JOIN_GAME:   handleJoinGame(pkt); break;
                }
            }
            serverPipe.ClosePipe();
        }
    }
}

void ServerApp::handleGetStats(Packet &pkt) {
    pthread_mutex_lock(&list_mutex);
    Player* p = findPlayer(pkt.login);
    if (p) {
        pkt.row = p->wins;
        pkt.col = p->losses;
        std::cout << "(stats)Sending stats to " << pkt.login << ": W:" << p->wins << " L:" << p->losses << std::endl;
    }
    pthread_mutex_unlock(&list_mutex);
    sendToClient(pkt.login, pkt);
}

ServerApp::~ServerApp() {
    saveStatsToFile();
    pthread_mutex_destroy(&list_mutex);
    unlink(SERVER_MAIN_FIFO);
}