#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(const QString &userId, QWidget *parent = nullptr);

private:
    void setupUI();

    QString m_userId;
};

#endif // SETTINGSPAGE_H
