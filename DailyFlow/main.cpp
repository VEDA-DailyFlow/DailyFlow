#include "mainwindow.h"
#include "envloader.h"
#include "logindialog.h"
#include "joindialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(!EnvLoader::load(".env")){
        qWarning() << ".env file not found or invalid!";
    }
    MainWindow w("sss");
    w.show();
    // LoginDialog-----------------------------------
    // LoginDialog login;
    // login.show();

    // JoinDialog------------------------------------
    // JoinDialog join;
    // join.show();

    // MainWindow------------------------------------
    // if (login.exec() == QDialog::Accepted) {
    //         MainWindow w;
    //         w.show();
    //         return a.exec();
    //     }

    return a.exec();
}
