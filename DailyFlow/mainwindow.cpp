#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "homepage.h"
#include "schedulepage.h"
#include "settingspage.h"

#include <QMessageBox>

MainWindow::MainWindow(const QString &userId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_userId(userId)
    , m_userName("엄도윤") // TODO: DataManager에서 실제 사용자 이름 가져오기
{
    ui->setupUi(this);

    // 사용자 이름 설정
    ui->userLabel->setText(m_userName + "님");

    // 각 페이지 생성
    m_homePage = new HomePage(m_userId, this);
    m_schedulePage = new SchedulePage(m_userId, this);
    m_settingsPage = new SettingsPage(m_userId, this);

    // 스택 위젯에 페이지 추가
    ui->stackedWidget->addWidget(m_homePage);
    ui->stackedWidget->addWidget(m_schedulePage);
    ui->stackedWidget->addWidget(m_settingsPage);

    // 네비게이션 버튼 시그널 연결
    connect(ui->homeButton, &QPushButton::clicked, this, &MainWindow::showHomePage);
    connect(ui->scheduleButton, &QPushButton::clicked, this, &MainWindow::showSchedulePage);
    connect(ui->settingsButton, &QPushButton::clicked, this, &MainWindow::showSettingsPage);

    // 로그아웃 버튼 연결
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainWindow::handleLogout);

    // 시작 시 홈 페이지 표시
    showHomePage();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showHomePage()
{
    ui->stackedWidget->setCurrentWidget(m_homePage);

    // 버튼 상태 업데이트
    ui->homeButton->setChecked(true);
    ui->scheduleButton->setChecked(false);
    ui->settingsButton->setChecked(false);
}

void MainWindow::showSchedulePage()
{
    ui->stackedWidget->setCurrentWidget(m_schedulePage);

    // 버튼 상태 업데이트
    ui->homeButton->setChecked(false);
    ui->scheduleButton->setChecked(true);
    ui->settingsButton->setChecked(false);
}

void MainWindow::showSettingsPage()
{
    ui->stackedWidget->setCurrentWidget(m_settingsPage);

    // 버튼 상태 업데이트
    ui->homeButton->setChecked(false);
    ui->scheduleButton->setChecked(false);
    ui->settingsButton->setChecked(true);
}

void MainWindow::handleLogout()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "로그아웃",
                                  "로그아웃 하시겠습니까?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // TODO: 로그아웃 처리 및 로그인 화면으로 이동
        QMessageBox::information(this, "로그아웃", "로그아웃 기능은 추후 구현 예정입니다.");
    }
}
