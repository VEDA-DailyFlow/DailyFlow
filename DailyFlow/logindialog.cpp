#include "logindialog.h"
#include "ui_logindialog.h"
#include "joindialog.h"
#include "datamanager.h"
#include <QMessageBox>   // 사용자에게 로그인 성공 여부를 보여주기 위함

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    int id = DataManager::instance().loginUser(username, password);

    if (id != -1) {
        m_userId = id;  // ID 저장
        accept();
    } else {
        QMessageBox::warning(this, "Login Failed", "아이디나 비밀번호가 틀립니다.");
    }
}

void LoginDialog::on_joinButton_clicked()
{
    JoinDialog joinDialog(this);
    joinDialog.exec();  // 모달로 실행 (회원가입 창이 닫힐 때까지 대기)
}
