#ifndef JOINDIALOG_H
#define JOINDIALOG_H

#include <QWidget>

namespace Ui {
class JoinDialog;
}

class JoinDialog : public QWidget
{
    Q_OBJECT

public:
    explicit JoinDialog(QWidget *parent = nullptr);
    ~JoinDialog();

private:
    Ui::JoinDialog *ui;
};

#endif // JOINDIALOG_H
