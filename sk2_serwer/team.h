#ifndef TEAM_H
#define TEAM_H

#include "client.h"
#include <vector>
using std::vector;
class Team
{
public:
    Team();
    ~Team();
    int attackShip();
    void addClient(client c);
    void removeClient(int);
    int getSize();
    vector<client> getClients() const;
    client getClient(int id);
    void setClients(const vector<client> &value);
    void clearAnswers();
    int hasClient(int fd);
    void setClientXY(int fd, int x, int y);
private:
    int remainingShips = 20;

    vector<client> clients;
};

#endif // TEAM_H
