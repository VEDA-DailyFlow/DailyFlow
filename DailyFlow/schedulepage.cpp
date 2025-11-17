#include "schedulepage.h"
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>

// ============================================================================
// CustomCalendar 구현
// ============================================================================

CustomCalendar::CustomCalendar(QWidget *parent)
    : QCalendarWidget(parent)
{
    setGridVisible(false); // 그리드 숨김
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    // 헤더 포맷 설정 (요일)
    QTextCharFormat headerFormat;
    headerFormat.setForeground(QBrush(QColor("#333333")));
    headerFormat.setFontWeight(QFont::Bold);
    headerFormat.setFontPointSize(12); // 14 -> 12
    setHeaderTextFormat(headerFormat);

    // 주말 포맷 설정
    QTextCharFormat weekendFormat;
    weekendFormat.setForeground(QBrush(QColor("#f44336")));
    weekendFormat.setFontWeight(QFont::Bold);
    weekendFormat.setFontPointSize(12); // 14 -> 12
    setWeekdayTextFormat(Qt::Saturday, weekendFormat);
    setWeekdayTextFormat(Qt::Sunday, weekendFormat);

    // 평일 포맷 설정
    QTextCharFormat weekdayFormat;
    weekdayFormat.setForeground(QBrush(QColor("#333333")));
    weekdayFormat.setFontWeight(QFont::Bold);
    weekdayFormat.setFontPointSize(12); // 14 -> 12
    setWeekdayTextFormat(Qt::Monday, weekdayFormat);
    setWeekdayTextFormat(Qt::Tuesday, weekdayFormat);
    setWeekdayTextFormat(Qt::Wednesday, weekdayFormat);
    setWeekdayTextFormat(Qt::Thursday, weekdayFormat);
    setWeekdayTextFormat(Qt::Friday, weekdayFormat);

    // 스타일시트
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

    // 고정 크기 설정 (700x450 -> 560x360)
    setFixedSize(560, 360);

    // 페이지 변경 시 행 업데이트
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
        // 다른 달의 날짜는 아예 표시하지 않음
        painter->fillRect(rect, QColor("#ffffff"));
        return;
    }

    // 현재 달의 날짜만 표시
    painter->fillRect(rect, QColor("#ffffff"));

    // 선택된 날짜 배경
    bool isSelected = (date == selectedDate());
    if (isSelected) {
        painter->fillRect(rect, QColor("#2196F3"));
    }

    // 오늘 날짜 배경 (선택되지 않은 경우)
    if (date == QDate::currentDate() && !isSelected) {
        painter->save();
        painter->setPen(QPen(QColor("#4CAF50"), 2));
        painter->setBrush(Qt::NoBrush);
        painter->drawRoundedRect(rect.adjusted(4, 4, -4, -4), 4, 4);
        painter->restore();
    }

    // 날짜 텍스트
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
    font.setPointSize(13); // 15 -> 13
    font.setBold(true);
    painter->setFont(font);
    painter->drawText(rect, Qt::AlignCenter, QString::number(date.day()));

    // 일정 표시 점
    if (m_scheduleCounts.contains(date)) {
        painter->save();

        int scheduleCount = m_scheduleCounts[date];
        int dotSize = 5; // 6 -> 5
        int spacing = 2; // 3 -> 2
        int dotsToShow = qMin(scheduleCount, 3);
        int totalWidth = dotsToShow * dotSize + (dotsToShow - 1) * spacing;
        int startX = rect.center().x() - totalWidth / 2;
        int dotY = rect.bottom() - 10; // 12 -> 10

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
    // 스크롤로 월 넘기기 비활성화
    event->ignore();
}

void CustomCalendar::updateVisibleRows()
{
    QTableView *tableView = findChild<QTableView*>("qt_calendar_calendarview");
    if (!tableView) {
        return;
    }

    // 셀 크기만 고정
    tableView->verticalHeader()->setDefaultSectionSize(52);
    tableView->horizontalHeader()->setDefaultSectionSize(76);

    tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    // 행 숨김 로직 완전 제거 - 모든 행 표시
    for (int row = 0; row < 6; ++row) {
        tableView->setRowHidden(row, false);
    }
}

// ============================================================================
// SchedulePage 구현
// ============================================================================

SchedulePage::SchedulePage(const QString &userId, QWidget *parent)
    : QWidget(parent)
    , m_userId(userId)
    , m_selectedDate(QDate::currentDate())
{
    // 테스트 데이터
    m_scheduleData[QDate::currentDate().addDays(3)] =
        QStringList() << "14:00 - 16:00 한화비전 미팅";
    m_scheduleData[QDate::currentDate().addDays(4)] =
        QStringList() << "10:00 - 12:00 VEDA 프로젝트 발표"
                      << "15:00 - 17:00 팀 회의";
    m_scheduleData[QDate::currentDate()] =
        QStringList() << "09:00 - 10:00 데일리 스탠드업";

    setupUI();
    updateCalendarSchedules();
    loadSchedulesForDate(m_selectedDate);
}

void SchedulePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(20);

    // 캘린더 섹션
    QGroupBox *calendarBox = new QGroupBox("📅 캘린더", this);
    calendarBox->setStyleSheet(
        "QGroupBox {"
        "   font-size: 16px;"
        "   font-weight: bold;"
        "   color: #333;"
        "   border: 2px solid #2196F3;"
        "   border-radius: 8px;"
        "   margin-top: 10px;"
        "   padding-top: 15px;"
        "   background-color: white;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   left: 15px;"
        "   padding: 0 5px;"
        "}"
        );

    QVBoxLayout *calendarBoxLayout = new QVBoxLayout(calendarBox);
    calendarBoxLayout->setContentsMargins(20, 25, 20, 20);
    calendarBoxLayout->setSpacing(15);

    // 범례
    QHBoxLayout *legendLayout = new QHBoxLayout();
    legendLayout->setSpacing(30);
    legendLayout->setContentsMargins(0, 0, 0, 15);

    QLabel *legendLabel = new QLabel("● 일정 있음", this);
    legendLabel->setStyleSheet(
        "color: #2196F3; "
        "font-size: 13px; "
        "font-weight: normal; "
        "padding: 5px 10px;"
        );

    QLabel *todayLabel = new QLabel("● 오늘", this);
    todayLabel->setStyleSheet(
        "color: #4CAF50; "
        "font-size: 13px; "
        "font-weight: normal; "
        "padding: 5px 10px;"
        );

    legendLayout->addStretch();
    legendLayout->addWidget(legendLabel);
    legendLayout->addWidget(todayLabel);
    legendLayout->addStretch();

    calendarBoxLayout->addLayout(legendLayout);

    // 스크롤 영역 생성
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(false);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrollArea->setAlignment(Qt::AlignCenter);
    scrollArea->setStyleSheet(
        "QScrollArea {"
        "   border: none;"
        "   background-color: white;"
        "}"
        "QScrollBar:vertical {"
        "   border: none;"
        "   background: #f0f0f0;"
        "   width: 10px;"
        "   margin: 0;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #2196F3;"
        "   border-radius: 5px;"
        "   min-height: 20px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "   background: #1976D2;"
        "}"
        "QScrollBar:horizontal {"
        "   border: none;"
        "   background: #f0f0f0;"
        "   height: 10px;"
        "   margin: 0;"
        "}"
        "QScrollBar::handle:horizontal {"
        "   background: #2196F3;"
        "   border-radius: 5px;"
        "   min-width: 20px;"
        "}"
        "QScrollBar::handle:horizontal:hover {"
        "   background: #1976D2;"
        "}"
        "QScrollBar::add-line, QScrollBar::sub-line {"
        "   border: none;"
        "   background: none;"
        "}"
        );

    // 캘린더를 중앙에 배치할 컨테이너 위젯
    QWidget *calendarContainer = new QWidget();
    QHBoxLayout *centerLayout = new QHBoxLayout(calendarContainer);
    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerLayout->addStretch();

    m_calendar = new CustomCalendar(calendarContainer);
    connect(m_calendar, &QCalendarWidget::selectionChanged,
            this, [this]() { onDateSelected(m_calendar->selectedDate()); });

    centerLayout->addWidget(m_calendar);
    centerLayout->addStretch();

    scrollArea->setWidget(m_calendar);
    calendarBoxLayout->addWidget(scrollArea);

    mainLayout->addWidget(calendarBox);

    // 일정 목록 섹션
    QGroupBox *scheduleBox = new QGroupBox("", this);
    scheduleBox->setStyleSheet(
        "QGroupBox {"
        "   border: 2px solid #e0e0e0;"
        "   border-radius: 8px;"
        "   margin-top: 5px;"
        "   padding-top: 10px;"
        "   background-color: white;"
        "}"
        );

    QVBoxLayout *scheduleLayout = new QVBoxLayout(scheduleBox);
    scheduleLayout->setContentsMargins(15, 15, 15, 15);

    // 선택된 날짜 표시
    m_dateLabel = new QLabel(this);
    m_dateLabel->setStyleSheet(
        "font-size: 16px; "
        "font-weight: bold; "
        "color: #2196F3; "
        "padding: 8px; "
        "background-color: #e3f2fd; "
        "border-radius: 4px;"
        );
    m_dateLabel->setText("📆 " + m_selectedDate.toString("yyyy년 M월 d일 (ddd)"));
    scheduleLayout->addWidget(m_dateLabel);

    // 일정 리스트
    m_scheduleList = new QListWidget(this);
    m_scheduleList->setStyleSheet(
        "QListWidget {"
        "   border: 1px solid #e0e0e0;"
        "   border-radius: 4px;"
        "   background-color: #fafafa;"
        "   padding: 5px;"
        "}"
        "QListWidget::item {"
        "   padding: 12px;"
        "   margin: 4px;"
        "   border-radius: 4px;"
        "   background-color: white;"
        "   border-left: 4px solid #2196F3;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #e3f2fd;"
        "   color: #1976D2;"
        "   border-left: 4px solid #1976D2;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #f5f5f5;"
        "}"
        );
    scheduleLayout->addWidget(m_scheduleList);

    // 버튼 레이아웃
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    m_addButton = new QPushButton("➕ 일정 추가", this);
    m_addButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #2196F3;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px 24px;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #1976D2;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1565C0;"
        "}"
        );
    connect(m_addButton, &QPushButton::clicked, this, &SchedulePage::onAddSchedule);

    m_editButton = new QPushButton("✏️ 수정", this);
    m_editButton->setEnabled(false);
    m_editButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px 24px;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover:enabled {"
        "   background-color: #45a049;"
        "}"
        "QPushButton:pressed:enabled {"
        "   background-color: #3d8b40;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #cccccc;"
        "   color: #888;"
        "}"
        );
    connect(m_editButton, &QPushButton::clicked, this, &SchedulePage::onEditSchedule);

    m_deleteButton = new QPushButton("🗑️ 삭제", this);
    m_deleteButton->setEnabled(false);
    m_deleteButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #f44336;"
        "   color: white;"
        "   border: none;"
        "   padding: 12px 24px;"
        "   border-radius: 6px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover:enabled {"
        "   background-color: #da190b;"
        "}"
        "QPushButton:pressed:enabled {"
        "   background-color: #c41c0c;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #cccccc;"
        "   color: #888;"
        "}"
        );
    connect(m_deleteButton, &QPushButton::clicked, this, &SchedulePage::onDeleteSchedule);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_editButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();

    scheduleLayout->addLayout(buttonLayout);
    mainLayout->addWidget(scheduleBox, 1);

    // 일정 선택 시 버튼 활성화
    connect(m_scheduleList, &QListWidget::itemSelectionChanged, this, [this]() {
        bool hasSelection = m_scheduleList->currentItem() != nullptr;
        m_editButton->setEnabled(hasSelection);
        m_deleteButton->setEnabled(hasSelection);
    });
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
    m_dateLabel->setText(dateText);

    loadSchedulesForDate(date);
}

void SchedulePage::loadSchedulesForDate(const QDate &date)
{
    m_scheduleList->clear();

    if (m_scheduleData.contains(date)) {
        const QStringList &schedules = m_scheduleData[date];
        for (const QString &schedule : schedules) {
            m_scheduleList->addItem(schedule);
        }
    }

    if (m_scheduleList->count() == 0) {
        QListWidgetItem *emptyItem = new QListWidgetItem("일정이 없습니다.");
        emptyItem->setFlags(emptyItem->flags() & ~Qt::ItemIsSelectable);
        emptyItem->setForeground(QColor("#999"));
        m_scheduleList->addItem(emptyItem);
    }
}

void SchedulePage::updateCalendarSchedules()
{
    QMap<QDate, int> scheduleCounts;

    for (auto it = m_scheduleData.begin(); it != m_scheduleData.end(); ++it) {
        scheduleCounts[it.key()] = it.value().count();
    }

    m_calendar->setScheduleDates(scheduleCounts);
}

void SchedulePage::onAddSchedule()
{
    QMessageBox::information(this, "일정 추가",
                             "일정 추가 기능은 ScheduleDialog 구현 후 사용 가능합니다.");
}

void SchedulePage::onEditSchedule()
{
    if (!m_scheduleList->currentItem()) {
        return;
    }

    QMessageBox::information(this, "일정 수정",
                             "일정 수정 기능은 ScheduleDialog 구현 후 사용 가능합니다.");
}

void SchedulePage::onDeleteSchedule()
{
    if (!m_scheduleList->currentItem() ||
        m_scheduleList->currentItem()->text() == "일정이 없습니다.") {
        return;
    }

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "일정 삭제",
                                  "선택한 일정을 삭제하시겠습니까?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QString scheduleText = m_scheduleList->currentItem()->text();
        if (m_scheduleData.contains(m_selectedDate)) {
            m_scheduleData[m_selectedDate].removeOne(scheduleText);
            if (m_scheduleData[m_selectedDate].isEmpty()) {
                m_scheduleData.remove(m_selectedDate);
            }
        }

        updateCalendarSchedules();
        loadSchedulesForDate(m_selectedDate);

        QMessageBox::information(this, "삭제 완료", "일정이 삭제되었습니다.");
    }
}
