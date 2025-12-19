#include "client_app.h"
#include "game_logic.h"
#include <unistd.h>
#include <ctime>

ClientApp::ClientApp() : isRunning(true) {
    memset(&myBoard, 0, sizeof(Board));
    memset(&friendBoard, 0, sizeof(Board));
}

void* ClientApp::listenThreadWrapper(void* ctx) {
    ((ClientApp*)ctx)->listenLoop();
    return nullptr;
}

void ClientApp::sendPacket(Packet &pkt) {
    NamedPipe sp(SERVER_MAIN_FIFO);
    if (sp.OpenPipe(O_WRONLY)) {
        sp.Send(&pkt, sizeof(Packet));
        usleep(10000);
        sp.ClosePipe();
    }
}

void ClientApp::displayBoards() {
    std::cout << "\n    YOUR BOARD                 FRIEND " << std::endl;
    for (int i = 0; i < BOARD_SIZE; i++) {
        std::cout << i << " | ";
        for (int j = 0; j < BOARD_SIZE; j++) {
            std::cout << GameBoard::cellToChar(myBoard.cells[i][j], true) << " ";
        }
        std::cout << "    " << i << " | ";
        for (int j = 0; j < BOARD_SIZE; j++) {
            std::cout << GameBoard::cellToChar(friendBoard.cells[i][j], false) << " ";
        }
        std::cout << std::endl;
    }
}

void ClientApp::start() {
    srand(time(NULL));
    std::cout << "Your login: ";
    std::cin >> login;
    Packet p;
    memset(&p, 0, sizeof(p));
    p.cmd = RequestType::LOGIN;
    strncpy(p.login, login.c_str(), MAX_LOGIN);
    sendPacket(p);

    pthread_create(&listenerThread, NULL, listenThreadWrapper, this);
    std::cout << "1. Create Game" << std::endl;
    std::cout << "2. Join Game" << std::endl;
    int mode;
    std::cin >> mode;
    if (mode == 1) {
        std::cout << "Enter game name: ";
        std::string gName;
        std::cin >> gName;
        p.cmd = RequestType::CREATE_GAME;
        strncpy(p.target, gName.c_str(), MAX_LOGIN);
        sendPacket(p);
        std::cout << "Waiting your friend" << std::endl;
    } else {
        std::cout << "Enter game name to join: ";
        std::string gName;
        std::cin >> gName;
        p.cmd = RequestType::JOIN_GAME;
        strncpy(p.target, gName.c_str(), MAX_LOGIN);
        sendPacket(p);
    }
    while(Friend.empty() && isRunning) {
        usleep(100000);
    }
    GameBoard logic;
    logic.placeShipsRandomly();
    myBoard = logic.getRawBoard();
    p.cmd = RequestType::PLACE_SHIPS;
    p.board = myBoard;
    sendPacket(p);
    while (isRunning) {
        std::cout << "MENU" << std::endl;
        if (!Friend.empty()) {
            std::cout << "Game with " << Friend << std::endl;
        }
        std::cout << "1. Show board\n2. Make a shot\n3. Exit\n4. Statistics\n> ";
        int c;
        std::cin >> c;
        if (c == 1) {
            displayBoards();
        } 
        else if (c == 2) {
            Packet s; memset(&s, 0, sizeof(s));
            s.cmd = RequestType::SHOT;
            strncpy(s.login, login.c_str(), MAX_LOGIN);
            if (Friend.empty()) {
                std::cout << "Enter name your friend: "; 
                std::cin >> Friend;
            }
            strncpy(s.target, Friend.c_str(), MAX_LOGIN);
            std::cout << "Coordinates (row column): "; 
            std::cin >> s.row >> s.col;
            sendPacket(s);
        } 
        else if (c == 3) { 
            isRunning = false; 
            p.cmd = RequestType::QUIT; 
            sendPacket(p); 
        }
        else if (c == 4) {
            Packet s; memset(&s, 0, sizeof(s));
            s.cmd = RequestType::GET_STATS;
            strncpy(s.login, login.c_str(), MAX_LOGIN);
            sendPacket(s);
        }
    }
    pthread_join(listenerThread, NULL);
}

void ClientApp::listenLoop() {
    NamedPipe mp(CLIENT_FIFO_PREFIX + login);
    mp.CreatePipe();
    while (isRunning) {
        if (mp.OpenPipe(O_RDONLY)) {
            Packet pkt;
            if (mp.Receive(&pkt, sizeof(Packet))) {
                if (pkt.cmd == RequestType::JOIN_GAME) {
                    Friend = pkt.login;
                    std::cout << "\nGame start. Your friend: " << Friend << std::endl;
                } else if (pkt.cmd == RequestType::SHOT) {
                    if (pkt.res == RES_REPEAT) {
                        if (std::string(pkt.login) == login) {
                            std::cout << "\n[!] You already shot here!" << std::endl;
                        }
                        continue;
                    }
                    if (std::string(pkt.target) == login) {
                        myBoard.cells[pkt.row][pkt.col] = pkt.is_hit ? HIT : MISS;
                    } else {
                        friendBoard.cells[pkt.row][pkt.col] = pkt.is_hit ? HIT : MISS;
                    }
                    
                    displayBoards();
                    bool iAmShooter = (std::string(pkt.login) == login);
                    if (iAmShooter) {
                        if (pkt.res == RES_HIT) {
                            std::cout << "HIT! KEEP SHOOTING." << std::endl;
                        } else if (pkt.res == RES_SUNK) {
                            std::cout << "KILLED! KEEP SHOOTING." << std::endl;
                        } else if (pkt.res == RES_MISS) {
                            std::cout << "MISTAKE! YOUR FRIEND IS SHOOTING." << std::endl;
                        }
                    } else {
                        if (pkt.is_hit) {
                            std::cout << "GOT HIT ON YOU! FRIEND SHOOTS AGAIN." << std::endl;
                        } else {
                            std::cout << "FRIEND MISSED! YOUR SHOT." << std::endl;
                        }
                    }
                    if (pkt.res == RES_LOSE) {
                        std::string winner = pkt.your_turn ? login : Friend;
                        std::cout << "GAME OVER! WINNER:" << winner << std::endl;
                        Friend = "";
                    }
                }
                else if (pkt.cmd == RequestType::GET_STATS) {
                    std::cout << "\nYOUR STATISTICS" << std::endl;
                    std::cout << "Player: " << pkt.login << std::endl;
                    std::cout << "Winners: " << pkt.row << std::endl;
                    std::cout << "Looses: " << pkt.col << std::endl;
                }
            }
            mp.ClosePipe();
        }
    }
}