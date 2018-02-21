#ifndef WIDGET_H
#define WIDGET_H
#include <QWidget>
#include <vector>
#include <QTcpSocket>
#include <QTimer>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QDebug>
#include <QMouseEvent>
#include "graphicsscene.h"
#include "dialog.h"
#include "ui_widget.h"


using namespace std;
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);

    ~Widget();
    void mousePressEvent (QMouseEvent * ev);
    void connectToServer(int option);
    void startGame();
    void gameInit();
    void drawShips();

    void getMsg();
    int sendData(int x, int y);
    void connn();
    void connected();
    void disconnected();
    void gameOver(bool win);
    void socketError();
    void end();

private slots:
    void timeEv();

private:
    Ui::Widget *ui;

    int x;
    int y;
    GraphicsScene * scene;
    GraphicsScene * opponentScene;
    Dialog *dialog;
    bool endOfGame = false;
    QTcpSocket * sock;
    char message[9];
    bool yourTurn;
    char *server = "127.0.0.1";
    char *protocol = "tcp";
    short service_port = 5558;
    QTimer *timer;
    int const sec = 60;
    int seconds = 60;
    int milisec = 1000;
    char shipd[40];
    bool game_end;
};

#endif // WIDGET_H
