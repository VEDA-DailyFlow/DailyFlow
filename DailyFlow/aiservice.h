#ifndef AISERVICE_H
#define AISERVICE_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>

class AIService : public QObject
{
    Q_OBJECT

public:
    static AIService& instance();

    QString generateDailySummary(int userId);
    QString generateTodaysFortune(int userId);

private:
    AIService();
    ~AIService();
    AIService(const AIService&) = delete;
    AIService& operator=(const AIService&) = delete;

    QString openai_apikey;
    QString weather_apikey;

    QNetworkAccessManager* m_networkManager;  // 멤버 변수로 추가

    QString callOpenAI(const QString &prompt);
    QString getWeather(const QString &loc);
    QString parseLocationFromAddress(const QString &address);
};

#endif // AISERVICE_H
