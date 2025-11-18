#include "logindialog.h"
#include "ui_logindialog.h"
#include "joindialog.h"
#include "datamanager.h"
#include <QMessageBox>
#include <QSettings>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    // QLineEdit 스타일 명시적으로 설정
    ui->usernameLineEdit->setStyleSheet(
        "QLineEdit {"
        "   padding: 8px;"
        "   border: 2px solid #ddd;"
        "   border-radius: 4px;"
        "   font-size: 14px;"
        "   color: #000000;"
        "   background-color: #ffffff;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #2196F3;"
        "}"
        );

    ui->passwordLineEdit->setStyleSheet(
        "QLineEdit {"
        "   padding: 8px;"
        "   border: 2px solid #ddd;"
        "   border-radius: 4px;"
        "   font-size: 14px;"
        "   color: #000000;"
        "   background-color: #ffffff;"
        "}"
        "QLineEdit:focus {"
        "   border-color: #2196F3;"
        "}"
        );

    // QPalette로도 설정 (이중 보험)
    QPalette palette = ui->usernameLineEdit->palette();
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    ui->usernameLineEdit->setPalette(palette);
    ui->passwordLineEdit->setPalette(palette);

    // 포커스 설정
    ui->usernameLineEdit->setFocus();
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
        // 재시도를 위해 입력 필드 초기화
        ui->passwordLineEdit->clear();
        ui->usernameLineEdit->setFocus();
    }
}

void LoginDialog::on_joinButton_clicked()
{
    JoinDialog joinDialog(this);
    joinDialog.exec();
}
