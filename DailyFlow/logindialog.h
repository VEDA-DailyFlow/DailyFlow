#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include "datamanager.h"
#include <QWidget>

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QWidget
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();

    void setDataManager(DataManager *dataManager);

private slots:
    void on_loginButton_clicked();
    void on_joinButton_clicked();

private:
    Ui::LoginDialog *ui;

    DataManager* m_dataManager;  // LoginDialog에서 DB에 접근하기 위함 (유저네임, 패스워드 확인 용도)
};

#endif // LOGINDIALOG_H
