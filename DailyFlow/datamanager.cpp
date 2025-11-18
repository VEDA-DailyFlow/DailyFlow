#include "datamanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDebug>
#include <QCryptographicHash>
#include <QRandomGenerator>

DataManager::DataManager(QWidget *parent)
    : QWidget{parent}
{
    if(initializeDataBase())  // 1. SQLite 데이터베이스 파일 생성
    {
        qDebug() << "initialize DataBase Success";
    }
    else
    {
        qDebug() << "DataBase Fail";
    }
}

bool DataManager::initializeDataBase()  // 1. 데이터베이스와 users, schedules 테이블 생성
{
    // 고유한 연결 이름을 사용해 DB 중복 연결 방지
    const QString connectionName = "DailyFlowConnection";
    if (QSqlDatabase::contains(connectionName)) {  // 기존 연결이 있을 때
        m_db = QSqlDatabase::database(connectionName);
    } else {
        m_db = QSqlDatabase::addDatabase("QSQLITE", connectionName);  // 기존 연결이 없을 -> 새로운 연결 생성
        m_db.setDatabaseName("dailyflow.db"); // DB 파일 이름 설정
    }

    // DB 오픈 (없으면 생성)
    if( !m_db.open() ) {
        qDebug() << "Error: Failed to open database:" << m_db.lastError().text();
        return false;
    }

    // 쿼리 객체를 m_db 위에서 실행하도록 함
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
        "    userId INTEGER NOT NULL," // 'users' 테이블의 id와 연결
        "    title TEXT NOT NULL,"
        "    date TEXT,"
        "    startTime TEXT,"
        "    endTime TEXT,"
        "    location TEXT,"
        "    memo TEXT,"
        "    category TEXT,"
        "    FOREIGN KEY(userId) REFERENCES users(id)" // 외래 키 설정
        ");"
        );
    if( !success ) {
        qDebug() << "Error: Failed to create 'schedules' table:" << query.lastError().text();
        return false; // <-- 중요
    }

    // 초기화 성공
    qDebug() << "Database and tables initialized successfully.";
    return true;
}


// USER 관련 함수
bool DataManager::addUser(const QString &username,
                          const QString &password,
                          const QString &name,
                          const QString &email,
                          const QString &dateOfBirth,
                          const QString &address)
{
    QSqlQuery query(m_db);

    query.prepare("INSERT INTO users (username, password, name, email, dateOfBirth, address) "
                  "VALUES (:username, :password, :name, :email, :dateOfBirth, :address)");  // 새 컬럼 추가

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


QString DataManager::hashPassword(const QString& password)
{
    // Salt 생성 (랜덤 16바이트)
    QByteArray salt;
    for(int i = 0; i < 16; i++) {
        salt.append(static_cast<char>(QRandomGenerator::global()->generate()));
    }

    // 반복 해싱
    QByteArray hash = password.toUtf8() + salt;
    for(int i = 0; i < 10000; i++) {
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
    }

    // Salt와 Hash를 함께 저장 (salt:hash 형식)
    return salt.toHex() + ":" + hash.toHex();
}

bool DataManager::verifyPassword(const QString& password, const QString& storedHash)
{
    QStringList parts = storedHash.split(":");
    QByteArray salt = QByteArray::fromHex(parts[0].toUtf8());
    QByteArray originalHash = QByteArray::fromHex(parts[1].toUtf8());

    QByteArray hash = password.toUtf8() + salt;
    for(int i = 0; i < 10000; i++) {
        hash = QCryptographicHash::hash(hash, QCryptographicHash::Sha256);
    }

    return hash == originalHash;
}
