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
    // UI에서 텍스트 가져오기
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    // DataManager에 유저네임, 패스워드 전달
    bool success = DataManager::instance()->loginUser(username, password);

    // 3-3. 결과 처리
    if (success) {
        accept();  // accept가 호출되면 login 창이
                   // main의 exec() 함수가 'QDialog::Accepted'라는 정수 값 반환

    } else {
        QMessageBox::warning(this, "Login Failed", "아이디나 비밀번호가 틀립니다.");
    }
}

void LoginDialog::on_joinButton_clicked()  // Join 버튼을 클릭하면 회원가입 창이 열림
{
    JoinDialog *joinDialog = new JoinDialog(this);  // this를 인자로 넘김(LoginDialog가 닫힐 때 JoinDialog도 같이 닫함)

    joinDialog->setAttribute(Qt::WA_DeleteOnClose);  // 창이 닫히면 자동으로 메모리에서 삭제
    joinDialog->show();  // 회원가입 창 띄움
}

