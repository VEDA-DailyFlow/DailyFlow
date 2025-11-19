#ifndef SCHEDULEDIALOG_H
#define SCHEDULEDIALOG_H

#include <QDialog>
#include <QVariantMap>
#include <QDate>

namespace Ui {
class ScheduleDialog;
}

class ScheduleDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        AddMode,
        EditMode
    };

    // 추가 모드 생성자
    explicit ScheduleDialog(QWidget *parent = nullptr,
                            const QDate &initialDate = QDate::currentDate());

    // 수정 모드 생성자
    explicit ScheduleDialog(const QVariantMap &scheduleData,
                            QWidget *parent = nullptr);

    ~ScheduleDialog();

    QVariantMap getScheduleData() const;

private slots:
    void onSaveClicked();
    void onCancelClicked();
    void validateInputFields();

private:
    void setupConnections();
    void loadScheduleData(const QVariantMap &data);
    bool validateInput();
    void applyTheme();

    Ui::ScheduleDialog *ui;
    Mode m_mode;
    int m_scheduleId;
};

#endif // SCHEDULEDIALOG_H
