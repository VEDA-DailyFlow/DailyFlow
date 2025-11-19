#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QList>
#include <QVariantMap>

// 기본적인 DB 생성과 프로그램의 기능 수행을 위해 DB에 접속해야하는 함수들의 모음
class DataManager : public QWidget
{
    Q_OBJECT
public:
    static DataManager& instance();

    // ============================================================================
    // 사용자 관리
    // ============================================================================

    // 로그인
    int loginUser(const QString& username, const QString& password);

    // 회원 가입
    bool addUser(const QString &username,
                 const QString &password,
                 const QString &name,
                 const QString &email,
                 const QString &dateOfBirth,
                 const QString &address);
    bool userExists(const QString& username);  // 아이디 중복 확인

    // 회원 정보 수정
    bool updateUser(int userId,
                    const QString& name,
                    const QString& email,
                    const QString& dateOfBirth,
                    const QString& address);
    bool changePassword(int userId,
                    const QString& oldPassword,
                    const QString& newPassword);
    bool deleteUser(int userId);

    // 회원 정보 조회
    QVariantMap getUserInfo(int userId);

    // ============================================================================
    // 일정 관리
    // ============================================================================

    // 일정 추가
    bool addSchedule(int userId, const QString& title, const QString& date,
                     const QString& startTime, const QString& endTime,
                     const QString& location, const QString& memo,
                     const QString& category);

    // 일정 조회
    QList<QVariantMap> getSchedulesByDate(int userId, const QString& date);   // 해당 날짜의 일정 목록
    QList<QVariantMap> getSchedulesByMonth(int userId, int year, int month);  // 해당 월의 전체 일정 목록 (일정페이지의 캘린더 표시용)
    QList<QVariantMap> getSchedulesForNextDays(int userId, int days = 7);     // 오늘부터 N일간의 일정 (홈페이지용)
    QList<QVariantMap> getAllSchedules(int userId);  // userID가 가진 모든 일정을 가져옴 (현재 사용처 없음)
    QVariantMap getScheduleById(int scheduleId);  // 각 일정은 고유 scheduleID를 가지며 그 일정의 모든 상세 정보를 가져옴
    QList<QVariantMap> searchSchedules(int userId, const QString& keyword);  // 검색(제목, 장소, 메모 내용 중)하여 '키워드'가 포함된 일정을 가져옴 (현재 사용처 없음)
    QList<QVariantMap> getSchedulesByCategory(int userId, const QString& category);  // 카테고리에 해당하는 일정만 필터링해서 가져옴 (현재 사용처 없음)

    // 일정 수정 - onEditButtonClicked() 함수의 callee
    bool updateSchedule(int scheduleId, const QString& title, const QString& date,
                        const QString& startTime, const QString& endTime,
                        const QString& location, const QString& memo,
                        const QString& category);

    // 일정 삭제 - onDeleteButtonClicked 함수의 callee
    bool deleteSchedule(int scheduleId);
    bool deleteSchedulesByDate(int userId, const QString& date);  // 현재 날짜에 있는 모든 일정을 한방에 삭제 (현재 사용처 없음)

    // ============================================================================
    // AI 요약 관리
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

signals:
    void scheduleChanged(int userId);

private:
    explicit DataManager();
    ~DataManager();
    DataManager(const DataManager&) = delete;
    DataManager& operator=(const DataManager&) = delete;

    bool initializeDataBase();

    QSqlDatabase m_db;  // 데이터베이스 객체
    QSqlTableModel* m_model;

    // 비밀번호 해싱 헬퍼 함수
    QString hashPassword(const QString& password);
    bool verifyPassword(const QString& password, const QString& storedHash);
};

#endif // DATAMANAGER_H
