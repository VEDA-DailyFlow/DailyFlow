#include "joindialog.h"
#include "ui_joindialog.h"
#include "datamanager.h"

JoinDialog::JoinDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::JoinDialog)
{
    ui->setupUi(this);
    ui->submitButton
}

JoinDialog::~JoinDialog()
{
    delete ui;
}

void JoinDialog::on_submitButton_clicked()
{

}

