#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    enum Choice{
        exit,
        emptyRoom,
        join
    };
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

    Choice getChoice();
    void setEmptyRoom();
    void setJoin();
private:
    Ui::Dialog *ui;
    Choice choice;

};

#endif // DIALOG_H
