#ifndef CLIENT_H
#define CLIENT_H


class client
{
public:
    client(int fd);
    void setAnswer(bool value);

    bool getAnswer() const;

    int getFd() const;
    void setFd(int value);
    void setXY(int x0, int y0);

    int getX() const;

    int getY() const;

private:
    int fd;
    int x;
    int y;
    bool answer;
};

#endif // CLIENT_H
