#include "schedulepage.h"
#include "ui_schedulepage.h"
#include "scheduledialog.h"
#include "datamanager.h"
#include <QMessageBox>
#include <QScrollArea>

// ============================================================================
// CustomCalendar 구현 (변경 없음)
// ============================================================================

CustomCalendar::CustomCalendar(QWidget *parent)
    : QCalendarWidget(parent)
{
    setGridVisible(false);
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    QTextCharFormat headerFormat;
    headerFormat.setForeground(QBrush(QColor("#333333")));
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setFontPointSize(12);
    setHeaderTextFormat(headerFormat);

    QTextCharFormat weekendFormat;
    weekendFormat.setForeground(QBrush(QColor("#f44336")));
    weekendFormat.setFontWeight(QFont::Bold);
    weekendFormat.setFontPointSize(12);
    setWeekdayTextFormat(Qt::Saturday, weekendFormat);
    setWeekdayTextFormat(Qt::Sunday, weekendFormat);

    QTextCharFormat weekdayFormat;
    weekdayFormat.setForeground(QBrush(QColor("#333333")));
    weekdayFormat.setFontWeight(QFont::Bold);
    weekdayFormat.setFontPointSize(12);
    setWeekdayTextFormat(Qt::Monday, weekdayFormat);
    setWeekdayTextFormat(Qt::Tuesday, weekdayFormat);
    setWeekdayTextFormat(Qt::Wednesday, weekdayFormat);
    setWeekdayTextFormat(Qt::Thursday, weekdayFormat);
    setWeekdayTextFormat(Qt::Friday, weekdayFormat);

    setStyleSheet(
        "QCalendarWidget QWidget { "
        "   background-color: white; "
        "}"
        "QCalendarWidget QToolButton {"
        "   height: 35px;"
        "   width: 70px;"
        "   color: #2196F3;"
        "   font-size: 13px;"
        "   icon-size: 18px;"
        "   background-color: white;"
        "   border: none;"
        "   border-radius: 4px;"
        "}"
        "QCalendarWidget QToolButton:hover {"
        "   background-color: #e3f2fd;"
        "}"
        "QCalendarWidget QToolButton:pressed {"
        "   background-color: #bbdefb;"
        "}"
        "QCalendarWidget QMenu {"
        "   background-color: white;"
        "   border: 1px solid #ddd;"
        "}"
        "QCalendarWidget QSpinBox {"
        "   background-color: white;"
        "   selection-background-color: #2196F3;"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "   padding: 4px;"
        "   font-size: 13px;"
        "}"
        "QCalendarWidget QTableView {"
        "   background-color: white;"
        "   selection-background-color: #2196F3;"
        "   selection-color: white;"
        "   border: none;"
        "   outline: none;"
        "   gridline-color: transparent;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QCalendarWidget QAbstractItemView:enabled {"
        "   color: #333;"
        "   background-color: white;"
        "   selection-background-color: #2196F3;"
        "   selection-color: white;"
        "   font-size: 13px;"
        "   font-weight: bold;"
        "}"
        "QCalendarWidget QAbstractItemView:disabled {"
        "   color: #bbb;"
        "}"
        "QCalendarWidget QWidget#qt_calendar_navigationbar {"
        "   background-color: white;"
        "}"
        );

    setFixedSize(560, 360);

    connect(this, &QCalendarWidget::currentPageChanged,
            this, &CustomCalendar::onPageChanged);
}

void CustomCalendar::setScheduleDates(const QMap<QDate, int> &scheduleCounts)
{
    m_scheduleCounts = scheduleCounts;
    updateCells();
}

void CustomCalendar::addScheduleDate(const QDate &date, int count)
{
    m_scheduleCounts[date] = count;
    updateCells();
}

void CustomCalendar::removeScheduleDate(const QDate &date)
{
    m_scheduleCounts.remove(date);
    updateCells();
}

void CustomCalendar::clearScheduleDates()
{
    m_scheduleCounts.clear();
    updateCells();
}

void CustomCalendar::onPageChanged(int year, int month)
{
    Q_UNUSED(year)
    Q_UNUSED(month)
    updateVisibleRows();
}

void CustomCalendar::paintCell(QPainter *painter, const QRect &rect, QDate date) const
{
    bool isCurrentMonth = (date.month() == monthShown() && date.year() == yearShown());

    if (!isCurrentMonth) {
        painter->fillRect(rect, QColor("#ffffff"));
        return;
    }

    painter->fillRect(rect, QColor("#ffffff"));

    bool isSelected = (date == selectedDate());
    if (isSelected) {
        painter->fillRect(rect, QColor("#2196F3"));
    }

    if (date == QDate::currentDate() && !isSelected) {
        painter->save();
        painter->setPen(QPen(QColor("#4CAF50"), 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(rect.adjusted(4, 4, -4, -4), 4, 4);
        painter->restore();
    }

    QColor textColor;
    if (isSelected) {
        textColor = QColor("#FFFFFF");
    } else if (date.dayOfWeek() == Qt::Saturday || date.dayOfWeek() == Qt::Sunday) {
        textColor = QColor("#f44336");
    } else {
        textColor = QColor("#333333");
    }

    painter->setPen(textColor);
    QFont font = painter->font();
    font.setPointSize(13);
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));

    if (m_scheduleCounts.contains(date)) {
        painter->save();

        int scheduleCount = m_scheduleCounts[date];
        int dotSize = 5;
        int spacing = 2;
        int dotsToShow = qMin(scheduleCount, 3);
        int totalWidth = dotsToShow * dotSize + (dotsToShow - 1) * spacing;
        int startX = rect.center().x() - totalWidth / 2;
        int dotY = rect.bottom() - 10;

        painter->setRenderHint(QPainter::Antialiasing);

        QColor dotColor;

        if (isSelected) {
            dotColor = QColor("#FFFFFF");
        } else if (date == QDate::currentDate()) {
            dotColor = QColor("#4CAF50");
        } else {
            dotColor = QColor("#2196F3");
        }

        painter->setBrush(dotColor);
        painter->setPen(Qt::NoPen);

        for (int i = 0; i < dotsToShow; ++i) {
            int x = startX + i * (dotSize + spacing);
            painter->drawEllipse(QPoint(x + dotSize/2, dotY), dotSize/2, dotSize/2);
        }

        painter->restore();
    }
}

void CustomCalendar::showEvent(QShowEvent *event)
{
    QCalendarWidget::showEvent(event);
    updateVisibleRows();
}

void CustomCalendar::wheelEvent(QWheelEvent *event)
{
    event->ignore();
}

void CustomCalendar::updateVisibleRows()
{
    QTableView *tableView = findChild<QTableView*>("qt_calendar_calendarview");
    if (!tableView) {
        return;
    }

    tableView->verticalHeader()->setDefaultSectionSize(52);
    tableView->horizontalHeader()->setDefaultSectionSize(76);

    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    for (int row = 0; row < 6; ++row) {
        tableView->setRowHidden(row, false);
    }
}

// ============================================================================
// SchedulePage 구현 (DataManager 연동)
// ============================================================================

SchedulePage::SchedulePage(int userId, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SchedulePage)
    , m_userId(userId)
    , m_selectedDate(QDate::currentDate())
{
    ui->setupUi(this);

    // CustomCalendar 생성 및 ScrollArea에 추가
    QWidget *calendarContainer = new QWidget();
    QHBoxLayout *centerLayout = new QHBoxLayout(calendarContainer);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->addStretch();

    m_calendar = new CustomCalendar(calendarContainer);
    connect(m_calendar, &QCalendarWidget::selectionChanged,
            this, [this]() { onDateSelected(m_calendar->selectedDate()); });
    connect(m_calendar, &QCalendarWidget::currentPageChanged,
            this, &SchedulePage::onMonthChanged);

    centerLayout->addWidget(m_calendar);
    centerLayout->addStretch();

    ui->scrollArea->setWidget(calendarContainer);

    // 버튼 시그널 연결
    connect(ui->addButton, &QPushButton::clicked, this, &SchedulePage::onAddSchedule);
    connect(ui->editButton, &QPushButton::clicked, this, &SchedulePage::onEditSchedule);
    connect(ui->deleteButton, &QPushButton::clicked, this, &SchedulePage::onDeleteSchedule);

    // 일정 선택 시 버튼 활성화
    connect(ui->scheduleList, &QListWidget::itemSelectionChanged, this, [this]() {
        bool hasSelection = ui->scheduleList->currentItem() != nullptr &&
                            ui->scheduleList->currentItem()->data(Qt::UserRole).toInt() > 0;
        ui->editButton->setEnabled(hasSelection);
        ui->deleteButton->setEnabled(hasSelection);
    });

    // 초기 데이터 로드
    updateCalendarSchedules();
    loadSchedulesForDate(m_selectedDate);

    // 날짜 라벨 초기화
    onDateSelected(m_selectedDate);
}

SchedulePage::~SchedulePage()
{
    delete ui;
}

void SchedulePage::onDateSelected(const QDate &date)
{
    if (date.month() != m_calendar->monthShown() || date.year() != m_calendar->yearShown()) {
        m_calendar->setCurrentPage(date.year(), date.month());
    }

    m_selectedDate = date;

    QString dateText = "📆 " + date.toString("yyyy년 M월 d일 (ddd)");
    if (date == QDate::currentDate()) {
        dateText += " [오늘]";
    }
    ui->dateLabel->setText(dateText);

    loadSchedulesForDate(date);
}

void SchedulePage::onMonthChanged(int year, int month)
{
    Q_UNUSED(month)
    Q_UNUSED(year)
    updateCalendarSchedules();
}

void SchedulePage::loadSchedulesForDate(const QDate &date)
{
    ui->scheduleList->clear();

    // DataManager에서 해당 날짜의 일정 가져오기
    QList<QVariantMap> schedules = DataManager::instance().getSchedulesByDate(
        m_userId,
        date.toString("yyyy-MM-dd")
        );

    if (schedules.isEmpty()) {
        QListWidgetItem *emptyItem = new QListWidgetItem("일정이 없습니다.");
        emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsSelectable);
        emptyItem->setForeground(QColor("#999"));
        emptyItem->setData(Qt::UserRole, -1);  // 빈 항목 표시
        ui->scheduleList->addItem(emptyItem);
    } else {
        for (const QVariantMap &schedule : schedules) {
            QString startTime = schedule["start_time"].toString();
            QString endTime = schedule["end_time"].toString();
            QString title = schedule["title"].toString();
            int scheduleId = schedule["id"].toInt();

            QString displayText = QString("%1 - %2 %3")
                                      .arg(startTime)
                                      .arg(endTime)
                                      .arg(title);

            QListWidgetItem *item = new QListWidgetItem(displayText);
            item->setData(Qt::UserRole, scheduleId);  // ID 저장
            ui->scheduleList->addItem(item);
        }
    }

    // 버튼 상태 초기화
    ui->editButton->setEnabled(false);
    ui->deleteButton->setEnabled(false);
}

void SchedulePage::updateCalendarSchedules()
{
    QMap<QDate, int> scheduleCounts;

    // 현재 표시 중인 월의 일정 가져오기
    int year = m_calendar->yearShown();
    int month = m_calendar->monthShown();

    QList<QVariantMap> schedules = DataManager::instance().getSchedulesByMonth(
        m_userId, year, month
        );

    // 날짜별 일정 개수 계산
    for (const QVariantMap &schedule : schedules) {
        QDate date = QDate::fromString(schedule["date"].toString(), "yyyy-MM-dd");
        scheduleCounts[date]++;
    }

    m_calendar->setScheduleDates(scheduleCounts);
}

void SchedulePage::onAddSchedule()
{
    ScheduleDialog dialog(this, m_selectedDate);

    if (dialog.exec() == QDialog::Accepted) {
        QVariantMap data = dialog.getScheduleData();

        bool success = DataManager::instance().addSchedule(
            m_userId,
            data["title"].toString(),
            data["date"].toString(),
            data["start_time"].toString(),
            data["end_time"].toString(),
            data["location"].toString(),
            data["memo"].toString(),
            data["category"].toString()
            );

        if (success) {
            // AI 요약 무효화
            DataManager::instance().invalidateSummary(m_userId, data["date"].toString());

            // UI 갱신
            updateCalendarSchedules();
            loadSchedulesForDate(m_selectedDate);

            QMessageBox::information(this, "일정 추가", "일정이 성공적으로 추가되었습니다.");
        } else {
            QMessageBox::warning(this, "오류", "일정 추가에 실패했습니다.");
        }
    }
}

void SchedulePage::onEditSchedule()
{
    QListWidgetItem *item = ui->scheduleList->currentItem();
    if (!item) {
        return;
    }

    int scheduleId = item->data(Qt::UserRole).toInt();
    if (scheduleId <= 0) {
        return;
    }

    // 기존 일정 데이터 가져오기
    QVariantMap scheduleData = DataManager::instance().getScheduleById(scheduleId);

    if (scheduleData.isEmpty()) {
        QMessageBox::warning(this, "오류", "일정 정보를 불러올 수 없습니다.");
        return;
    }

    // 수정 Dialog 열기
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
            // AI 요약 무효화 (원래 날짜와 새 날짜 모두)
            DataManager::instance().invalidateSummary(m_userId, scheduleData["date"].toString());
            DataManager::instance().invalidateSummary(m_userId, data["date"].toString());

            // UI 갱신
            updateCalendarSchedules();
            loadSchedulesForDate(m_selectedDate);

            QMessageBox::information(this, "일정 수정", "일정이 성공적으로 수정되었습니다.");
        } else {
            QMessageBox::warning(this, "오류", "일정 수정에 실패했습니다.");
        }
    }
}

void SchedulePage::onDeleteSchedule()
{
    QListWidgetItem *item = ui->scheduleList->currentItem();
    if (!item) {
        return;
    }

    int scheduleId = item->data(Qt::UserRole).toInt();
    if (scheduleId <= 0) {
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this,
        "일정 삭제",
        "선택한 일정을 삭제하시겠습니까?",
        QMessageBox::Yes | QMessageBox::No
        );

    if (reply == QMessageBox::Yes) {
        // 삭제 전 날짜 정보 가져오기 (AI 요약 무효화용)
        QVariantMap scheduleData = DataManager::instance().getScheduleById(scheduleId);
        QString dateStr = scheduleData["date"].toString();

        bool success = DataManager::instance().deleteSchedule(scheduleId);

        if (success) {
            // AI 요약 무효화
            DataManager::instance().invalidateSummary(m_userId, dateStr);

            // UI 갱신
            updateCalendarSchedules();
            loadSchedulesForDate(m_selectedDate);

            QMessageBox::information(this, "삭제 완료", "일정이 삭제되었습니다.");
        } else {
            QMessageBox::warning(this, "오류", "일정 삭제에 실패했습니다.");
        }
    }
}
