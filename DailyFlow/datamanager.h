#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QWidget>
#include <QSqlDatabase>

class DataManager : public QWidget
{
    Q_OBJECT
public:
    DataManager(QWidget *parent = nullptr);
    ~DataManager();

private:
    QSqlDatabase m_db;  // 데이터베이스 연결 객체
};

#endif // DATAMANAGER_H
