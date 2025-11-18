#include "aiservice.h"
#include "datamanager.h"
#include "envloader.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QEventLoop>
#include <QUrlQuery>
#include <QDate>
#include <QDebug>

AIService& AIService::instance(){
    static AIService instance;
    return instance;
}

AIService::AIService() : m_networkManager(new QNetworkAccessManager(this))
{
    openai_apikey = EnvLoader::get("OPENAI_API_KEY");
    weather_apikey = EnvLoader::get("WEATHER_API_KEY");

    if(openai_apikey.isEmpty()){
        qCritical() << "OPENAI_API_KEY is not set!";
    }

    if(weather_apikey.isEmpty()){
        qCritical() << "WEATHER_API_KEY is not set!";
    }
}

AIService::~AIService(){

}

QString AIService::generateDailySummary(const QString& userId){
    // 1. 사용자 정보 가져오기
    int userIdInt = userId.toInt();
    QVariantMap userInfo = DataManager::instance().getUserInfo(userIdInt);
    QString address = userInfo["address"].toString();

    // 기본 주소가 없으면 서울로 설정
    if(address.isEmpty()) {
        address = "Seoul";
    }

    // 2. 오늘 날짜
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString todayKorean = QDate::currentDate().toString("M월 d일 dddd");

    // 3. 캐시 확인
    QString cached = DataManager::instance().getCachedSummary(userIdInt, today);
    if(!cached.isEmpty()) {
        qDebug() << "Using cached summary for" << today;
        return cached;
    }

    // 4. 오늘 일정 가져오기
    QList<QVariantMap> schedules = DataManager::instance().getSchedulesByDate(userIdInt, today);

    // 5. 날씨 정보 가져오기
    QString weatherInfo = getWeather(address);

    // 6. 일정 정보를 텍스트로 정리
    QString schedulesText;
    if(schedules.isEmpty()) {
        schedulesText = "오늘은 등록된 일정이 없습니다.";
    } else {
        schedulesText = QString("오늘은 총 %1개의 일정이 있습니다:\n").arg(schedules.size());
        for(const QVariantMap& schedule : schedules) {
            schedulesText += QString("- %1 %2 (%3 ~ %4)\n")
            .arg(schedule["startTime"].toString())
                .arg(schedule["title"].toString())
                .arg(schedule["location"].toString())
                .arg(schedule["endTime"].toString());

            if(!schedule["memo"].toString().isEmpty()) {
                schedulesText += QString("  메모: %1\n").arg(schedule["memo"].toString());
            }
        }
    }

    // 7. AI 프롬프트 작성
    QString prompt = QString(
                         "당신은 친근한 일정 관리 비서입니다.\n\n"
                         "날짜: %1\n"
                         "날씨: %2\n"
                         "일정:\n%3\n\n"
                         "다음 형식으로 일정 요약을 작성해주세요:\n\n"
                         "📅 [날짜]\n\n"
                         "🌦️ [날씨 정보]\n\n"
                         "━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                         "✨ AI 추천:\n"
                         "[일정 요약 및 추천 사항]\n\n"
                         "📌 준비물: [필요한 준비물]\n\n"
                         "친근하고 따뜻한 톤으로 작성하되, 날씨를 고려한 실용적인 조언을 포함해주세요."
                         ).arg(todayKorean, weatherInfo, schedulesText);

    // 8. OpenAI API 호출
    QString summary = callOpenAI(prompt);

    if(summary.isEmpty()) {
        summary = QString(
                      "📅 %1\n\n"
                      "🌦️ %2\n\n"
                      "━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                      "✨ 오늘의 일정:\n%3"
                      ).arg(todayKorean, weatherInfo, schedulesText);
    }

    // 9. DB에 저장
    DataManager::instance().saveDailySummary(userIdInt, today, summary, weatherInfo);

    return summary;
}

QString AIService::generateTodaysFortune(const QString& userId){
    // 1. 사용자 정보 가져오기
    int userIdInt = userId.toInt();
    QVariantMap userInfo = DataManager::instance().getUserInfo(userIdInt);
    QString dateOfBirth = userInfo["dateOfBirth"].toString();

    // 2. 오늘 날짜
    QString today = QDate::currentDate().toString("yyyy-MM-dd");

    // 3. AI 프롬프트 작성
    QString prompt = QString(
                         "당신은 친절한 운세 상담사입니다.\n\n"
                         "오늘 날짜: %1\n"
                         "생년월일: %2\n\n"
                         "다음 형식으로 오늘의 운세를 작성해주세요:\n\n"
                         "🔮오늘의 운세\n\n"
                         "전체운: [전체운 내용 - 2-3문장]\n\n"
                         "금전운: [금전운 내용 - 1-2문장]\n\n"
                         "연애운: [연애운 내용 - 1-2문장]\n\n"
                         "건강운: [건강운 내용 - 1-2문장]\n\n"
                         "긍정적이면서도 현실적인 조언을 포함하여 따뜻한 톤으로 작성해주세요."
                         ).arg(today, dateOfBirth);

    // 4. OpenAI API 호출
    QString fortune = callOpenAI(prompt);

    if(fortune.isEmpty()) {
        fortune = QString(
            "🔮 오늘의 운세\n\n"
            "전체운: 오늘은 평온한 하루가 될 것입니다. 계획한 일들을 차근차근 진행해보세요.\n\n"
            "금전운: 안정적인 흐름입니다. 충동적인 지출만 조심하세요.\n\n"
            "연애운: 솔직한 대화가 좋은 결과를 가져옵니다.\n\n"
            "건강운: 컨디션 관리에 신경 쓰는 것이 좋습니다."
            );
    }

    return fortune;
}

//헬퍼 함수
QString AIService::callOpenAI(const QString &prompt){
    if(openai_apikey.isEmpty()) {
        qWarning() << "OpenAI API key is not set!";
        return QString();
    }

    QNetworkRequest request(QUrl("https://api.openai.com/v1/chat/completions"));

    // 헤더 설정
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(openai_apikey).toUtf8());

    // JSON 요청 본문 작성
    QJsonObject json;
    json["model"] = "gpt-3.5-turbo";
    json["temperature"] = 0.7;
    json["max_tokens"] = 1000;

    QJsonArray messages;
    QJsonObject message;
    message["role"] = "user";
    message["content"] = prompt;
    messages.append(message);
    json["messages"] = messages;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    // 동기 방식으로 요청 전송
    QEventLoop loop;
    QNetworkReply* reply = m_networkManager->post(request, data);  // 멤버 변수 사용
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString result;

    if(reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument responseDoc = QJsonDocument::fromJson(response);
        QJsonObject responseObj = responseDoc.object();

        if(responseObj.contains("choices")) {
            QJsonArray choices = responseObj["choices"].toArray();
            if(!choices.isEmpty()) {
                QJsonObject choice = choices[0].toObject();
                QJsonObject messageObj = choice["message"].toObject();
                result = messageObj["content"].toString().trimmed();
            }
        }
    } else {
        qWarning() << "OpenAI API Error:" << reply->errorString();
        qWarning() << "Response:" << reply->readAll();
    }

    reply->deleteLater();
    return result;
}

QString AIService::getWeather(const QString &loc){
    if(weather_apikey.isEmpty()) {
        qWarning() << "Weather API key is not set!";
        return "날씨 정보를 가져올 수 없습니다.";
    }

    // OpenWeatherMap API URL 구성
    QString urlString = QString("https://api.openweathermap.org/data/2.5/weather?q=%1&appid=%2&units=metric&lang=kr")
                            .arg(loc, weather_apikey);

    QUrl url(urlString);  // 먼저 QUrl 객체 생성
    QNetworkRequest request(url);  // 그 다음 QNetworkRequest 생성

    // 동기 방식으로 요청 전송
    QEventLoop loop;
    QNetworkReply* reply = m_networkManager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString weatherInfo = "날씨 정보를 가져올 수 없습니다.";

    if(reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(response);
        QJsonObject obj = doc.object();

        if(obj.contains("main") && obj.contains("weather")) {
            QJsonObject main = obj["main"].toObject();
            QJsonArray weather = obj["weather"].toArray();

            double temp = main["temp"].toDouble();
            int humidity = main["humidity"].toInt();

            QString description = "정보 없음";
            if(!weather.isEmpty()) {
                QJsonObject weatherObj = weather[0].toObject();
                description = weatherObj["description"].toString();
            }

            weatherInfo = QString("%1, 기온 %.1f°C, 습도 %2%")
                              .arg(description)
                              .arg(temp)
                              .arg(humidity);
        }
    } else {
        qWarning() << "Weather API Error:" << reply->errorString();
    }

    reply->deleteLater();
    return weatherInfo;
}
