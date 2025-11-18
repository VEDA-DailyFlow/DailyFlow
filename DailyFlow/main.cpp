#include "mainwindow.h"
#include "envloader.h"
#include "logindialog.h"
#include "joindialog.h"
#include "datamanager.h"
#include "aiservice.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(!EnvLoader::load(".env")){
        qWarning() << ".env file not found or invalid!";
    }
    // DataManager, ai service 싱글톤 인스턴스 생성
    DataManager::instance();
    AIService::instance();

    MainWindow w(123);
    w.show();
    // LoginDialog-----------------------------------
    // LoginDialog login;
    // login.show();

    // DataManager-----------------------------------


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
