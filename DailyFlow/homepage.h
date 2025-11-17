#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(const QString &userId, QWidget *parent = nullptr);

private:
    void setupUI();
    void loadAISummary();
    void loadUpcomingSchedules();

    QString m_userId;

    // UI 컴포넌트
    QLabel *m_aiSummaryTitle;
    QTextEdit *m_aiSummaryText;

    QLabel *m_upcomingTitle;
    QListWidget *m_scheduleList;

    QLabel *m_detailTitle;
    QTextEdit *m_scheduleDetail;

    QPushButton *m_editButton;
    QPushButton *m_deleteButton;
};

#endif // HOMEPAGE_H
