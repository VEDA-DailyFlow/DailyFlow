#include "logindialog.h"
#include "ui_logindialog.h"
#include "joindialog.h"

LoginDialog::LoginDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::setDataManager(DataManager *dataManager)
{
    m_dataManager = dataManager;
}

void LoginDialog::on_loginButton_clicked()
{

}


void LoginDialog::on_joinButton_clicked()  // Join 버튼을 클릭하면 회원가입 창이 열림
{
    // JoinDialog 객체 생성
    JoinDialog *joinDialog = new JoinDialog(this);  // this를 인자로 넘김(LoginDialog가 닫힐 때 JoinDialog도 같이 닫함)

    // 3. (핵심) LoginDialog가 갖고 있던 DataManager를 JoinDialog에게 넘겨줌
    // (이래야 JoinDialog가 DB에 접근 가능)
    joinDialog->setDataManager(m_dataManager);

    // 4. JoinDialog 창을 띄움
    // .show()는 비모달(Non-Modal): Join 창과 Login 창을 동시에 조작 가능
    // .exec()는 모달(Modal): Join 창을 닫기 전까지 Login 창 조작 불가능
    joinDialog->show();
}

