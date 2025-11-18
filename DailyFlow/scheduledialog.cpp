#include "scheduledialog.h"
#include "ui_scheduledialog.h"
#include <QMessageBox>
#include <QTime>

// 추가 모드 생성자
ScheduleDialog::ScheduleDialog(QWidget *parent, const QDate &initialDate)
    : QDialog(parent)
    , ui(new Ui::ScheduleDialog)
    , m_mode(AddMode)
    , m_scheduleId(-1)
{
    ui->setupUi(this);
    setupConnections();

    setWindowTitle("일정 추가");
    ui->saveButton->setText("추가");
    ui->dateEdit->setDate(initialDate);

    QTime now = QTime::currentTime();
    QTime startTime = QTime(now.hour() + 1, 0);  // 다음 시간 정각
    QTime endTime = startTime.addSecs(3600);     // 1시간 후

    ui->startTimeEdit->setTime(startTime);
    ui->endTimeEdit->setTime(endTime);

    validateInputFields();
}

// 수정 모드 생성자
ScheduleDialog::ScheduleDialog(const QVariantMap &scheduleData, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ScheduleDialog)
    , m_mode(EditMode)
    , m_scheduleId(scheduleData["id"].toInt())
{
    ui->setupUi(this);
    setupConnections();

    // 수정 모드 설정
    setWindowTitle("일정 수정");
    ui->saveButton->setText("저장");
    loadScheduleData(scheduleData);

    validateInputFields();
}

ScheduleDialog::~ScheduleDialog()
{
    delete ui;
}

void ScheduleDialog::setupConnections()
{
    // 버튼 시그널 연결
    connect(ui->saveButton, &QPushButton::clicked,
            this, &ScheduleDialog::onSaveClicked);
    connect(ui->cancelButton, &QPushButton::clicked,
            this, &ScheduleDialog::onCancelClicked);

    // 입력 필드 변경 시 실시간 검증
    connect(ui->titleEdit, &QLineEdit::textChanged,
            this, &ScheduleDialog::validateInputFields);
    connect(ui->startTimeEdit, &QTimeEdit::timeChanged,
            this, &ScheduleDialog::validateInputFields);
    connect(ui->endTimeEdit, &QTimeEdit::timeChanged,
            this, &ScheduleDialog::validateInputFields);
}

void ScheduleDialog::loadScheduleData(const QVariantMap &data)
{
    ui->titleEdit->setText(data["title"].toString());
    ui->dateEdit->setDate(QDate::fromString(data["date"].toString(), "yyyy-MM-dd"));
    ui->startTimeEdit->setTime(QTime::fromString(data["start_time"].toString(), "HH:mm"));
    ui->endTimeEdit->setTime(QTime::fromString(data["end_time"].toString(), "HH:mm"));
    ui->locationEdit->setText(data["location"].toString());
    ui->memoEdit->setPlainText(data["memo"].toString());

    // 카테고리 설정
    QString category = data["category"].toString();
    int index = ui->categoryCombo->findText(category, Qt::MatchContains);
    if (index >= 0) {
        ui->categoryCombo->setCurrentIndex(index);
    }
}

QVariantMap ScheduleDialog::getScheduleData() const
{
    QVariantMap data;

    if (m_mode == EditMode) {
        data["id"] = m_scheduleId;
    }

    data["title"] = ui->titleEdit->text().trimmed();
    data["date"] = ui->dateEdit->date().toString("yyyy-MM-dd");
    data["start_time"] = ui->startTimeEdit->time().toString("HH:mm");
    data["end_time"] = ui->endTimeEdit->time().toString("HH:mm");
    data["location"] = ui->locationEdit->text().trimmed();
    data["memo"] = ui->memoEdit->toPlainText().trimmed();

    // 카테고리에서 이모지 제거
    QString category = ui->categoryCombo->currentText();
    category = category.mid(category.indexOf(' ') + 1);
    data["category"] = category;

    return data;
}

bool ScheduleDialog::validateInput()
{
    // 제목 검증
    if (ui->titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "일정 제목을 입력해주세요.");
        ui->titleEdit->setFocus();
        return false;
    }

    // 시간 검증
    if (ui->endTimeEdit->time() <= ui->startTimeEdit->time()) {
        QMessageBox::warning(this, "입력 오류",
                             "종료 시간은 시작 시간보다 늦어야 합니다.");
        ui->endTimeEdit->setFocus();
        return false;
    }

    return true;
}

void ScheduleDialog::validateInputFields()
{
    // 실시간으로 저장 버튼 활성화/비활성화
    bool valid = !ui->titleEdit->text().trimmed().isEmpty() &&
                 ui->endTimeEdit->time() > ui->startTimeEdit->time();

    ui->saveButton->setEnabled(valid);
}

void ScheduleDialog::onSaveClicked()
{
    if (validateInput()) {
        accept();  // QDialog::Accepted 반환
    }
}

void ScheduleDialog::onCancelClicked()
{
    reject();  // QDialog::Rejected 반환
}
