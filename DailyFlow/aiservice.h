#ifndef AISERVICE_H
#define AISERVICE_H

#include <QObject>
#include <QString>

class AIService : public QObject
{
    Q_OBJECT

public:
    static AIService& instance();                   // 싱글톤 인스턴스

    QString generateDailySummary(const QString &userId);            // AI일정 요약 + 날씨
    QString generateTodaysFortune(const QString &userId);           // 오늘의 운세

private:
    AIService();                                    // 싱글톤이기에 private으로 선언 -> 외부에서 생성 불가
    ~AIService();
    AIService(const AIService&) = delete;
    AIService& operator=(const AIService&) = delete;

    QString openai_apikey;
    QString weather_apikey;

    QString callOpenAI(const QString &prompt);
    QString getWeather(const QString &loc);
};

#endif // AISERVICE_H
