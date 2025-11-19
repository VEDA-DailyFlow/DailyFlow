#include "mainwindow.h"
#include "envloader.h"
#include "logindialog.h"
#include "datamanager.h"
#include "aiservice.h"
#include <QDialog>
#include <QApplication>
#include <QEventLoop>  // MainWindow가 닫힐 때까지 대기하기 위해
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(!EnvLoader::load(".env")){
        qWarning() << ".env file not found or invalid!";
    }

    // DataManager, ai service 싱글톤 인스턴스 생성
    DataManager::instance();
    AIService::instance();

    bool restartLoop = true; // 로그아웃 시 루프를 다시 시작하기 위한 플래그

    a.setWindowIcon(QIcon(":/logo.png"));  // 프로그램 실행 아이콘
    while (restartLoop)
    {
        LoginDialog loginDialog;

        if (loginDialog.exec() == QDialog::Accepted) {

            // 로그인 성공 시
            int userId = loginDialog.getLoggedInUserId();
            qDebug() << "로그인 성공! User ID:" << userId;

            MainWindow w(userId); // MainWindow 생성

            // MainWindow가 닫힐 때까지 대기하는 로컬 이벤트 루프
            QEventLoop loop;
            QObject::connect(&w, &MainWindow::windowClosed, &loop, &QEventLoop::quit);
            w.show();
            loop.exec(); // windowClosed 신호가 올 때까지 대기

            // MainWindow가 닫힌 이유를 확인
            if (w.isLogout()) {
                restartLoop = true; // 로그아웃 하였을 때
                qDebug() << "로그아웃. 로그인 창으로 돌아갑니다.";
            } else {
                restartLoop = false; // 로그인 후 "X" 버튼으로 창을 닫았을 때
                qDebug() << "메인 창 닫음. 프로그램 종료.";
            }
        }
        else
        {
            qDebug() << "로그인하지 않고 프로그램 종료";
            restartLoop = false; // while 루프 종료
        }
    }

    return 0; // 프로그램 완전 종료
}
