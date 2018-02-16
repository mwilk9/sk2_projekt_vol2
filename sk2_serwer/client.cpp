#include "client.h"

client::client(int fd){
    this->fd = fd;
    x = 0;
    y=0;
}

void client::setAnswer(bool value)
{
    answer = value;
}

bool client::getAnswer() const
{
    return answer;
}

int client::getFd() const
{
    return fd;
}

void client::setFd(int value)
{
    fd = value;
}

void client::setXY(int x0, int y0){
    this->x = x0;
    this->y = y0;
    this->setAnswer(true);
}

int client::getX() const
{
    return x;
}

int client::getY() const
{
    return y;
}
