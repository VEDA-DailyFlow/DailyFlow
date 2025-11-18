#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QListWidgetItem>

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

private:
    void loadAISummary();
    void loadUpcomingSchedules();
    void displayScheduleDetail(int scheduleId);

    Ui::HomePage *ui;
    int m_userId;
    QMap<QListWidgetItem*, int> m_itemToScheduleId;  // 리스트 아이템 -> 스케줄 ID 매핑
};

#endif // HOMEPAGE_H
