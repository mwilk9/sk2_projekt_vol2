#include "game.h"
#include <stdlib.h>
#include <stdio.h>


Game::Game(Team *teams[2]){
    for (int k = 0; k<2;k++){
        players[k].t = teams[k];
        generateBoard(&players[k]);
        //printBoard(&players[k]);
    }
}

Game::~Game(){

}

/*
 * init playboard for a player
 */
void Game::generateBoard(Player *p){
    int i = 4;
    int j;
    int a=0;
    while (i){
        j=5-i;
        for (int k = 0; k<j; k++){
            randomAllocation(a,i, p);
        }
        i--;
    }
}


/*
 * check if chosen point(x,y) is not already taken by other ship
 */
bool Game::free(int x, int y, vector<Ship::Square> *squares){
    for (unsigned int i = 0; i< squares->size(); i++)
        if ((*squares)[i].x == x && (*squares)[i].y==y) return false;
    return true;
}


/*
 * check if chosen point(x,y) can be taken by a ship
 * */
bool Game::canAllocate(Player *p, int x, int y, vector<Ship::Square> *squares){
    if (x<0 || x>9 || y<0 || y>9 ) return false; //jeżeli pole jest poza granicami
    if (p->matrix[x][y].status == ship || p->matrix[x][y].status == nextToShip) return false; //jeżeli pole jest już zajęte lub obok jest statek
    return free(x,y,squares);
}


/*
 * randomly choose places for a ship's parts
 * */
bool Game::allocate(Game::Player *p, int x0, int y0, int size, vector<Ship::Square> &squares){
    if (!size) return true;
    int x, y;
    Ship::Square sq;
    vector<Ship::Square> s;
    for (int j=0;j<4;j++)
        s.push_back(sq);
    s[0].x=x0-1;
    s[1].x=x0+1;
    s[2].x=x0;
    s[3].x=x0;
    s[2].y=y0-1;
    s[3].y=y0+1;
    s[0].y=y0;
    s[1].y=y0;
    while (s.size()){
        int r = rand()%s.size();
        if (canAllocate(p,s[r].x, s[r].y, &squares)) {
            x=s[r].x;
            y=s[r].y;
            squares.push_back(s[r]);
            return allocate(p, x,y,size-1, squares);
        }
       else s.erase(s.begin() + r);
    }
    return false;
}

/*
 * set squares around ship's part as nextToShip, so no ships could be placed next to each other
 * */
void Game::surroundAShip(Player * p, int x, int y){
    for(int i =x-1;i<=x+1;i++)
        for(int j =y-1;j<=y+1;j++)
            if(i<10 && j<10 && i>=0 && j>=0)
                p->matrix[i][j].status=nextToShip;
}

/*
 * generate random allocation for a ship
 * */
void Game::randomAllocation(int &offset, int size, Player *p){
    Ship::Square s;
    vector<Ship::Square> as;
    Ship *newShip = new Ship(size);
    while (true){
        int xx = rand()%10;
        int yy = rand()%10;
        if (p->matrix[xx][yy].status != ship && p->matrix[xx][yy].status != nextToShip){
            s.x = xx;
            s.y = yy;
            as.push_back(s);
            if (!allocate(p,xx,yy,size-1,as))  {
                as.clear();
            }
            else{

                for (unsigned int i=0;i<as.size();i++)
                     surroundAShip(p, as[i].x,as[i].y);
                for (unsigned int i=0;i<as.size();i++){
                    p->matrix[as[i].x][as[i].y].status = ship;
                    p->matrix[as[i].x][as[i].y].ship = newShip;
                    newShip->addSquare(as[i].x,as[i].y);
                    p->ships[2*offset]=as[i].x+'0';
                    p->ships[2*offset+1] = as[i].y+'0';
                    offset++;
                }
                break;

            }
        }
    }

}


/*
 * return result of player's move
 * */
Game::Result Game::makeAMove(int x, int y, Player* p, Ship *& s){
    if (p->matrix[x][y].status == ship){
        p->matrix[x][y].status=shipHit;
        if (!p->matrix[x][y].ship->attack()) {
            s = p->matrix[x][y].ship;
            if (p->t->attackShip()) return sunk;
            else return sunkAll;
        }
        else if(!p->t->attackShip()) return sunkAll;
        else return hit;
    }
    return miss;
}


/*
 * for debbuging/testing purposes - prints a board with allocated ships
 * */
void Game::printBoard(Player *p){
    printf("****:\n");
    for (int i =0;i<10; i++){
        for (int j =0; j<10; j++)
            printf("%u\t", p->matrix[j][i].status);
        printf("\n");
        }

}

/*
 * return chosen player
 * */
Game::Player * Game::getPlayer(int nr){
    return (&players[nr]);
}
