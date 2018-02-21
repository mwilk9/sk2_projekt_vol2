#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),

    ui(new Ui::Dialog)
{
    ui->setupUi(this);
/*    connect(ui->accept,&QPushButton::clicked , this, &Dialog::setEmptyRoom);
    connect(ui->notEmptyRoomButton, &QPushButton::clicked, this, &Dialog::setJoin);*/
    //connect(ui->exitButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(ui->accept,SIGNAL(clicked()), this, SLOT(accept()));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(reject()));
}

Dialog::~Dialog(){
    delete ui;
}

void Dialog::setMessage(QString t){
    ui->label->setText(t);
}

void Dialog::setButtons(QString t1, QString t2){
    ui->accept->setText(t1);
    ui->cancel->setText(t2);
}

