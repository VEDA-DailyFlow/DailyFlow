#include "joindialog.h"
#include "ui_joindialog.h"
#include "datamanager.h"
#include <QMessageBox>
#include <QSettings>

JoinDialog::JoinDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::JoinDialog)
{
    ui->setupUi(this);

    // 모든 QLineEdit에 스타일 적용
    QString lineEditStyle =
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
        "}";

    ui->usernameLineEdit->setStyleSheet(lineEditStyle);
    ui->passwordLineEdit->setStyleSheet(lineEditStyle);
    ui->confirmPasswordLineEdit->setStyleSheet(lineEditStyle);
    ui->nameLineEdit->setStyleSheet(lineEditStyle);
    ui->emailLineEdit->setStyleSheet(lineEditStyle);
    ui->addressLineEdit->setStyleSheet(lineEditStyle);
    ui->addressDetailLineEdit->setStyleSheet(lineEditStyle);
    ui->emailDomainComboBox->setStyleSheet(lineEditStyle);
    ui->dateOfBirthEdit->setStyleSheet(lineEditStyle);

    // QPalette 설정
    QPalette palette;
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);

    ui->usernameLineEdit->setPalette(palette);
    ui->passwordLineEdit->setPalette(palette);
    ui->confirmPasswordLineEdit->setPalette(palette);
    ui->nameLineEdit->setPalette(palette);
    ui->emailLineEdit->setPalette(palette);
    ui->addressLineEdit->setPalette(palette);
    ui->addressDetailLineEdit->setPalette(palette);
    ui->emailDomainComboBox->setPalette(palette);
    ui->dateOfBirthEdit->setPalette(palette);

    // 포커스 설정
    ui->usernameLineEdit->setFocus();
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
        QMessageBox::warning(nullptr, "입력 오류", "Username을 입력해주세요.");
        ui->usernameLineEdit->setFocus();
        return;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(nullptr, "입력 오류", "Password를 입력해주세요.");
        ui->passwordLineEdit->setFocus();
        return;
    }

    if (password != confirmPassword) {
        QMessageBox::warning(nullptr, "입력 오류", "비밀번호가 일치하지 않습니다.");
        ui->confirmPasswordLineEdit->clear();
        ui->confirmPasswordLineEdit->setFocus();
        return;
    }

    if (name.isEmpty()) {
        QMessageBox::warning(nullptr, "입력 오류", "Name을 입력해주세요.");
        ui->nameLineEdit->setFocus();
        return;
    }

    if (ui->emailLineEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(nullptr, "입력 오류", "Email을 입력해주세요.");
        ui->emailLineEdit->setFocus();
        return;
    }

    // 3. DataManager를 통해 회원가입 처리
    bool success = DataManager::instance().addUser(username, password, name,
                                                   email, dateOfBirth, address);

    // 4. 결과 처리
    if (success) {
        QMessageBox::information(nullptr, "회원가입 성공", "회원가입이 완료되었습니다!");
        accept();  // close() 대신 accept() 사용
    } else {
        QMessageBox::warning(nullptr, "회원가입 실패",
                             "회원가입에 실패했습니다.\n(중복된 Username일 수 있습니다)");
    }
}

void JoinDialog::on_cancelButton_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(nullptr, "취소 확인",
                                  "회원가입을 취소하시겠습니까?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        reject();  // close() 대신 reject() 사용
    }
}
