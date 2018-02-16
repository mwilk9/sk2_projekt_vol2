#include "ship.h"

Ship::Ship(int size){
    this->size = size;
    points = new Point[size];
    notsunk = size;
}


void Ship::addPoint(int x, int y){
    points[count].x=x;
    points[count].y=y;
    count++;
}

Ship::Point *Ship::getPoints(){
    return points;
}

int Ship::getSize() const
{
    return size;
}
