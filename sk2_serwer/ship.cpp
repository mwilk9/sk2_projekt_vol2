#include "ship.h"

Ship::Ship(int size){
    this->notSunk = size;
    this->size = size;
    points = new Square[size];
    this->offset = 0;
}

Ship::~Ship(){
    delete points;
}

/*
* Adds coordinates of sqared on the board the ship is placed on (placed at?)
*/
void Ship::addSquare(int x, int y){
    points[offset].x = x;
    points[offset].y = y;
    offset++;
}

Ship::Square *Ship::getPoints() const{
    return points;
}

/*
 * Decreases the number of parts of the ship that weren't hit yet
 */
int Ship::attack(){
    notSunk--;
    return notSunk;
}

int Ship::getSize() const{
    return size;
}




