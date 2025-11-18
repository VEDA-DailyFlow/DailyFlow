#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>

class DataManager : public QWidget
{
    Q_OBJECT
public:
    DataManager(QWidget *parent = nullptr);
    ~DataManager();

    void initializeDataBase();
    bool addUser(const QString &username,
                 const QString &password,
                 const QString &name,
                 const QString &email,
                 const QString &dateOfBirth,
                 const QString &address);

private:
    QSqlDatabase m_db;  // 데이터베이스 객체
    QSqlTableModel* m_model;
};

#endif // DATAMANAGER_H
