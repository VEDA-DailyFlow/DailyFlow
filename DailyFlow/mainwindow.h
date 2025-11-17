#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

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
    void setupUI();
    void setupTopBar();
    void setupNavigationBar();

    QString m_userId;
    QString m_userName;

    // UI 컴포넌트
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;

    // 상단바
    QWidget *m_topBar;
    QLabel *m_titleLabel;
    QLabel *m_userLabel;
    QPushButton *m_logoutButton;

    // 메인 컨텐츠
    QStackedWidget *m_stackedWidget;
    HomePage *m_homePage;
    SchedulePage *m_schedulePage;
    SettingsPage *m_settingsPage;

    // 하단 네비게이션
    QWidget *m_navigationBar;
    QPushButton *m_homeButton;
    QPushButton *m_scheduleButton;
    QPushButton *m_settingsButton;
};

#endif // MAINWINDOW_H
