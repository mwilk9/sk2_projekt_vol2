#ifndef SERVER_H
#define SERVER_H
#include "game.h"
#include "team.h"
#include "ship.h"
#include "stdio.h"
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <poll.h>
#include <thread>
#include <unordered_set>
#include <signal.h>
#include <netinet/tcp.h>
#include <iostream>
#include <chrono>
#include <ctime>


class Server
{
public:
    enum msg{
        sunk=0,
        hit,
        miss,
        hidden,
        connected,
        waiting,
        yourMove,
        notYourMove,
        tmChoice,
        finalChoice,
        msgEnd,
        win,
        lose,
        ships,
    };
    Server();
    ~Server();
    void NewGame();
    void Play(Game *game);
    void ChooseSquare(int &x, int &y, Team * t);
    int initCon();
    int getNewClients(Team *, int, char*);
    void sendData(int fd, Team *t);
    int waitForData(Team *t);
    bool rightTeam(Team *t, int fd);
    void closeConnection();
    bool checkClients(Team *t, Team *t2);



private:
    const int one = 1;
    Game *game;
    int servFd;
    int port = 5558;
    const static int max_players=2;
    char message[10] ="123456789";
    char chosenOnes[3];
    pollfd nacoczekac[2*max_players];
};

#endif // SERVER_H
