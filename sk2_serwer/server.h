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
    int sendData(int fd, Team *t);
    int waitForData(Team *t);
    bool rightTeam(Team *t, int fd);
    void closeConnection();



private:
    const int MAX_WAIT = 10;
    const int one = 1;
    Game *game;
    int x;
    int y;
    int threads = 0;
    int max_threads = 1;
    int servFd;
    int port = 5556;
    int res;
    int moveTeamA;
    const static int max_players=1;
    char message[10] ="123456789";
    char chosenOnes[3];
//    int pll;
    pollfd nacoczekac[2*max_players];
};

#endif // SERVER_H
