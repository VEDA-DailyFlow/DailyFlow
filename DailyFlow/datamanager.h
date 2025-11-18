#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QList>
#include <QVariantMap>

class DataManager : public QWidget
{
    Q_OBJECT
public:
    static DataManager* instance();

    // ============================================================================
    // 유저 정보
    // ============================================================================
    bool addUser(const QString &username,
                 const QString &password,
                 const QString &name,
                 const QString &email,
                 const QString &dateOfBirth,
                 const QString &address);
    int loginUser(const QString& username, const QString& password);
    bool updateUser(int userId,
                    const QString& name,
                    const QString& email,
                    const QString& dateOfBirth,
                    const QString& address);
    bool changePassword(int userId,
                    const QString& oldPassword,
                    const QString& newPassword);
    bool deleteUser(int userId);
    bool userExists(const QString& username);


    // ============================================================================
    // 스케줄 정보
    // ============================================================================
    bool addSchedule(int userId, const QString& title, const QString& date,
                     const QString& startTime, const QString& endTime,
                     const QString& location, const QString& memo,
                     const QString& category);

    // 일정 조회
    QList<QVariantMap> getSchedulesByDate(int userId, const QString& date);
    QList<QVariantMap> getSchedulesByMonth(int userId, int year, int month);
    QList<QVariantMap> getAllSchedules(int userId);
    QVariantMap getScheduleById(int scheduleId);

    // 일정 수정
    bool updateSchedule(int scheduleId, const QString& title, const QString& date,
                        const QString& startTime, const QString& endTime,
                        const QString& location, const QString& memo,
                        const QString& category);

    // 일정 삭제
    bool deleteSchedule(int scheduleId);
    bool deleteSchedulesByDate(int userId, const QString& date);

    // 일정 검색
    QList<QVariantMap> searchSchedules(int userId, const QString& keyword);

    // 카테고리별 일정 조회
    QList<QVariantMap> getSchedulesByCategory(int userId, const QString& category);

    // ============================================================================
    // AI 유저
    // ============================================================================
    // 요약 있으면 가져오고 없으면 생성
    QString getOrCreateDailySummary(int userId, const QString& date);

    // 요약 캐시 확인
    QString getCachedSummary(int userId, const QString& date);

    // 요약 저장/업데이트
    bool saveDailySummary(int userId, const QString& date,
                          const QString& summary, const QString& weatherInfo);

    // 요약 무효화 (일정 변경 시)
    bool invalidateSummary(int userId, const QString& date);

    // 오래된 요약 삭제 (선택사항)
    bool cleanOldSummaries(int daysToKeep = 30);


private:
    DataManager(QWidget *parent = nullptr);
    ~DataManager();

    bool initializeDataBase();

    static DataManager* m_instance; //싱글톤 인스턴스
    QSqlDatabase m_db;  // 데이터베이스 객체
    QSqlTableModel* m_model;

    // 비밀번호 해싱 헬퍼 함수
    QString hashPassword(const QString& password);
    bool verifyPassword(const QString& password, const QString& storedHash);
};

#endif // DATAMANAGER_H
