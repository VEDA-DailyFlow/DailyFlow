#include "mainwindow.h"
#include "envloader.h"
#include "logindialog.h"
#include "datamanager.h"
#include <QDialog>
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

    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        int userId = loginDialog.getLoggedInUserId();
        qDebug() << "로그인 성공! User ID:" << userId;
        MainWindow w(userId);
        w.show();
        return a.exec();
    }
    else
    {
        qDebug() << "로그인 실패. 프로그램 종료";
        return 0;
    }
}
