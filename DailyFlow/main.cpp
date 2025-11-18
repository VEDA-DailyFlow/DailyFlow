#include "mainwindow.h"
#include "envloader.h"
#include "logindialog.h"
#include "joindialog.h"
#include "datamanager.h"
#include <QDialog>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(!EnvLoader::load(".env")){
        qWarning() << ".env file not found or invalid!";
    }
    // DataManager 싱글톤 인스턴스 생성
    DataManager::instance();

    LoginDialog loginDialog;

    // 로그인 창이 닫힐 때까지 기다리다가 로그인 성공 여부를 받음
    int result = loginDialog.exec();

    // // 로그인 성공 시에만 MainWindow를 띄움
    // if (result == QDialog::Accepted) {
    //     MainWindow *mainWin = new MainWindow();
    //     mainWin->setAttribute(Qt::WA_DeleteOnClose);
    //     mainWin->show();

    //     // mainWin이 떠야 하므로 a.exec()가 필요
    //     return a.exec();
    // }


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
