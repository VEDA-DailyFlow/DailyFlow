#ifndef SCHEDULEPAGE_H
#define SCHEDULEPAGE_H

#include <QWidget>
#include <QCalendarWidget>
#include <QPainter>
#include <QTextCharFormat>
#include <QTableView>
#include <QHeaderView>
#include <QWheelEvent>

namespace Ui {
class SchedulePage;
}

// 커스텀 캘린더 위젯
class CustomCalendar : public QCalendarWidget
{
    Q_OBJECT

public:
    explicit CustomCalendar(QWidget *parent = nullptr);

    void setScheduleDates(const QMap<QDate, int> &scheduleCounts);
    void addScheduleDate(const QDate &date, int count = 1);
    void removeScheduleDate(const QDate &date);
    void clearScheduleDates();

public slots:
    void onPageChanged(int year, int month);

protected:
    void paintCell(QPainter *painter, const QRect &rect, QDate date) const override;
    void showEvent(QShowEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void updateVisibleRows();
    QMap<QDate, int> m_scheduleCounts;
};

class SchedulePage : public QWidget
{
    Q_OBJECT

public:
    explicit SchedulePage(int userId, QWidget *parent = nullptr);
    ~SchedulePage();

private slots:
    void onDateSelected(const QDate &date);
    void onMonthChanged(int year, int month);
    void onAddSchedule();
    void onEditSchedule();
    void onDeleteSchedule();

private:
    void loadSchedulesForDate(const QDate &date);
    void updateCalendarSchedules();

    Ui::SchedulePage *ui;
    int m_userId;  // QString -> int 변경
    QDate m_selectedDate;
    CustomCalendar *m_calendar;
};

#endif // SCHEDULEPAGE_H
