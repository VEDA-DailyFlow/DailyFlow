#include "mainwindow.h"
#include "homepage.h"
#include "schedulepage.h"
#include "settingspage.h"

#include <QMessageBox>

MainWindow::MainWindow(const QString &userId, QWidget *parent)
    : QMainWindow(parent)
    , m_userId(userId)
    , m_userName("엄도윤") // TODO: DataManager에서 실제 사용자 이름 가져오기
{
    setupUI();
    showHomePage(); // 시작 시 홈 페이지 표시
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // 메인 윈도우 설정
    setWindowTitle("DailyFlow");
    setMinimumSize(800, 600);

    // 중앙 위젯 생성
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    // 메인 레이아웃
    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 상단바 생성
    setupTopBar();
    m_mainLayout->addWidget(m_topBar);

    // 스택 위젯 생성 (페이지 전환용)
    m_stackedWidget = new QStackedWidget(this);

    // 각 페이지 생성
    m_homePage = new HomePage(m_userId, this);
    m_schedulePage = new SchedulePage(m_userId, this);
    m_settingsPage = new SettingsPage(m_userId, this);

    // 스택 위젯에 페이지 추가
    m_stackedWidget->addWidget(m_homePage);
    m_stackedWidget->addWidget(m_schedulePage);
    m_stackedWidget->addWidget(m_settingsPage);

    m_mainLayout->addWidget(m_stackedWidget, 1); // stretch factor 1로 남은 공간 모두 차지

    // 하단 네비게이션바 생성
    setupNavigationBar();
    m_mainLayout->addWidget(m_navigationBar);
}

void MainWindow::setupTopBar()
{
    m_topBar = new QWidget(this);
    m_topBar->setStyleSheet("QWidget { background-color: #2196F3; padding: 10px; }");
    m_topBar->setFixedHeight(60);

    QHBoxLayout *topLayout = new QHBoxLayout(m_topBar);

    // 타이틀
    m_titleLabel = new QLabel("DailyFlow", m_topBar);
    m_titleLabel->setStyleSheet("QLabel { color: white; font-size: 20px; font-weight: bold; }");
    topLayout->addWidget(m_titleLabel);

    topLayout->addStretch();

    // 사용자 이름
    m_userLabel = new QLabel(m_userName + "님", m_topBar);
    m_userLabel->setStyleSheet("QLabel { color: white; font-size: 14px; margin-right: 10px; }");
    topLayout->addWidget(m_userLabel);

    // 로그아웃 버튼
    m_logoutButton = new QPushButton("로그아웃", m_topBar);
    m_logoutButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #da190b;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #b71c1c;"
        "}"
        );
    m_logoutButton->setCursor(Qt::PointingHandCursor);
    topLayout->addWidget(m_logoutButton);
}

void MainWindow::setupNavigationBar()
{
    m_navigationBar = new QWidget(this);
    m_navigationBar->setStyleSheet("QWidget { background-color: #f5f5f5; border-top: 1px solid #ddd; }");
    m_navigationBar->setFixedHeight(70);

    QHBoxLayout *navLayout = new QHBoxLayout(m_navigationBar);
    navLayout->setContentsMargins(0, 0, 0, 0);
    navLayout->setSpacing(0);

    // 공통 버튼 스타일
    QString buttonStyle =
        "QPushButton {"
        "   border: none;"
        "   background-color: transparent;"
        "   color: #666;"
        "   font-size: 14px;"
        "   padding: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #e0e0e0;"
        "}"
        "QPushButton:checked {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   font-weight: bold;"
        "}";

    // 홈 버튼
    m_homeButton = new QPushButton("🏠\n홈", m_navigationBar);
    m_homeButton->setCheckable(true);
    m_homeButton->setStyleSheet(buttonStyle);
    m_homeButton->setCursor(Qt::PointingHandCursor);
    m_homeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_homeButton, &QPushButton::clicked, this, &MainWindow::showHomePage);
    navLayout->addWidget(m_homeButton);

    // 일정 버튼
    m_scheduleButton = new QPushButton("📅\n일정", m_navigationBar);
    m_scheduleButton->setCheckable(true);
    m_scheduleButton->setStyleSheet(buttonStyle);
    m_scheduleButton->setCursor(Qt::PointingHandCursor);
    m_scheduleButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_scheduleButton, &QPushButton::clicked, this, &MainWindow::showSchedulePage);
    navLayout->addWidget(m_scheduleButton);

    // 설정 버튼
    m_settingsButton = new QPushButton("⚙️\n설정", m_navigationBar);
    m_settingsButton->setCheckable(true);
    m_settingsButton->setStyleSheet(buttonStyle);
    m_settingsButton->setCursor(Qt::PointingHandCursor);
    m_settingsButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(m_settingsButton, &QPushButton::clicked, this, &MainWindow::showSettingsPage);
    navLayout->addWidget(m_settingsButton);
}

void MainWindow::showHomePage()
{
    m_stackedWidget->setCurrentWidget(m_homePage);

    // 버튼 상태 업데이트
    m_homeButton->setChecked(true);
    m_scheduleButton->setChecked(false);
    m_settingsButton->setChecked(false);
}

void MainWindow::showSchedulePage()
{
    m_stackedWidget->setCurrentWidget(m_schedulePage);

    // 버튼 상태 업데이트
    m_homeButton->setChecked(false);
    m_scheduleButton->setChecked(true);
    m_settingsButton->setChecked(false);
}

void MainWindow::showSettingsPage()
{
    m_stackedWidget->setCurrentWidget(m_settingsPage);

    // 버튼 상태 업데이트
    m_homeButton->setChecked(false);
    m_scheduleButton->setChecked(false);
    m_settingsButton->setChecked(true);
}

