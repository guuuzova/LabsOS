#pragma once

#include "common.h"
#include "pipes.h"

#include <pthread.h>
#include <string>

class ClientApp {
public:
  ClientApp();
  ~ClientApp() = default;
  void start();

private:
  std::string login;
  std::string Friend;
  bool isRunning;
  pthread_t listenerThread;
  Board myBoard;
  Board friendBoard;

  static void *listenThreadWrapper(void *ctx);
  void listenLoop();

  void sendPacket(Packet &pkt);
  void displayBoards();
};