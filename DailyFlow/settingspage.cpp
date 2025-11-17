#include "settingspage.h"
#include "ui_settingspage.h"

SettingsPage::SettingsPage(const QString &userId, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SettingsPage)
    , m_userId(userId)
{
    ui->setupUi(this);
}

SettingsPage::~SettingsPage()
{
    delete ui;
}
