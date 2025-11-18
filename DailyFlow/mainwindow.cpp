#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "homepage.h"
#include "schedulepage.h"
#include "settingspage.h"
#include "datamanager.h"
#include <QMessageBox>
#include <QVariantMap>
#include <QTimer>


MainWindow::MainWindow(const int &userId, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_Id(userId)
{
    ui->setupUi(this);
    QVariantMap userInfo = DataManager::instance().getUserInfo(userId);

    // 오류 검출: 사용자 정보가 비어있는지 확인
    if (userInfo.isEmpty()) {
        qCritical() << "Failed to load user info for userId:" << userId;

        // 사용자에게 에러 메시지 표시
        QMessageBox::critical(this, "오류",
                              "사용자 정보를 불러올 수 없습니다.\n다시 로그인해주세요.");

        // MainWindow 닫고 로그인 화면으로 돌아가기
        QTimer::singleShot(0, this, [this]() {
            this->close();
            // LoginWindow *loginWindow = new LoginWindow();
            // loginWindow->show();
        });

        return;
    }

    m_userId = userInfo["username"].toString();
    m_userName = userInfo["name"].toString();
    // 사용자 이름 설정
    ui->userLabel->setText(m_userName + "님");

    // 각 페이지 생성
    m_homePage = new HomePage(m_Id, this);
    m_schedulePage = new SchedulePage(m_Id, this);
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

    connect(&DataManager::instance(), &DataManager::scheduleChanged,
            this, [this](int userId) {
                if (userId == m_Id) {
                    m_homePage->refreshSchedules();
                }
            });

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
