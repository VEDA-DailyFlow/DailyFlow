#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QListWidgetItem>
#include <QMap>

namespace Ui {
class HomePage;
}

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(int userId, QWidget *parent = nullptr);
    ~HomePage();

public slots:
    void refreshSchedules();

private slots:
    void onScheduleItemClicked(QListWidgetItem *item);
    void onEditButtonClicked();
    void onDeleteButtonClicked();
    void onRefreshButtonClicked();    // 통합 새로고침 버튼

private:
    Ui::HomePage *ui;
    int m_userId;
    QMap<QListWidgetItem*, int> m_itemToScheduleId;

    void loadAISummary();
    void loadFortune();
    void loadUpcomingSchedules();
    void displayScheduleDetail(int scheduleId);
};

#endif // HOMEPAGE_H
