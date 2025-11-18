#include "datamanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDebug>
#include <QCryptographicHash>
#include <QRandomGenerator>
#include <QDateTime>

DataManager::DataManager(QWidget *parent)
    : QWidget{parent}
{
    if(initializeDataBase())
    {
        qDebug() << "initialize DataBase Success";
    }
    else
    {
        qDebug() << "DataBase Fail";
    }
}

DataManager::~DataManager()
{
    if(m_db.isOpen()) {
        m_db.close();
    }
}

bool DataManager::initializeDataBase()
{
    const QString connectionName = "DailyFlowConnection";
    if (QSqlDatabase::contains(connectionName)) {
        m_db = QSqlDatabase::database(connectionName);
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
        m_db.setDatabaseName("dailyflow.db");
    }

    if( !m_db.open() ) {
        qDebug() << "Error: Failed to open database:" << m_db.lastError().text();
        return false;
    }

    QSqlQuery query(m_db);

    // 'users' 테이블 생성
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    username TEXT UNIQUE NOT NULL,"
        "    password TEXT NOT NULL,"
        "    name TEXT,"
        "    email TEXT,"
        "    dateOfBirth TEXT,"
        "    address TEXT"
        ");"
        );
    if( !success ) {
        qDebug() << "Error: Failed to create 'users' table:" << query.lastError().text();
        return false;
    }

    // 'schedules' 테이블 생성
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS schedules ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    userId INTEGER NOT NULL,"
        "    title TEXT NOT NULL,"
        "    date TEXT,"
        "    startTime TEXT,"
        "    endTime TEXT,"
        "    location TEXT,"
        "    memo TEXT,"
        "    category TEXT,"
        "    FOREIGN KEY(userId) REFERENCES users(id)"
        ");"
        );
    if( !success ) {
        qDebug() << "Error: Failed to create 'schedules' table:" << query.lastError().text();
        return false;
    }

    // 'schedule_summaries' 테이블 생성
    success = query.exec(
        "CREATE TABLE IF NOT EXISTS schedule_summaries ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    userId INTEGER NOT NULL,"
        "    date TEXT NOT NULL,"
        "    summary TEXT NOT NULL,"
        "    weatherInfo TEXT,"
        "    createdAt TEXT NOT NULL,"
        "    updatedAt TEXT NOT NULL,"
        "    UNIQUE(userId, date),"
        "    FOREIGN KEY(userId) REFERENCES users(id)"
        ");"
        );
    if( !success ) {
        qDebug() << "Error: Failed to create 'schedule_summaries' table:" << query.lastError().text();
        return false;
    }

    qDebug() << "Database and tables initialized successfully.";
    return true;
}

// ============================================================================
// 유저 정보
// ============================================================================

bool DataManager::addUser(const QString &username,
                          const QString &password,
                          const QString &name,
                          const QString &email,
                          const QString &dateOfBirth,
                          const QString &address)
{
    QSqlQuery query(m_db);

    query.prepare("INSERT INTO users (username, password, name, email, dateOfBirth, address) "
                  "VALUES (:username, :password, :name, :email, :dateOfBirth, :address)");

    QString hashedPassword = hashPassword(password);

    query.bindValue(":username", username);
    query.bindValue(":password", hashedPassword);
    query.bindValue(":name", name);
    query.bindValue(":email", email);
    query.bindValue(":dateOfBirth", dateOfBirth);
    query.bindValue(":address", address);

    if (!query.exec()) {
        qDebug() << "Error: Failed to add user:" << query.lastError().text();
        return false;
    }

    qDebug() << "User" << username << "added successfully!";
    return true;
}

bool DataManager::loginUser(const QString& username, const QString& password)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT password FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (!query.exec()) {
        qDebug() << "Error: Login query failed:" << query.lastError().text();
        return false;
    }

    if (!query.next()) {
        qDebug() << "Error: User not found";
        return false;
    }

    QString storedHash = query.value(0).toString();
    return verifyPassword(password, storedHash);
}

bool DataManager::updateUser(int userId,
                             const QString& name,
                             const QString& email,
                             const QString& dateOfBirth,
                             const QString& address)
{
    QSqlQuery query(m_db);
    query.prepare("UPDATE users SET name = :name, email = :email, "
                  "dateOfBirth = :dateOfBirth, address = :address "
                  "WHERE id = :userId");

    query.bindValue(":name", name);
    query.bindValue(":email", email);
    query.bindValue(":dateOfBirth", dateOfBirth);
    query.bindValue(":address", address);
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Error: Failed to update user:" << query.lastError().text();
        return false;
    }

    qDebug() << "User" << userId << "updated successfully!";
    return true;
}

bool DataManager::changePassword(int userId,
                                 const QString& name,
                                 const QString& oldPassword,
                                 const QString& newPassword)
{
    // 먼저 기존 비밀번호 확인
    QSqlQuery query(m_db);
    query.prepare("SELECT password FROM users WHERE id = :userId");
    query.bindValue(":userId", userId);

    if (!query.exec() || !query.next()) {
        qDebug() << "Error: User not found";
        return false;
    }

    QString storedHash = query.value(0).toString();
    if (!verifyPassword(oldPassword, storedHash)) {
        qDebug() << "Error: Old password is incorrect";
        return false;
    }

    // 새 비밀번호로 업데이트
    QString newHash = hashPassword(newPassword);
    query.prepare("UPDATE users SET password = :password WHERE id = :userId");
    query.bindValue(":password", newHash);
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Error: Failed to change password:" << query.lastError().text();
        return false;
    }

    qDebug() << "Password changed successfully for user" << userId;
    return true;
}

bool DataManager::deleteUser(int userId)
{
    QSqlQuery query(m_db);

    // 먼저 관련 일정 삭제
    query.prepare("DELETE FROM schedules WHERE userId = :userId");
    query.bindValue(":userId", userId);
    query.exec();

    // 요약 삭제
    query.prepare("DELETE FROM schedule_summaries WHERE userId = :userId");
    query.bindValue(":userId", userId);
    query.exec();

    // 사용자 삭제
    query.prepare("DELETE FROM users WHERE id = :userId");
    query.bindValue(":userId", userId);

    if (!query.exec()) {
        qDebug() << "Error: Failed to delete user:" << query.lastError().text();
        return false;
    }

    qDebug() << "User" << userId << "deleted successfully!";
    return true;
}

bool DataManager::userExists(const QString& username)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT COUNT(*) FROM users WHERE username = :username");
    query.bindValue(":username", username);

    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }

    return false;
}

// ============================================================================
// 스케줄 정보
// ============================================================================

bool DataManager::addSchedule(int userId, const QString& title, const QString& date,
                              const QString& startTime, const QString& endTime,
                              const QString& location, const QString& memo,
                              const QString& category)
{
    QSqlQuery query(m_db);
    query.prepare("INSERT INTO schedules (userId, title, date, startTime, endTime, "
                  "location, memo, category) "
                  "VALUES (:userId, :title, :date, :startTime, :endTime, "
                  ":location, :memo, :category)");

    query.bindValue(":userId", userId);
    query.bindValue(":title", title);
    query.bindValue(":date", date);
    query.bindValue(":startTime", startTime);
    query.bindValue(":endTime", endTime);
    query.bindValue(":location", location);
    query.bindValue(":memo", memo);
    query.bindValue(":category", category);

    if (!query.exec()) {
        qDebug() << "Error: Failed to add schedule:" << query.lastError().text();
        return false;
    }

    // 해당 날짜의 요약 무효화
    invalidateSummary(userId, date);

    qDebug() << "Schedule added successfully!";
    return true;
}

QList<QVariantMap> DataManager::getSchedulesByDate(int userId, const QString& date)
{
    QList<QVariantMap> schedules;
    QSqlQuery query(m_db);

    query.prepare("SELECT id, title, date, startTime, endTime, location, memo, category "
                  "FROM schedules WHERE userId = :userId AND date = :date "
                  "ORDER BY startTime");
    query.bindValue(":userId", userId);
    query.bindValue(":date", date);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap schedule;
            schedule["id"] = query.value(0).toInt();
            schedule["title"] = query.value(1).toString();
            schedule["date"] = query.value(2).toString();
            schedule["startTime"] = query.value(3).toString();
            schedule["endTime"] = query.value(4).toString();
            schedule["location"] = query.value(5).toString();
            schedule["memo"] = query.value(6).toString();
            schedule["category"] = query.value(7).toString();
            schedules.append(schedule);
        }
    } else {
        qDebug() << "Error: Failed to get schedules:" << query.lastError().text();
    }

    return schedules;
}

QList<QVariantMap> DataManager::getSchedulesByMonth(int userId, int year, int month)
{
    QList<QVariantMap> schedules;
    QSqlQuery query(m_db);

    // YYYY-MM 형식으로 검색
    QString yearMonth = QString("%1-%2").arg(year, 4, 10, QChar('0')).arg(month, 2, 10, QChar('0'));

    query.prepare("SELECT id, title, date, startTime, endTime, location, memo, category "
                  "FROM schedules WHERE userId = :userId AND date LIKE :yearMonth "
                  "ORDER BY date, startTime");
    query.bindValue(":userId", userId);
    query.bindValue(":yearMonth", yearMonth + "%");

    if (query.exec()) {
        while (query.next()) {
            QVariantMap schedule;
            schedule["id"] = query.value(0).toInt();
            schedule["title"] = query.value(1).toString();
            schedule["date"] = query.value(2).toString();
            schedule["startTime"] = query.value(3).toString();
            schedule["endTime"] = query.value(4).toString();
            schedule["location"] = query.value(5).toString();
            schedule["memo"] = query.value(6).toString();
            schedule["category"] = query.value(7).toString();
            schedules.append(schedule);
        }
    } else {
        qDebug() << "Error: Failed to get schedules by month:" << query.lastError().text();
    }

    return schedules;
}

QList<QVariantMap> DataManager::getAllSchedules(int userId)
{
    QList<QVariantMap> schedules;
    QSqlQuery query(m_db);

    query.prepare("SELECT id, title, date, startTime, endTime, location, memo, category "
                  "FROM schedules WHERE userId = :userId "
                  "ORDER BY date, startTime");
    query.bindValue(":userId", userId);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap schedule;
            schedule["id"] = query.value(0).toInt();
            schedule["title"] = query.value(1).toString();
            schedule["date"] = query.value(2).toString();
            schedule["startTime"] = query.value(3).toString();
            schedule["endTime"] = query.value(4).toString();
            schedule["location"] = query.value(5).toString();
            schedule["memo"] = query.value(6).toString();
            schedule["category"] = query.value(7).toString();
            schedules.append(schedule);
        }
    } else {
        qDebug() << "Error: Failed to get all schedules:" << query.lastError().text();
    }

    return schedules;
}

QVariantMap DataManager::getScheduleById(int scheduleId)
{
    QVariantMap schedule;
    QSqlQuery query(m_db);

    query.prepare("SELECT id, userId, title, date, startTime, endTime, location, memo, category "
                  "FROM schedules WHERE id = :scheduleId");
    query.bindValue(":scheduleId", scheduleId);

    if (query.exec() && query.next()) {
        schedule["id"] = query.value(0).toInt();
        schedule["userId"] = query.value(1).toInt();
        schedule["title"] = query.value(2).toString();
        schedule["date"] = query.value(3).toString();
        schedule["startTime"] = query.value(4).toString();
        schedule["endTime"] = query.value(5).toString();
        schedule["location"] = query.value(6).toString();
        schedule["memo"] = query.value(7).toString();
        schedule["category"] = query.value(8).toString();
    } else {
        qDebug() << "Error: Failed to get schedule by id:" << query.lastError().text();
    }

    return schedule;
}

bool DataManager::updateSchedule(int scheduleId, const QString& title, const QString& date,
                                 const QString& startTime, const QString& endTime,
                                 const QString& location, const QString& memo,
                                 const QString& category)
{
    // 먼저 userId와 기존 date를 가져옴
    QVariantMap oldSchedule = getScheduleById(scheduleId);
    int userId = oldSchedule["userId"].toInt();
    QString oldDate = oldSchedule["date"].toString();

    QSqlQuery query(m_db);
    query.prepare("UPDATE schedules SET title = :title, date = :date, "
                  "startTime = :startTime, endTime = :endTime, location = :location, "
                  "memo = :memo, category = :category "
                  "WHERE id = :scheduleId");

    query.bindValue(":title", title);
    query.bindValue(":date", date);
    query.bindValue(":startTime", startTime);
    query.bindValue(":endTime", endTime);
    query.bindValue(":location", location);
    query.bindValue(":memo", memo);
    query.bindValue(":category", category);
    query.bindValue(":scheduleId", scheduleId);

    if (!query.exec()) {
        qDebug() << "Error: Failed to update schedule:" << query.lastError().text();
        return false;
    }

    // 관련된 날짜의 요약 무효화
    invalidateSummary(userId, oldDate);
    if (date != oldDate) {
        invalidateSummary(userId, date);
    }

    qDebug() << "Schedule" << scheduleId << "updated successfully!";
    return true;
}

bool DataManager::deleteSchedule(int scheduleId)
{
    // 먼저 userId와 date를 가져옴
    QVariantMap schedule = getScheduleById(scheduleId);
    int userId = schedule["userId"].toInt();
    QString date = schedule["date"].toString();

    QSqlQuery query(m_db);
    query.prepare("DELETE FROM schedules WHERE id = :scheduleId");
    query.bindValue(":scheduleId", scheduleId);

    if (!query.exec()) {
        qDebug() << "Error: Failed to delete schedule:" << query.lastError().text();
        return false;
    }

    // 해당 날짜의 요약 무효화
    invalidateSummary(userId, date);

    qDebug() << "Schedule" << scheduleId << "deleted successfully!";
    return true;
}

bool DataManager::deleteSchedulesByDate(int userId, const QString& date)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM schedules WHERE userId = :userId AND date = :date");
    query.bindValue(":userId", userId);
    query.bindValue(":date", date);

    if (!query.exec()) {
        qDebug() << "Error: Failed to delete schedules by date:" << query.lastError().text();
        return false;
    }

    // 해당 날짜의 요약 무효화
    invalidateSummary(userId, date);

    qDebug() << "Schedules on" << date << "deleted successfully!";
    return true;
}

QList<QVariantMap> DataManager::searchSchedules(int userId, const QString& keyword)
{
    QList<QVariantMap> schedules;
    QSqlQuery query(m_db);

    query.prepare("SELECT id, title, date, startTime, endTime, location, memo, category "
                  "FROM schedules WHERE userId = :userId AND "
                  "(title LIKE :keyword OR location LIKE :keyword OR memo LIKE :keyword) "
                  "ORDER BY date, startTime");
    query.bindValue(":userId", userId);
    query.bindValue(":keyword", "%" + keyword + "%");

    if (query.exec()) {
        while (query.next()) {
            QVariantMap schedule;
            schedule["id"] = query.value(0).toInt();
            schedule["title"] = query.value(1).toString();
            schedule["date"] = query.value(2).toString();
            schedule["startTime"] = query.value(3).toString();
            schedule["endTime"] = query.value(4).toString();
            schedule["location"] = query.value(5).toString();
            schedule["memo"] = query.value(6).toString();
            schedule["category"] = query.value(7).toString();
            schedules.append(schedule);
        }
    } else {
        qDebug() << "Error: Failed to search schedules:" << query.lastError().text();
    }

    return schedules;
}

QList<QVariantMap> DataManager::getSchedulesByCategory(int userId, const QString& category)
{
    QList<QVariantMap> schedules;
    QSqlQuery query(m_db);

    query.prepare("SELECT id, title, date, startTime, endTime, location, memo, category "
                  "FROM schedules WHERE userId = :userId AND category = :category "
                  "ORDER BY date, startTime");
    query.bindValue(":userId", userId);
    query.bindValue(":category", category);

    if (query.exec()) {
        while (query.next()) {
            QVariantMap schedule;
            schedule["id"] = query.value(0).toInt();
            schedule["title"] = query.value(1).toString();
            schedule["date"] = query.value(2).toString();
            schedule["startTime"] = query.value(3).toString();
            schedule["endTime"] = query.value(4).toString();
            schedule["location"] = query.value(5).toString();
            schedule["memo"] = query.value(6).toString();
            schedule["category"] = query.value(7).toString();
            schedules.append(schedule);
        }
    } else {
        qDebug() << "Error: Failed to get schedules by category:" << query.lastError().text();
    }

    return schedules;
}

// ============================================================================
// AI 요약
// ============================================================================

QString DataManager::getOrCreateDailySummary(int userId, const QString& date)
{
    // 1. 캐시 확인
    QString cached = getCachedSummary(userId, date);
    if (!cached.isEmpty()) {
        qDebug() << "Using cached summary for" << date;
        return cached;
    }

    // 2. 캐시 없으면 새로 생성 필요 (여기서는 빈 문자열 반환)
    // 실제 AI API 호출은 별도 클래스에서 처리하고, 결과를 saveDailySummary로 저장
    qDebug() << "No cached summary found for" << date << "- need to generate";
    return QString();
}

QString DataManager::getCachedSummary(int userId, const QString& date)
{
    QSqlQuery query(m_db);
    query.prepare("SELECT summary FROM schedule_summaries "
                  "WHERE userId = :userId AND date = :date");
    query.bindValue(":userId", userId);
    query.bindValue(":date", date);

    if (query.exec() && query.next()) {
        return query.value(0).toString();
    }

    return QString();
}

bool DataManager::saveDailySummary(int userId, const QString& date,
                                   const QString& summary, const QString& weatherInfo)
{
    QSqlQuery query(m_db);
    QString currentTime = QDateTime::currentDateTime().toString(Qt::ISODate);

    // UPSERT (INSERT OR REPLACE)
    query.prepare("INSERT OR REPLACE INTO schedule_summaries "
                  "(userId, date, summary, weatherInfo, createdAt, updatedAt) "
                  "VALUES (:userId, :date, :summary, :weatherInfo, "
                  "COALESCE((SELECT createdAt FROM schedule_summaries WHERE userId = :userId AND date = :date), :currentTime), "
                  ":currentTime)");

    query.bindValue(":userId", userId);
    query.bindValue(":date", date);
    query.bindValue(":summary", summary);
    query.bindValue(":weatherInfo", weatherInfo);
    query.bindValue(":currentTime", currentTime);

    if (!query.exec()) {
        qDebug() << "Error: Failed to save daily summary:" << query.lastError().text();
        return false;
    }

    qDebug() << "Daily summary saved for" << date;
    return true;
}

bool DataManager::invalidateSummary(int userId, const QString& date)
{
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM schedule_summaries WHERE userId = :userId AND date = :date");
    query.bindValue(":userId", userId);
    query.bindValue(":date", date);

    if (!query.exec()) {
        qDebug() << "Error: Failed to invalidate summary:" << query.lastError().text();
        return false;
    }

    qDebug() << "Summary invalidated for" << date;
    return true;
}

bool DataManager::cleanOldSummaries(int daysToKeep)
{
    QSqlQuery query(m_db);
    QDateTime cutoffDate = QDateTime::currentDateTime().addDays(-daysToKeep);
    QString cutoffDateStr = cutoffDate.toString(Qt::ISODate);

    query.prepare("DELETE FROM schedule_summaries WHERE updatedAt < :cutoffDate");
    query.bindValue(":cutoffDate", cutoffDateStr);

    if (!query.exec()) {
        qDebug() << "Error: Failed to clean old summaries:" << query.lastError().text();
        return false;
    }

    int deletedCount = query.numRowsAffected();
    qDebug() << "Cleaned" << deletedCount << "old summaries";
    return true;
}

// ============================================================================
// Private 헬퍼 함수
// ============================================================================

QString DataManager::hashPassword(const QString& password)
{
    QByteArray salt;
    for(int i = 0; i < 16; i++) {
        salt.append(static_cast<char>(QRandomGenerator::global()->generate()));
    }

    QByteArray hash = password.toUtf8() + salt;
    for(int i = 0; i < 10000; i++) {
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
    }

    return salt.toHex() + ":" + hash.toHex();
}

bool DataManager::verifyPassword(const QString& password, const QString& storedHash)
{
    QStringList parts = storedHash.split(":");
    if (parts.size() != 2) {
        qDebug() << "Error: Invalid password hash format";
        return false;
    }

    QByteArray salt = QByteArray::fromHex(parts[0].toUtf8());
    QByteArray originalHash = QByteArray::fromHex(parts[1].toUtf8());

    QByteArray hash = password.toUtf8() + salt;
    for(int i = 0; i < 10000; i++) {
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
    }

    return hash == originalHash;
}
