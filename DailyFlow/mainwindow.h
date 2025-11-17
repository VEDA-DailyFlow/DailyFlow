#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class HomePage;
class SchedulePage;
class SettingsPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString &userId, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showHomePage();
    void showSchedulePage();
    void showSettingsPage();
    void handleLogout();

private:
    Ui::MainWindow *ui;
    QString m_userId;
    QString m_userName;

    // 페이지들
    HomePage *m_homePage;
    SchedulePage *m_schedulePage;
    SettingsPage *m_settingsPage;
};

#endif // MAINWINDOW_H
