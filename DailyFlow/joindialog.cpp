#include "joindialog.h"
#include "ui_joindialog.h"
#include "datamanager.h"
#include <QMessageBox>

JoinDialog::JoinDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::JoinDialog)
{
    ui->setupUi(this);
}

JoinDialog::~JoinDialog()
{
    delete ui;
}

void JoinDialog::on_submitButton_clicked()
{
    // 1. UI에서 입력값 가져오기
    QString username = ui->usernameLineEdit->text().trimmed();
    QString password = ui->passwordLineEdit->text();
    QString confirmPassword = ui->confirmPasswordLineEdit->text();
    QString name = ui->nameLineEdit->text().trimmed();

    // 이메일 = emailLineEdit + @ + emailDomainComboBox
    QString email = ui->emailLineEdit->text().trimmed() + "@" +
                    ui->emailDomainComboBox->currentText();

    // 생년월일
    QString dateOfBirth = ui->dateOfBirthEdit->date().toString("yyyy-MM-dd");

    // 주소 = addressLineEdit + addressDetailLineEdit
    QString address = ui->addressLineEdit->text().trimmed() + " " +
                      ui->addressDetailLineEdit->text().trimmed();
    address = address.trimmed();

    // 2. 입력값 유효성 검사
    if (username.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "Username을 입력해주세요.");
        ui->usernameLineEdit->setFocus();
        return;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "Password를 입력해주세요.");
        ui->passwordLineEdit->setFocus();
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(this, "입력 오류", "비밀번호가 일치하지 않습니다.");
        ui->confirmPasswordLineEdit->clear();
        ui->confirmPasswordLineEdit->setFocus();
        return;
    }

    if (name.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "Name을 입력해주세요.");
        ui->nameLineEdit->setFocus();
        return;
    }

    if (ui->emailLineEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "Email을 입력해주세요.");
        ui->emailLineEdit->setFocus();
        return;
    }

    // 3. DataManager를 통해 회원가입 처리
    bool success = DataManager::instance().addUser(username, password, name,
                                                   email, dateOfBirth, address);

    // 4. 결과 처리
    if (success) {
        QMessageBox::information(this, "회원가입 성공", "회원가입이 완료되었습니다!");
        close();  // 회원가입 창 닫기
    } else {
        QMessageBox::warning(this, "회원가입 실패",
                             "회원가입에 실패했습니다.\n(중복된 Username일 수 있습니다)");
    }
}

void JoinDialog::on_cancelButton_clicked()
{
    // 취소 버튼: 확인 후 창 닫기
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "취소 확인",
                                  "회원가입을 취소하시겠습니까?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        close();
    }
}
