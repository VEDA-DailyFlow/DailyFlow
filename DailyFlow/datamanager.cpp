#include "datamanager.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QFile>
#include <QDebug>

DataManager::DataManager(QWidget *parent)
    : QWidget{parent}
{
    if(initializeDataBase())  // 1. SQLite 데이터베이스 파일 생성
    {

    }
}

bool DataManager::initializeDataBase()  // 1. SQLite 데이터베이스 파일 생성
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
