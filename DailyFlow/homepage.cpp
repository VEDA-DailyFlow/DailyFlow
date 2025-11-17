#include "homepage.h"
#include <QGroupBox>

HomePage::HomePage(const QString &userId, QWidget *parent)
    : QWidget(parent)
    , m_userId(userId)
{
    setupUI();
    loadAISummary();
    loadUpcomingSchedules();
}

void HomePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // AI 오늘 일정 요약 섹션
    QGroupBox *aiSummaryBox = new QGroupBox("📌 AI 오늘 일정 요약", this);
    aiSummaryBox->setStyleSheet(
        "QGroupBox {"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   border: 2px solid #2196F3;"
        "   border-radius: 8px;"
        "   margin-top: 10px;"
        "   padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 10px;"
        "   padding: 0 5px;"
        "}"
        );

    QVBoxLayout *aiLayout = new QVBoxLayout(aiSummaryBox);
    m_aiSummaryText = new QTextEdit(this);
    m_aiSummaryText->setReadOnly(true);
    m_aiSummaryText->setMaximumHeight(120);
    m_aiSummaryText->setStyleSheet(
        "QTextEdit {"
        "   background-color: #f0f8ff;"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "   padding: 10px;"
        "   font-size: 13px;"
        "}"
        );
    m_aiSummaryText->setText("AI 요약을 불러오는 중...");
    aiLayout->addWidget(m_aiSummaryText);

    mainLayout->addWidget(aiSummaryBox);

    // 하단 섹션 (일정 목록 + 상세 정보)
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(15);

    // 좌측: 예정된 일정 목록
    QVBoxLayout *leftLayout = new QVBoxLayout();

    QGroupBox *scheduleListBox = new QGroupBox("📅 예정된 일정 (7일)", this);
    scheduleListBox->setStyleSheet(
        "QGroupBox {"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border: 1px solid #ddd;"
        "   border-radius: 8px;"
        "   margin-top: 10px;"
        "   padding-top: 10px;"
        "}"
        );

    QVBoxLayout *listLayout = new QVBoxLayout(scheduleListBox);
    m_scheduleList = new QListWidget(this);
    m_scheduleList->setStyleSheet(
        "QListWidget {"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "   background-color: white;"
        "}"
        "QListWidget::item {"
        "   padding: 8px;"
        "   border-bottom: 1px solid #f0f0f0;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #e3f2fd;"
        "   color: black;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #f5f5f5;"
        "}"
        );
    listLayout->addWidget(m_scheduleList);
    leftLayout->addWidget(scheduleListBox);

    bottomLayout->addLayout(leftLayout, 1);

    // 우측: 선택한 일정 상세 정보
    QVBoxLayout *rightLayout = new QVBoxLayout();

    QGroupBox *detailBox = new QGroupBox("📝 일정 상세", this);
    detailBox->setStyleSheet(
        "QGroupBox {"
        "   font-size: 14px;"
        "   font-weight: bold;"
        "   border: 1px solid #ddd;"
        "   border-radius: 8px;"
        "   margin-top: 10px;"
        "   padding-top: 10px;"
        "}"
        );

    QVBoxLayout *detailLayout = new QVBoxLayout(detailBox);

    m_scheduleDetail = new QTextEdit(this);
    m_scheduleDetail->setReadOnly(true);
    m_scheduleDetail->setStyleSheet(
        "QTextEdit {"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "   padding: 10px;"
        "   background-color: white;"
        "   font-size: 13px;"
        "}"
        );
    m_scheduleDetail->setText("일정을 선택하세요.");
    detailLayout->addWidget(m_scheduleDetail);

    // 버튼 레이아웃
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    m_editButton = new QPushButton("수정", this);
    m_editButton->setEnabled(false);
    m_editButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover:enabled {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #cccccc;"
        "}"
        );

    m_deleteButton = new QPushButton("삭제", this);
    m_deleteButton->setEnabled(false);
    m_deleteButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   border: none;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover:enabled {"
        "   background-color: #da190b;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #cccccc;"
        "}"
        );

    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();

    detailLayout->addLayout(buttonLayout);
    rightLayout->addWidget(detailBox);

    bottomLayout->addLayout(rightLayout, 1);

    mainLayout->addLayout(bottomLayout, 1);
}

void HomePage::loadAISummary()
{
    // TODO: AIService를 통해 실제 AI 요약 가져오기
    m_aiSummaryText->setText(
        "안녕하세요 도윤님! 😊\n\n"
        "오늘은 구름 많고 기온 12도로 쌀쌀합니다. 외출 시 따뜻하게 입으세요!\n"
        "오늘 일정이 없으니 여유롭게 보내시면 좋을 것 같아요."
        );
}

void HomePage::loadUpcomingSchedules()
{
    // TODO: DataManager를 통해 실제 일정 데이터 가져오기
    m_scheduleList->addItem("📌 11/20 (수) 14:00 - 한화비전 미팅");
    m_scheduleList->addItem("📌 11/21 (목) 10:00 - VEDA 프로젝트 발표");
    m_scheduleList->addItem("📌 11/22 (금) 15:00 - 알고리즘 스터디");
}
