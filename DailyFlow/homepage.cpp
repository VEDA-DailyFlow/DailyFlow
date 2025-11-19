#include "homepage.h"
#include "ui_homepage.h"
#include "datamanager.h"
#include "aiservice.h"
#include "scheduledialog.h"
#include <QDate>
#include <QMessageBox>
#include <QSettings>

HomePage::HomePage(int userId, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
    , m_userId(userId)
{
    ui->setupUi(this);
    ui->refreshButton->setFixedSize(80, 30);
    ui->aiTabWidget->setCornerWidget(ui->refreshButton, Qt::TopRightCorner);

    // 시그널-슬롯 연결
    connect(ui->scheduleList, &QListWidget::itemClicked,
            this, &HomePage::onScheduleItemClicked);
    connect(ui->editButton, &QPushButton::clicked,
            this, &HomePage::onEditButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked,
            this, &HomePage::onDeleteButtonClicked);
    connect(ui->refreshButton, &QPushButton::clicked,
            this, &HomePage::onRefreshButtonClicked);

    // ★ 초기 테마 적용 (저장된 설정 불러오기)
    QSettings settings("DailyFlow", "Settings");
    bool isDarkMode = settings.value("darkMode", false).toBool();
    applyTheme(isDarkMode);

    loadAISummary();
    loadFortune();
    loadUpcomingSchedules();
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::loadAISummary()
{
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString summary = DataManager::instance().getCachedSummary(m_userId, today);

    if (summary.isEmpty()) {
        ui->aiSummaryText->setText("AI 일정 요약을 생성하는 중...");

        summary = AIService::instance().generateDailySummary(m_userId);

        if (!summary.isEmpty()) {
            ui->aiSummaryText->setText(summary);
        } else {
            ui->aiSummaryText->setText("AI 일정 요약 생성에 실패했습니다.");
        }
    } else {
        ui->aiSummaryText->setText(summary);
    }
}

void HomePage::loadFortune()
{
    ui->fortuneText->setText("오늘의 운세를 생성하는 중...");

    QString fortune = AIService::instance().generateTodaysFortune(m_userId);

    if (!fortune.isEmpty()) {
        ui->fortuneText->setText(fortune);
    } else {
        ui->fortuneText->setText("오늘의 운세 생성에 실패했습니다.");
    }
}

void HomePage::refreshSchedules()
{
    loadUpcomingSchedules();
    loadAISummary();
}

void HomePage::loadUpcomingSchedules()
{
    ui->scheduleList->clear();
    m_itemToScheduleId.clear();

    QList<QVariantMap> schedules = DataManager::instance().getSchedulesForNextDays(m_userId, 7);

    if (schedules.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem("일정이 없습니다.");
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        ui->scheduleList->addItem(item);
        return;
    }

    for (const QVariantMap &schedule : schedules) {
        QString date = schedule["date"].toString();
        QString startTime = schedule["startTime"].toString();
        QString title = schedule["title"].toString();
        int scheduleId = schedule["id"].toInt();

        QDate qDate = QDate::fromString(date, "yyyy-MM-dd");
        QString dayOfWeek = QLocale(QLocale::Korean).dayName(qDate.dayOfWeek(), QLocale::ShortFormat);
        QString formattedDate = qDate.toString("MM/dd");
        QString formattedTime = startTime.left(5);

        QString itemText = QString("📌 %1 (%2) %3 - %4")
                               .arg(formattedDate)
                               .arg(dayOfWeek)
                               .arg(formattedTime)
                               .arg(title);

        QListWidgetItem *item = new QListWidgetItem(itemText);
        ui->scheduleList->addItem(item);

        m_itemToScheduleId[item] = scheduleId;
    }
}

void HomePage::onScheduleItemClicked(QListWidgetItem *item)
{
    if (!(item->flags() & Qt::ItemIsSelectable)) {
        return;
    }

    int scheduleId = m_itemToScheduleId.value(item, -1);
    if (scheduleId == -1) {
        return;
    }

    displayScheduleDetail(scheduleId);

    ui->editButton->setEnabled(true);
    ui->deleteButton->setEnabled(true);
}

void HomePage::displayScheduleDetail(int scheduleId)
{
    QVariantMap schedule = DataManager::instance().getScheduleById(scheduleId);

    if (schedule.isEmpty()) {
        ui->scheduleDetail->setText("일정 정보를 불러올 수 없습니다.");
        return;
    }

    QString html = QString(
                       "<h3 style='color: #2196F3; margin-bottom: 10px;'>%1</h3>"
                       "<p style='margin: 5px 0;'><b>📅 날짜:</b> %2</p>"
                       "<p style='margin: 5px 0;'><b>🕐 시간:</b> %3 ~ %4</p>"
                       "<p style='margin: 5px 0;'><b>📍 장소:</b> %5</p>"
                       "<p style='margin: 5px 0;'><b>🏷️ 카테고리:</b> %6</p>"
                       "<hr style='margin: 10px 0; border: none; border-top: 1px solid #ddd;'>"
                       "<p style='margin: 5px 0;'><b>📝 메모:</b></p>"
                       "<p style='margin: 5px 0; padding-left: 10px;'>%7</p>"
                       ).arg(schedule["title"].toString())
                       .arg(schedule["date"].toString())
                       .arg(schedule["startTime"].toString().left(5))
                       .arg(schedule["endTime"].toString().left(5))
                       .arg(schedule["location"].toString().isEmpty() ? "없음" : schedule["location"].toString())
                       .arg(schedule["category"].toString().isEmpty() ? "없음" : schedule["category"].toString())
                       .arg(schedule["memo"].toString().isEmpty() ? "없음" : schedule["memo"].toString());

    ui->scheduleDetail->setHtml(html);
}

void HomePage::onEditButtonClicked()
{
    QListWidgetItem *currentItem = ui->scheduleList->currentItem();
    if (!currentItem) return;

    int scheduleId = m_itemToScheduleId.value(currentItem, -1);
    if (scheduleId == -1) return;

    QVariantMap scheduleData = DataManager::instance().getScheduleById(scheduleId);
    ScheduleDialog dialog(scheduleData, this);

    if (dialog.exec() == QDialog::Accepted) {
        QVariantMap data = dialog.getScheduleData();

        bool success = DataManager::instance().updateSchedule(
            scheduleId,
            data["title"].toString(),
            data["date"].toString(),
            data["start_time"].toString(),
            data["end_time"].toString(),
            data["location"].toString(),
            data["memo"].toString(),
            data["category"].toString()
            );

        if (success) {
            QMessageBox::information(this, "수정 완료", "일정이 수정되었습니다.");
        } else {
            QMessageBox::warning(this, "오류", "일정 수정에 실패했습니다.");
        }
    }
}

void HomePage::onDeleteButtonClicked()
{
    QListWidgetItem *currentItem = ui->scheduleList->currentItem();
    if (!currentItem) {
        return;
    }

    int scheduleId = m_itemToScheduleId.value(currentItem, -1);
    if (scheduleId == -1) {
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "일정 삭제",
                                  "선택한 일정을 삭제하시겠습니까?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (DataManager::instance().deleteSchedule(scheduleId)) {
            QMessageBox::information(this, "삭제 완료", "일정이 삭제되었습니다.");

            loadUpcomingSchedules();

            ui->scheduleDetail->setText("일정을 선택하세요.");
            ui->editButton->setEnabled(false);
            ui->deleteButton->setEnabled(false);
        } else {
            QMessageBox::warning(this, "삭제 실패", "일정 삭제에 실패했습니다.");
        }
    }
}

void HomePage::onRefreshButtonClicked()
{
    // 현재 선택된 탭 확인
    int currentTab = ui->aiTabWidget->currentIndex();

    if (currentTab == 0) {
        // AI 일정 요약 탭
        QString today = QDate::currentDate().toString("yyyy-MM-dd");
        DataManager::instance().invalidateSummary(m_userId, today);

        ui->aiSummaryText->setText("AI 일정 요약을 새로 생성하는 중...");

        QString summary = AIService::instance().generateDailySummary(m_userId);

        if (!summary.isEmpty()) {
            ui->aiSummaryText->setText(summary);
            QMessageBox::information(this, "새로고침 완료", "AI 일정 요약이 갱신되었습니다.");
        } else {
            ui->aiSummaryText->setText("AI 일정 요약 생성에 실패했습니다.");
            QMessageBox::warning(this, "오류", "AI 일정 요약 생성에 실패했습니다.");
        }
    } else {
        // 오늘의 운세 탭
        ui->fortuneText->setText("오늘의 운세를 새로 생성하는 중...");

        QString fortune = AIService::instance().generateTodaysFortune(m_userId);

        if (!fortune.isEmpty()) {
            ui->fortuneText->setText(fortune);
            QMessageBox::information(this, "새로고침 완료", "오늘의 운세가 갱신되었습니다.");
        } else {
            ui->fortuneText->setText("오늘의 운세 생성에 실패했습니다.");
            QMessageBox::warning(this, "오류", "오늘의 운세 생성에 실패했습니다.");
        }
    }
}

void HomePage::applyTheme(bool isDarkMode)
{
    if (isDarkMode) {
        // 다크모드 스타일
        ui->aiSummaryBox->setStyleSheet(
            "QGroupBox {"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "   border: 2px solid #42A5F5;"
            "   border-radius: 8px;"
            "   margin-top: 10px;"
            "   padding-top: 10px;"
            "   color: white;"
            "}"
            "QGroupBox::title {"
            "   subcontrol-origin: margin;"
            "   left: 10px;"
            "   padding: 0 5px;"
            "}"
            );

        ui->aiTabWidget->setStyleSheet(
            "QTabWidget::pane {"
            "   border: 1px solid #555;"
            "   border-radius: 4px;"
            "   background-color: #2a2a2a;"
            "}"
            "QTabBar::tab {"
            "   background-color: #3a3a3a;"
            "   color: white;"
            "   padding: 8px 20px;"
            "   margin-right: 2px;"
            "   border-top-left-radius: 4px;"
            "   border-top-right-radius: 4px;"
            "}"
            "QTabBar::tab:selected {"
            "   background-color: #42A5F5;"
            "   color: white;"
            "   font-weight: bold;"
            "}"
            "QTabBar::tab:hover {"
            "   background-color: #64B5F6;"
            "   color: white;"
            "}"
            );

        ui->aiSummaryText->setStyleSheet(
            "QTextEdit {"
            "   background-color: #2a2a2a;"
            "   border: 1px solid #555;"
            "   border-radius: 4px;"
            "   padding: 10px;"
            "   font-size: 13px;"
            "   color: white;"
            "}"
            );

        ui->fortuneText->setStyleSheet(
            "QTextEdit {"
            "   background-color: #3a2a3a;"
            "   border: 1px solid #555;"
            "   border-radius: 4px;"
            "   padding: 10px;"
            "   font-size: 13px;"
            "   color: white;"
            "}"
            );

        ui->scheduleListBox->setStyleSheet(
            "QGroupBox {"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   border: 1px solid #555;"
            "   border-radius: 8px;"
            "   margin-top: 10px;"
            "   padding-top: 10px;"
            "   color: white;"
            "}"
            );

        ui->scheduleList->setStyleSheet(
            "QListWidget {"
            "   border: 1px solid #555;"
            "   border-radius: 4px;"
            "   background-color: #2a2a2a;"
            "   color: white;"
            "}"
            "QListWidget::item {"
            "   padding: 8px;"
            "   border-bottom: 1px solid #3a3a3a;"
            "}"
            "QListWidget::item:selected {"
            "   background-color: #1976D2;"
            "   color: white;"
            "}"
            "QListWidget::item:hover {"
            "   background-color: #3a3a3a;"
            "}"
            );

        ui->detailBox->setStyleSheet(
            "QGroupBox {"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   border: 1px solid #555;"
            "   border-radius: 8px;"
            "   margin-top: 10px;"
            "   padding-top: 10px;"
            "   color: white;"
            "}"
            );

        ui->scheduleDetail->setStyleSheet(
            "QTextEdit {"
            "   border: 1px solid #555;"
            "   border-radius: 4px;"
            "   padding: 10px;"
            "   background-color: #2a2a2a;"
            "   font-size: 13px;"
            "   color: white;"
            "}"
            );

    } else {
        // 라이트모드 스타일 (기존 UI 파일 스타일)
        ui->aiSummaryBox->setStyleSheet(
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

        ui->aiTabWidget->setStyleSheet(
            "QTabWidget::pane {"
            "   border: 1px solid #ddd;"
            "   border-radius: 4px;"
            "   background-color: #f0f8ff;"
            "}"
            "QTabBar::tab {"
            "   background-color: #e0e0e0;"
            "   padding: 8px 20px;"
            "   margin-right: 2px;"
            "   border-top-left-radius: 4px;"
            "   border-top-right-radius: 4px;"
            "}"
            "QTabBar::tab:selected {"
            "   background-color: #2196F3;"
            "   color: white;"
            "   font-weight: bold;"
            "}"
            "QTabBar::tab:hover {"
            "   background-color: #64B5F6;"
            "   color: white;"
            "}"
            );

        ui->aiSummaryText->setStyleSheet(
            "QTextEdit {"
            "   background-color: #f0f8ff;"
            "   border: 1px solid #ddd;"
            "   border-radius: 4px;"
            "   padding: 10px;"
            "   font-size: 13px;"
            "}"
            );

        ui->fortuneText->setStyleSheet(
            "QTextEdit {"
            "   background-color: #f3e5f5;"
            "   border: 1px solid #ddd;"
            "   border-radius: 4px;"
            "   padding: 10px;"
            "   font-size: 13px;"
            "}"
            );

        ui->scheduleListBox->setStyleSheet(
            "QGroupBox {"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   border: 1px solid #ddd;"
            "   border-radius: 8px;"
            "   margin-top: 10px;"
            "   padding-top: 10px;"
            "}"
            );

        ui->scheduleList->setStyleSheet(
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

        ui->detailBox->setStyleSheet(
            "QGroupBox {"
            "   font-size: 14px;"
            "   font-weight: bold;"
            "   border: 1px solid #ddd;"
            "   border-radius: 8px;"
            "   margin-top: 10px;"
            "   padding-top: 10px;"
            "}"
            );

        ui->scheduleDetail->setStyleSheet(
            "QTextEdit {"
            "   border: 1px solid #ddd;"
            "   border-radius: 4px;"
            "   padding: 10px;"
            "   background-color: white;"
            "   font-size: 13px;"
            "}"
            );
    }

    // 버튼들은 그대로 유지 (초록색, 빨간색)
}
