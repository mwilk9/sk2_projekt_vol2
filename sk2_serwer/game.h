#ifndef GAME_H
#define GAME_H


#include "team.h"
#include "ship.h"

class Game
{
public:
    enum Status{
        noShip,
        ship,
        nextToShip,
        shipHit,
    };
    enum Result{
        sunkAll,
        sunk,
        hit,
        miss,
    };
    struct Point{
        Status status;
        Ship *ship = nullptr;
    };
    struct Player{
        Team *t;
        Point matrix[10][10];
        char ships[40];
    };

    Game(Team *teams[2]);
    ~Game();
    Result makeAMove(int x, int y, Player *p, Ship *&ship);
    void generateBoard(Player *p);
    void getSquare();
    void randomAllocation(int &nr, int size, Player *p);
    bool allocate(Player *p,int x0, int y0, int size, vector<Ship::Square> &squares);
    bool canAllocate(Player* p,int x, int y,vector<Ship::Square> *squares);
    void surroundAShip(Player * p, int x, int y);
    void printBoard(Player *p);
    bool free(int x, int y, vector<Ship::Square> *squares);
    Player * getPlayer(int nr);
private:
    Player players[2];

};

#endif // GAME_H
