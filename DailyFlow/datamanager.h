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

    bool initializeDataBase();
    bool addUser(const QString &username,
                 const QString &password,
                 const QString &name,
                 const QString &email,
                 const QString &dateOfBirth,
                 const QString &address);
    bool loginUser(const QString& username, const QString& password);
    bool updateUser(int userId,
                    const QString& name,
                    const QString& email,
                    const QString& dateOfBirth,
                    const QString& address);
    bool changePassword(int userId,
                    const QString& name,
                    const QString& oldPassword,
                    const QString& newPassword);

private:
    QSqlDatabase m_db;  // 데이터베이스 객체
    QSqlTableModel* m_model;

    // 비밀번호 해싱 헬퍼 함수
    QString hashPassword(const QString& password);
    bool verifyPassword(const QString& password, const QString& storedHash);
};

#endif // DATAMANAGER_H
