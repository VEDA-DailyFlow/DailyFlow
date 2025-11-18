#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    int getLoggedInUserId() const { return m_userId; }

private slots:
    void on_loginButton_clicked();
    void on_joinButton_clicked();

private:
    Ui::LoginDialog *ui;
    int m_userId = -1;  // 로그인된 사용자 ID 저장
};

#endif // LOGINDIALOG_H
