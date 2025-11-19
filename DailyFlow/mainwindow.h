#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>

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
    MainWindow(const int &userId, QWidget *parent = nullptr);
    ~MainWindow();
    bool isLogout() const { return m_isLogout; }

signals:
    void windowClosed();  // main.cpp에 창이 닫혔다고 알릴 신호

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void showHomePage();
    void showSchedulePage();
    void showSettingsPage();
    void handleLogout();

private:
    Ui::MainWindow *ui;
    int m_Id;
    QString m_userId;
    QString m_userName;

    bool m_isLogout = false;

    // 페이지들
    HomePage *m_homePage;
    SchedulePage *m_schedulePage;
    SettingsPage *m_settingsPage;
};

#endif // MAINWINDOW_H
