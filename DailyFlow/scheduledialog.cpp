#include "scheduledialog.h"
#include "ui_scheduledialog.h"
#include <QMessageBox>
#include <QTime>
#include <QSettings>

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

    applyTheme();
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

    applyTheme();
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

void ScheduleDialog::applyTheme()
{
    QSettings settings("DailyFlow", "Settings");
    bool isDarkMode = settings.value("darkMode", false).toBool();

    if (isDarkMode) {
        setStyleSheet(
            "QDialog { background-color: #353535; }"
            "QLabel { font-size: 13px; font-weight: bold; color: white; }"
            "QLineEdit, QTextEdit, QDateEdit, QTimeEdit, QComboBox {"
            "   padding: 8px;"
            "   border: 2px solid #555;"
            "   border-radius: 4px;"
            "   background-color: #2a2a2a;"
            "   color: white;"
            "   font-size: 13px;"
            "}"
            "QLineEdit:focus, QTextEdit:focus, QDateEdit:focus, QTimeEdit:focus, QComboBox:focus {"
            "   border: 2px solid #42A5F5;"
            "}"
            "QComboBox::drop-down { border: none; width: 30px; }"
            "QComboBox::down-arrow {"
            "   image: none;"
            "   border-left: 5px solid transparent;"
            "   border-right: 5px solid transparent;"
            "   border-top: 5px solid #aaa;"
            "   margin-right: 10px;"
            "}"
            "QComboBox QAbstractItemView {"
            "   background-color: #2a2a2a;"
            "   color: white;"
            "   selection-background-color: #42A5F5;"
            "}"
            "QPushButton#cancelButton {"
            "   background-color: #757575;"
            "   color: white;"
            "   border: none;"
            "   padding: 10px 20px;"
            "   border-radius: 6px;"
            "   font-weight: bold;"
            "   font-size: 13px;"
            "}"
            "QPushButton#cancelButton:hover {"
            "   background-color: #616161;"
            "}"
            "QPushButton#cancelButton:pressed {"
            "   background-color: #424242;"
            "}"
            "QPushButton#saveButton {"
            "   background-color: #2196F3;"
            "   color: white;"
            "   border: none;"
            "   padding: 10px 20px;"
            "   border-radius: 6px;"
            "   font-weight: bold;"
            "   font-size: 13px;"
            "}"
            "QPushButton#saveButton:hover:enabled {"
            "   background-color: #1976D2;"
            "}"
            "QPushButton#saveButton:pressed:enabled {"
            "   background-color: #1565C0;"
            "}"
            "QPushButton#saveButton:disabled {"
            "   background-color: #555555;"
            "   color: #888888;"
            "}"
            );
    } else {
        setStyleSheet(
            "QDialog { background-color: white; }"
            "QLabel { font-size: 13px; font-weight: bold; color: #333; }"
            "QLineEdit, QTextEdit, QDateEdit, QTimeEdit, QComboBox {"
            "   padding: 8px;"
            "   border: 2px solid #e0e0e0;"
            "   border-radius: 4px;"
            "   background-color: white;"
            "   font-size: 13px;"
            "}"
            "QLineEdit:focus, QTextEdit:focus, QDateEdit:focus, QTimeEdit:focus, QComboBox:focus {"
            "   border: 2px solid #2196F3;"
            "}"
            "QComboBox::drop-down { border: none; width: 30px; }"
            "QComboBox::down-arrow {"
            "   image: none;"
            "   border-left: 5px solid transparent;"
            "   border-right: 5px solid transparent;"
            "   border-top: 5px solid #666;"
            "   margin-right: 10px;"
            "}"
            "QPushButton#cancelButton {"
            "   background-color: #757575;"
            "   color: white;"
            "   border: none;"
            "   padding: 10px 20px;"
            "   border-radius: 6px;"
            "   font-weight: bold;"
            "   font-size: 13px;"
            "}"
            "QPushButton#cancelButton:hover {"
            "   background-color: #616161;"
            "}"
            "QPushButton#cancelButton:pressed {"
            "   background-color: #424242;"
            "}"
            "QPushButton#saveButton {"
            "   background-color: #2196F3;"
            "   color: white;"
            "   border: none;"
            "   padding: 10px 20px;"
            "   border-radius: 6px;"
            "   font-weight: bold;"
            "   font-size: 13px;"
            "}"
            "QPushButton#saveButton:hover:enabled {"
            "   background-color: #1976D2;"
            "}"
            "QPushButton#saveButton:pressed:enabled {"
            "   background-color: #1565C0;"
            "}"
            "QPushButton#saveButton:disabled {"
            "   background-color: #BDBDBD;"
            "   color: #757575;"
            "}"
            );
    }
}
