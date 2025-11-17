#ifndef SCHEDULEPAGE_H
#define SCHEDULEPAGE_H

#include <QWidget>
#include <QCalendarWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QTextCharFormat>
#include <QTableView>
#include <QHeaderView>
#include <QWheelEvent>

// 커스텀 캘린더 위젯
class CustomCalendar : public QCalendarWidget
{
    Q_OBJECT

public:
    explicit CustomCalendar(QWidget *parent = nullptr);

    // 특정 날짜에 일정이 있는지 설정
    void setScheduleDates(const QMap<QDate, int> &scheduleCounts);
    void addScheduleDate(const QDate &date, int count = 1);
    void removeScheduleDate(const QDate &date);
    void clearScheduleDates();

public slots:
    void onPageChanged(int year, int month);

protected:
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;
    void showEvent(QShowEvent *event) override;
    void wheelEvent(QWheelEvent *event) override; // 추가

private:
    void updateVisibleRows();
    QMap<QDate, int> m_scheduleCounts; // 날짜별 일정 개수
};

class SchedulePage : public QWidget
{
    Q_OBJECT

public:
    explicit SchedulePage(const QString &userId, QWidget *parent = nullptr);

private slots:
    void onDateSelected(const QDate &date);
    void onAddSchedule();
    void onEditSchedule();
    void onDeleteSchedule();

private:
    void setupUI();
    void loadSchedulesForDate(const QDate &date);
    void updateCalendarSchedules();

    QString m_userId;
    QDate m_selectedDate;

    // UI 컴포넌트
    CustomCalendar *m_calendar;
    QLabel *m_dateLabel;
    QListWidget *m_scheduleList;
    QPushButton *m_addButton;
    QPushButton *m_editButton;
    QPushButton *m_deleteButton;

    // 임시 데이터 저장 (실제로는 DataManager 사용)
    QMap<QDate, QStringList> m_scheduleData;
};

#endif // SCHEDULEPAGE_H
