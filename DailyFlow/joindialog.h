#ifndef JOINDIALOG_H
#define JOINDIALOG_H

#include <QDialog>

namespace Ui {
class JoinDialog;
}

class JoinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoinDialog(QWidget *parent = nullptr);
    ~JoinDialog();

private slots:
    void on_submitButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::JoinDialog *ui;
};

#endif // JOINDIALOG_H
