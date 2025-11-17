#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QSqlDatabase>

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);

private:
    QSqlDatabase m_db;  // 데이터베이스 연결 객체
};

#endif // DATAMANAGER_H
