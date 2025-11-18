#include "homepage.h"
#include "ui_homepage.h"
#include "datamanager.h"
#include "scheduledialog.h"
#include <QDate>
#include <QMessageBox>

HomePage::HomePage(int userId, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
    , m_userId(userId)
{
    ui->setupUi(this);

    // 시그널-슬롯 연결
    connect(ui->scheduleList, &QListWidget::itemClicked,
            this, &HomePage::onScheduleItemClicked);
    connect(ui->editButton, &QPushButton::clicked,
            this, &HomePage::onEditButtonClicked);
    connect(ui->deleteButton, &QPushButton::clicked,
            this, &HomePage::onDeleteButtonClicked);

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
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString summary = DataManager::instance().getCachedSummary(m_userId, today);

    if (summary.isEmpty()) {
        ui->aiSummaryText->setText("AI 요약을 불러오는 중...");
        // TODO: AIService로 요약 생성
    } else {
        ui->aiSummaryText->setText(summary);
    }
}

void HomePage::refreshSchedules()
{
    loadUpcomingSchedules();
    loadAISummary();
}

void HomePage::loadUpcomingSchedules()
{
    // 기존 데이터 클리어
    ui->scheduleList->clear();
    m_itemToScheduleId.clear();

    // 오늘부터 7일간의 일정 가져오기
    QList<QVariantMap> schedules = DataManager::instance().getSchedulesForNextDays(m_userId, 7);

    if (schedules.isEmpty()) {
        QListWidgetItem *item = new QListWidgetItem("일정이 없습니다.");
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);  // 선택 불가
        ui->scheduleList->addItem(item);
        return;
    }

    // 일정을 리스트에 추가
    for (const QVariantMap &schedule : schedules) {
        QString date = schedule["date"].toString();
        QString startTime = schedule["startTime"].toString();
        QString title = schedule["title"].toString();
        int scheduleId = schedule["id"].toInt();

        // 날짜 형식 변환: 2024-11-20 -> 11/20 (수)
        QDate qDate = QDate::fromString(date, "yyyy-MM-dd");
        QString dayOfWeek = QLocale(QLocale::Korean).dayName(qDate.dayOfWeek(), QLocale::ShortFormat);
        QString formattedDate = qDate.toString("MM/dd");

        // 시간 형식: HH:mm:ss -> HH:mm
        QString formattedTime = startTime.left(5);

        // 리스트 아이템 텍스트 생성
        QString itemText = QString("📌 %1 (%2) %3 - %4")
                               .arg(formattedDate)
                               .arg(dayOfWeek)
                               .arg(formattedTime)
                               .arg(title);

        QListWidgetItem *item = new QListWidgetItem(itemText);
        ui->scheduleList->addItem(item);

        // 아이템과 스케줄 ID 매핑 저장
        m_itemToScheduleId[item] = scheduleId;
    }
}

void HomePage::onScheduleItemClicked(QListWidgetItem *item)
{
    // 선택 불가능한 아이템인 경우 무시
    if (!(item->flags() & Qt::ItemIsSelectable)) {
        return;
    }

    // 스케줄 ID 가져오기
    int scheduleId = m_itemToScheduleId.value(item, -1);
    if (scheduleId == -1) {
        return;
    }

    // 상세 정보 표시
    displayScheduleDetail(scheduleId);

    // 버튼 활성화
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

    // 상세 정보 HTML 형식으로 표시
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

    // 일정 정보 가져오기
    QVariantMap scheduleData = DataManager::instance().getScheduleById(scheduleId);

    // ScheduleDialog 열기 (수정 모드)
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
            // DataManager가 scheduleChanged 시그널 발생
            // MainWindow가 자동으로 HomePage 갱신해줌
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

    // 삭제 확인
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "일정 삭제",
                                  "선택한 일정을 삭제하시겠습니까?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (DataManager::instance().deleteSchedule(scheduleId)) {
            QMessageBox::information(this, "삭제 완료", "일정이 삭제되었습니다.");

            // 리스트 새로고침
            loadUpcomingSchedules();

            // 상세 정보 초기화
            ui->scheduleDetail->setText("일정을 선택하세요.");
            ui->editButton->setEnabled(false);
            ui->deleteButton->setEnabled(false);
        } else {
            QMessageBox::warning(this, "삭제 실패", "일정 삭제에 실패했습니다.");
        }
    }
}
