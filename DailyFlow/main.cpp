#include "mainwindow.h"
#include "envloader.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if(!EnvLoader::load(".env")){
        qWarning() << ".env file not found or invalid!";
    }
    MainWindow w("sss");
    w.show();
    return a.exec();
}
