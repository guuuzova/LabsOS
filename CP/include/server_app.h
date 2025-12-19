#pragma once

#include "common.h"
#include "pipes.h"

#include <pthread.h>
#include <string>
#include <vector>

class ServerApp {
public:
  ServerApp();
  ~ServerApp();
  void run();

private:
  std::vector<Player> players;
  std::vector<GameSession> activeGames;
  pthread_mutex_t list_mutex;
  NamedPipe serverPipe;
  bool isRunning;

  Player *findPlayer(const std::string &login);
  void sendToClient(const std::string &login, Packet &pkt);
  void loadStatusFromFile();
  void saveStatsToFile();
  void handleCreateGame(Packet &pkt);
  void handleJoinGame(Packet &pkt);
  void handleLogin(Packet &pkt);
  void handlePlaceShips(Packet &pkt);
  void hanldeShoot(Packet &pkt);
  void handleLogout(Packet &pkt);
  void handleGetStats(Packet &pkt);
};