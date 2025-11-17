#include "settingspage.h"
#include <QGroupBox>

SettingsPage::SettingsPage(const QString &userId, QWidget *parent)
    : QWidget(parent)
    , m_userId(userId)
{
    setupUI();
}

void SettingsPage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // 설정 페이지 제목
    QLabel *titleLabel = new QLabel("⚙️ 설정", this);
    titleLabel->setStyleSheet(
        "font-size: 24px;"
        "font-weight: bold;"
        "color: #2196F3;"
        "padding: 20px;"
        );
    mainLayout->addWidget(titleLabel);

    // 안내 메시지
    QGroupBox *infoBox = new QGroupBox("안내", this);
    infoBox->setStyleSheet(
        "QGroupBox {"
        "   font-size: 14px;"
        "   border: 1px solid #ddd;"
        "   border-radius: 8px;"
        "   margin-top: 10px;"
        "   padding: 20px;"
        "}"
        );

    QVBoxLayout *infoLayout = new QVBoxLayout(infoBox);
    QLabel *infoLabel = new QLabel(
        "설정 페이지는 추후 구현 예정입니다.\n\n"
        "구현 예정 기능:\n"
        "• 프로필 설정\n"
        "• 알림 설정\n"
        "• 테마 변경\n"
        "• 카테고리 관리",
        this
        );
    infoLabel->setStyleSheet("font-size: 13px; line-height: 1.6;");
    infoLayout->addWidget(infoLabel);

    mainLayout->addWidget(infoBox);
    mainLayout->addStretch();
}
