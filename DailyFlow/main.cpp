#include "mainwindow.h"
#include "logindialog.h"
#include "joindialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // LoginDialog-----------------------------------
    // LoginDialog login;
    // login.show();

    // JoinDialog------------------------------------
    JoinDialog join;
    join.show();

    // MainWindow------------------------------------
    // if (login.exec() == QDialog::Accepted) {
    //         MainWindow w;
    //         w.show();
    //         return a.exec();
    //     }

    return a.exec();
}
