#include "homepage.h"
#include "ui_homepage.h"

HomePage::HomePage(const QString &userId, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
    , m_userId(userId)
{
    ui->setupUi(this);

    loadAISummary();
    loadUpcomingSchedules();
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::loadAISummary()
{
    // TODO: AIService를 통해 실제 AI 요약 가져오기
    ui->aiSummaryText->setText(
        "안녕하세요 도윤님! 😊\n\n"
        "오늘은 구름 많고 기온 12도로 쌀쌀합니다. 외출 시 따뜻하게 입으세요!\n"
        "오늘 일정이 없으니 여유롭게 보내시면 좋을 것 같아요."
        );
}

void HomePage::loadUpcomingSchedules()
{
    // TODO: DataManager를 통해 실제 일정 데이터 가져오기
    ui->scheduleList->addItem("📌 11/20 (수) 14:00 - 한화비전 미팅");
    ui->scheduleList->addItem("📌 11/21 (목) 10:00 - VEDA 프로젝트 발표");
    ui->scheduleList->addItem("📌 11/22 (금) 15:00 - 알고리즘 스터디");
}
