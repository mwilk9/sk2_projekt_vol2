#include "team.h"

Team::Team(){

}

Team::~Team(){
 }

void Team::addClient(client c){
    if (this->clients.size() < 5)
        this->clients.push_back(c);
}



void Team::removeClient(int fd){
    int id = this->hasClient(fd);
    clients.erase(clients.begin()+id);
}

int Team::getSize(){
    return clients.size();
}

/*
 * Decrease remaining ship's part of the team
 * if returns 0 - game over
 */
int Team::attackShip(){
    this->remainingShips--;
    return this->remainingShips;
}


vector<client> Team::getClients() const{
    return clients;
}

client Team::getClient(int id){
    return clients[id];
}

void Team::setClients(const vector<client> &value){
    clients = value;
}

void Team::clearAnswers()
{
    for (int i=0;i<clients.size();i++)
        clients[i].setAnswer(0);
}

int Team::hasClient(int fd){
    int res = -1;
    for (int i=0;i<clients.size();i++){
        if (clients[i].getFd() == fd) {
            res = i;
            break;
        }
    }
    return res;
}

void Team::setClientXY(int id, int x, int y){
    this->clients[id].setXY(x, y);
}
