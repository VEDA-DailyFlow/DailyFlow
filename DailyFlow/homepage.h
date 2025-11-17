#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>

namespace Ui {
class HomePage;
}

class HomePage : public QWidget
{
    Q_OBJECT

public:
    explicit HomePage(const QString &userId, QWidget *parent = nullptr);
    ~HomePage();

private:
    void loadAISummary();
    void loadUpcomingSchedules();

    Ui::HomePage *ui;
    QString m_userId;
};

#endif // HOMEPAGE_H
