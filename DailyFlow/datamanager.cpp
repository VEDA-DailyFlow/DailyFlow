#include "datamanager.h"
#include <QSqlQuery>  // <-- 1. SQL 쿼리 실행 헤더
#include <QSqlError>  // <-- 2. SQL 에러 확인 헤더
#include <QDebug>     // <-- 3. 디버그 메시지 출력 헤더

DataManager::DataManager(QObject *parent)
    : QObject{parent}
{
    // 4. SQLite 드라이버를 사용하여 DB 연결
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName("dailyflow.db"); // DB 파일 이름 (프로젝트 폴더에 생성됨)

    // 5. DB 열기 시도
    if (!m_db.open()) {
        qDebug() << "Error: Failed to open database:" << m_db.lastError().text();
    } else {
        qDebug() << "Database opened successfully!";
    }

    // 6. 'users' 테이블이 없으면 새로 생성
    QSqlQuery query(m_db); // m_db 연결을 사용하는 쿼리 객체
    bool success = query.exec(
        "CREATE TABLE IF NOT EXISTS users ("
        "    id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "    username TEXT UNIQUE NOT NULL,"
        "    password TEXT NOT NULL,"
        "    name TEXT,"
        "    email TEXT"
        ");"
        );

    if (success) {
        qDebug() << "'users' table created successfully or already exists.";
    } else {
        qDebug() << "Error: Failed to create 'users' table:" << query.lastError().text();
    }
}
