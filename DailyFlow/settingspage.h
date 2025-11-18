#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>

namespace Ui {
class SettingsPage;
}

class SettingsPage : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsPage(int userId, QWidget *parent = nullptr);
    ~SettingsPage();

signals:
    void logoutRequested();
    void themeChanged(bool isDarkMode);

private slots:
    void onEditInfoClicked();
    void onSaveInfoClicked();
    void onCancelEditClicked();
    void onChangePasswordClicked();
    void onDeleteAccountClicked();
    void onThemeToggled(bool checked);

private:
    Ui::SettingsPage *ui;
    int m_userId;
    bool m_isEditMode;

    void loadUserInfo();
    void setEditMode(bool enabled);
    void applyDarkMode(bool isDark);
};

#endif // SETTINGSPAGE_H
