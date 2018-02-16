#ifndef SHIP_H
#define SHIP_H


class Ship
{
public:
    Ship(int size);
    ~Ship();

    struct Square{
        int x;
        int y;
    };

    void addSquare(int x, int y);
    Square *getPoints() const;
    int attack();
    int getSize() const;


private:
    int size;
    int notSunk;
    Square * points;
    int offset;
};

#endif // SHIP_H
