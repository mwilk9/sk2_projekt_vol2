#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),

    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    connect(ui->emptyRoomButton,&QPushButton::clicked , this, &Dialog::setEmptyRoom);
    connect(ui->notEmptyRoomButton, &QPushButton::clicked, this, &Dialog::setJoin);
    connect(ui->exitButton, SIGNAL(clicked()), this, SLOT(reject()));
}

Dialog::~Dialog(){
    delete ui;
}

Dialog::Choice Dialog::getChoice(){
    if (exec()) return choice;
    else return Dialog::exit;
}

void Dialog::setEmptyRoom(){
    choice = emptyRoom;
    accept();
}

void Dialog::setJoin(){
    choice = join;
    accept();
}
