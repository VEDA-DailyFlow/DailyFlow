#include "settingspage.h"
#include "ui_settingspage.h"
#include "datamanager.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QSettings>
#include <QPalette>
#include <QApplication>

SettingsPage::SettingsPage(int userId, QWidget *parent)  // QString에서 int로 변경
    : QWidget(parent)
    , ui(new Ui::SettingsPage)
    , m_userId(userId)  // 직접 int로 받음
    , m_isEditMode(false)
{
    ui->setupUi(this);

    // 회원 정보 로드
    loadUserInfo();

    // 초기에는 읽기 전용 모드
    setEditMode(false);

    // 시그널-슬롯 연결
    connect(ui->btnEditInfo, &QPushButton::clicked, this, &SettingsPage::onEditInfoClicked);
    connect(ui->btnSaveInfo, &QPushButton::clicked, this, &SettingsPage::onSaveInfoClicked);
    connect(ui->btnCancelEdit, &QPushButton::clicked, this, &SettingsPage::onCancelEditClicked);
    connect(ui->btnChangePassword, &QPushButton::clicked, this, &SettingsPage::onChangePasswordClicked);
    connect(ui->btnDeleteAccount, &QPushButton::clicked, this, &SettingsPage::onDeleteAccountClicked);
    connect(ui->checkDarkMode, &QCheckBox::toggled, this, &SettingsPage::onThemeToggled);

    // 저장된 테마 설정 불러오기
    QSettings settings("DailyFlow", "Settings");
    bool isDarkMode = settings.value("darkMode", false).toBool();
    ui->checkDarkMode->setChecked(isDarkMode);
    applyDarkMode(isDarkMode);
}

SettingsPage::~SettingsPage()
{
    delete ui;
}

void SettingsPage::loadUserInfo()
{
    DataManager& dm = DataManager::instance();
    QVariantMap userInfo = dm.getUserInfo(m_userId);  // 이미 int라서 toInt() 불필요

    if (!userInfo.isEmpty()) {
        ui->lineEditUsername->setText(userInfo["username"].toString());
        ui->lineEditName->setText(userInfo["name"].toString());
        ui->lineEditEmail->setText(userInfo["email"].toString());
        ui->lineEditDateOfBirth->setText(userInfo["dateOfBirth"].toString());
        ui->lineEditAddress->setText(userInfo["address"].toString());
    }
}

void SettingsPage::setEditMode(bool enabled)
{
    m_isEditMode = enabled;

    // 아이디는 항상 읽기 전용
    ui->lineEditUsername->setReadOnly(true);

    // 나머지 필드들은 편집 모드에 따라 변경
    ui->lineEditName->setReadOnly(!enabled);
    ui->lineEditEmail->setReadOnly(!enabled);
    ui->lineEditDateOfBirth->setReadOnly(!enabled);
    ui->lineEditAddress->setReadOnly(!enabled);

    // 현재 다크모드 상태 확인
    QSettings settings("DailyFlow", "Settings");
    bool isDarkMode = settings.value("darkMode", false).toBool();

    // 편집 모드에 따라 배경색 변경
    QString editableStyle = isDarkMode ?
                                "QLineEdit { background-color: #2a2a2a; color: white; }" :
                                "QLineEdit { background-color: white; color: black; }";

    QString readOnlyStyle = isDarkMode ?
                                "QLineEdit { background-color: transparent; border: none; color: white; }" :
                                "QLineEdit { background-color: transparent; border: none; color: black; }";

    // 아이디는 항상 투명
    ui->lineEditUsername->setStyleSheet(readOnlyStyle);

    if (enabled) {
        // 편집 모드: 일반 입력창 스타일
        ui->lineEditName->setStyleSheet(editableStyle);
        ui->lineEditEmail->setStyleSheet(editableStyle);
        ui->lineEditDateOfBirth->setStyleSheet(editableStyle);
        ui->lineEditAddress->setStyleSheet(editableStyle);
    } else {
        // 읽기 모드: 투명 배경
        ui->lineEditName->setStyleSheet(readOnlyStyle);
        ui->lineEditEmail->setStyleSheet(readOnlyStyle);
        ui->lineEditDateOfBirth->setStyleSheet(readOnlyStyle);
        ui->lineEditAddress->setStyleSheet(readOnlyStyle);
    }

    // 버튼 표시 제어
    ui->btnEditInfo->setVisible(!enabled);
    ui->btnSaveInfo->setVisible(enabled);
    ui->btnCancelEdit->setVisible(enabled);
}

void SettingsPage::onEditInfoClicked()
{
    setEditMode(true);
}

void SettingsPage::onSaveInfoClicked()
{
    QString name = ui->lineEditName->text().trimmed();
    QString email = ui->lineEditEmail->text().trimmed();
    QString dateOfBirth = ui->lineEditDateOfBirth->text().trimmed();
    QString address = ui->lineEditAddress->text().trimmed();

    // 유효성 검사
    if (name.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "이름을 입력해주세요.");
        return;
    }

    if (email.isEmpty() || !email.contains("@")) {
        QMessageBox::warning(this, "입력 오류", "올바른 이메일을 입력해주세요.");
        return;
    }

    // 데이터베이스 업데이트
    DataManager& dm = DataManager::instance();
    bool success = dm.updateUser(m_userId, name, email, dateOfBirth, address);

    if (success) {
        QMessageBox::information(this, "성공", "회원 정보가 수정되었습니다.");
        setEditMode(false);
    } else {
        QMessageBox::critical(this, "오류", "회원 정보 수정에 실패했습니다.");
    }
}

void SettingsPage::onCancelEditClicked()
{
    // 원래 정보로 되돌리기
    loadUserInfo();
    setEditMode(false);
}

void SettingsPage::onChangePasswordClicked()
{
    // 현재 비밀번호 확인
    bool ok;
    QString oldPassword = QInputDialog::getText(this, "비밀번호 확인",
                                                "현재 비밀번호를 입력하세요:",
                                                QLineEdit::Password, "", &ok);
    if (!ok || oldPassword.isEmpty()) {
        return;
    }

    // 새 비밀번호 입력
    QString newPassword = QInputDialog::getText(this, "비밀번호 변경",
                                                "새 비밀번호를 입력하세요:",
                                                QLineEdit::Password, "", &ok);
    if (!ok || newPassword.isEmpty()) {
        return;
    }

    // 비밀번호 길이 확인
    if (newPassword.length() < 4) {
        QMessageBox::warning(this, "입력 오류", "비밀번호는 최소 4자 이상이어야 합니다.");
        return;
    }

    // 새 비밀번호 확인
    QString confirmPassword = QInputDialog::getText(this, "비밀번호 확인",
                                                    "새 비밀번호를 다시 입력하세요:",
                                                    QLineEdit::Password, "", &ok);
    if (!ok || confirmPassword.isEmpty()) {
        return;
    }

    if (newPassword != confirmPassword) {
        QMessageBox::warning(this, "입력 오류", "비밀번호가 일치하지 않습니다.");
        return;
    }

    // 비밀번호 변경
    DataManager& dm = DataManager::instance();
    bool success = dm.changePassword(m_userId, oldPassword, newPassword);

    if (success) {
        QMessageBox::information(this, "성공", "비밀번호가 변경되었습니다.");
    } else {
        QMessageBox::critical(this, "오류", "현재 비밀번호가 올바르지 않거나 변경에 실패했습니다.");
    }
}

void SettingsPage::onDeleteAccountClicked()
{
    // 경고 메시지
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "회원 탈퇴",
                                 "정말로 탈퇴하시겠습니까?\n"
                                 "모든 데이터가 삭제되며 복구할 수 없습니다.",
                                 QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes) {
        return;
    }

    // 비밀번호 확인
    bool ok;
    QString password = QInputDialog::getText(this, "비밀번호 확인",
                                             "비밀번호를 입력하여 본인 확인을 해주세요:",
                                             QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) {
        return;
    }

    // 비밀번호 확인을 위해 로그인 시도
    DataManager& dm = DataManager::instance();
    QVariantMap userInfo = dm.getUserInfo(m_userId);
    QString username = userInfo["username"].toString();

    int loginResult = dm.loginUser(username, password);
    if (loginResult != m_userId) {
        QMessageBox::critical(this, "오류", "비밀번호가 올바르지 않습니다.");
        return;
    }

    // 회원 탈퇴
    bool success = dm.deleteUser(m_userId);

    if (success) {
        QMessageBox::information(this, "탈퇴 완료", "회원 탈퇴가 완료되었습니다.");
        emit logoutRequested();  // 로그아웃 처리
    } else {
        QMessageBox::critical(this, "오류", "회원 탈퇴에 실패했습니다.");
    }
}

void SettingsPage::onThemeToggled(bool checked)
{
    // 설정 저장
    QSettings settings("DailyFlow", "Settings");
    settings.setValue("darkMode", checked);

    // 테마 적용
    applyDarkMode(checked);

    // 시그널 발생
    emit themeChanged(checked);
}

void SettingsPage::applyDarkMode(bool isDark)
{
    if (isDark) {
        // 다크 모드 팔레트
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
        darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
        darkPalette.setColor(QPalette::ToolTipBase, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);

        darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
        darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));

        qApp->setPalette(darkPalette);
    } else {
        // 라이트 모드 (기본 팔레트로 리셋)
        qApp->setPalette(QPalette());
    }

    // 테마 변경 후 현재 편집 모드 상태로 LineEdit 스타일 재적용
    setEditMode(m_isEditMode);
}
