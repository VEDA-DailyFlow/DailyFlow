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
    // QObject의 부모-자식 관계로 자동 삭제됨
}

QString AIService::generateDailySummary(int userId){
    // 1. 사용자 정보 가져오기
    QVariantMap userInfo = DataManager::instance().getUserInfo(userId);
    QString userName = userInfo["name"].toString();
    QString userAddress = userInfo["address"].toString();
    QString userBirth = userInfo["dateOfBirth"].toString();

    // 주소에서 지역명 파싱
    QString location = parseLocationFromAddress(userAddress);
    if(location.isEmpty()) {
        location = "Seoul";  // 기본값
    }

    // 2. 오늘 날짜
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString todayKorean = QDate::currentDate().toString("M월 d일 dddd");

    // 3. 캐시 확인
    QString cached = DataManager::instance().getCachedSummary(userId, today);
    if(!cached.isEmpty()) {
        qDebug() << "Using cached summary for" << today;
        return cached;
    }

    // 4. 오늘 일정 가져오기
    QList<QVariantMap> schedules = DataManager::instance().getSchedulesByDate(userId, today);

    // 5. 날씨 정보 가져오기
    QString weatherInfo = getWeather(location);

    // 6. 일정 정보를 텍스트로 정리
    QString schedulesText;
    if(schedules.isEmpty()) {
        schedulesText = "오늘은 등록된 일정이 없습니다.";
    } else {
        schedulesText = QString("오늘은 총 %1개의 일정이 있습니다:\n").arg(schedules.size());
        for(const QVariantMap& schedule : schedules) {
            schedulesText += QString("- %1 %2")
            .arg(schedule["startTime"].toString())
                .arg(schedule["title"].toString());

            if(!schedule["location"].toString().isEmpty()) {
                schedulesText += QString(" (%1)").arg(schedule["location"].toString());
            }
            if(!schedule["endTime"].toString().isEmpty()) {
                schedulesText += QString(" ~ %1").arg(schedule["endTime"].toString());
            }
            schedulesText += "\n";

            if(!schedule["memo"].toString().isEmpty()) {
                schedulesText += QString("  메모: %1\n").arg(schedule["memo"].toString());
            }
        }
    }

    // 7. AI 프롬프트 작성 (이름 포함)
    QString prompt = QString(
                         "당신은 친근한 일정 관리 비서입니다.\n\n"
                         "사용자 이름: %1\n"
                         "날짜: %2\n"
                         "날씨: %3\n"
                         "일정:\n%4\n\n"
                         "다음 형식으로 일정 요약을 작성해주세요:\n\n"
                         "📅 %2\n\n"
                         "🌦️ [날씨 정보를 자연스럽게 표현]\n\n"
                         "━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                         "✨ AI 추천:\n"
                         "[%1님의 일정을 분석하여 친근하게 요약하고, 날씨를 고려한 실용적인 조언 제공]\n\n"
                         "📌 준비물: [필요한 준비물]\n\n"
                         "따뜻하고 친근한 톤으로 작성하되, 사용자 이름을 자연스럽게 사용해주세요."
                         ).arg(userName, todayKorean, weatherInfo, schedulesText);

    // 8. OpenAI API 호출
    QString summary = callOpenAI(prompt);

    if(summary.isEmpty()) {
        summary = QString(
                      "📅 %1\n\n"
                      "🌦️ %2\n\n"
                      "━━━━━━━━━━━━━━━━━━━━━━━━━━━\n\n"
                      "✨ %3님의 오늘 일정:\n%4\n\n"
                      "좋은 하루 보내세요! 😊"
                      ).arg(todayKorean, weatherInfo, userName, schedulesText);
    }

    // 9. DB에 저장
    DataManager::instance().saveDailySummary(userId, today, summary, weatherInfo);

    return summary;
}

QString AIService::generateTodaysFortune(int userId){
    // 1. 사용자 정보 가져오기
    QVariantMap userInfo = DataManager::instance().getUserInfo(userId);
    QString userName = userInfo["name"].toString();
    QString userBirth = userInfo["dateOfBirth"].toString();

    // 2. 오늘 날짜
    QString today = QDate::currentDate().toString("yyyy-MM-dd");
    QString todayKorean = QDate::currentDate().toString("M월 d일 dddd");

    // 3. 생년월일 파싱 (나이, 띠 등 계산 가능)
    QDate birthDate = QDate::fromString(userBirth, "yyyy-MM-dd");
    int age = QDate::currentDate().year() - birthDate.year();

    // 4. AI 프롬프트 작성 (이름과 생년월일 포함)
    QString prompt = QString(
                         "당신은 친절하고 따뜻한 운세 상담사입니다.\n\n"
                         "사용자 이름: %1\n"
                         "오늘 날짜: %2\n"
                         "생년월일: %3 (만 %4세)\n\n"
                         "다음 형식으로 오늘의 운세를 작성해주세요:\n\n"
                         "🔮 %2 %1님의 운세\n"
                         "생년월일: %3 (만 %4세)\n\n"
                         "전체운: [전체운 내용 - 2-3문장, 긍정적이면서도 현실적인 조언]\n\n"
                         "금전운: [금전운 내용 - 1-2문장]\n\n"
                         "연애운: [연애운 내용 - 1-2문장]\n\n"
                         "건강운: [건강운 내용 - 1-2문장]\n\n"
                         "💡 오늘의 조언: [구체적이고 실용적인 한마디]\n\n"
                         "따뜻하고 긍정적인 톤으로, %1님을 자연스럽게 호칭하며 작성해주세요."
                         ).arg(userName, todayKorean, userBirth, QString::number(age));

    // 5. OpenAI API 호출
    QString fortune = callOpenAI(prompt);

    if(fortune.isEmpty()) {
        fortune = QString(
                      "🔮 %2 %1님의 운세\n"
                      "생년월일: %3 (만 %4세)\n\n"
                      "전체운: 오늘은 평온한 하루가 될 것입니다. 계획한 일들을 차근차근 진행해보세요.\n\n"
                      "금전운: 안정적인 흐름입니다. 충동적인 지출만 조심하세요.\n\n"
                      "연애운: 솔직한 대화가 좋은 결과를 가져옵니다.\n\n"
                      "건강운: 컨디션 관리에 신경 쓰는 것이 좋습니다.\n\n"
                      "💡 오늘의 조언: 작은 것부터 차근차근 실천하세요!"
                      ).arg(userName, todayKorean, userBirth, QString::number(age));
    }

    return fortune;
}

QString AIService::parseLocationFromAddress(const QString &address){
    qDebug() << "address: " << address;
    if(address.isEmpty()) {
        return "Seoul";
    }

    // 주소 파싱 로직
    // 예: "서울특별시 강남구 테헤란로" -> "Seoul" 또는 "Gangnam,Seoul"
    // 예: "경기도 성남시 분당구" -> "Seongnam"

    QStringList parts = address.split(" ");
    if(parts.isEmpty()) {
        return "Seoul";
    }

    QString firstPart = parts[0];

    // 시/도 단위로 매핑
    if(firstPart.contains("서울")) return "Seoul";
    if(firstPart.contains("부산")) return "Busan";
    if(firstPart.contains("인천")) return "Incheon";
    if(firstPart.contains("대구")) return "Daegu";
    if(firstPart.contains("대전")) return "Daejeon";
    if(firstPart.contains("광주")) return "Gwangju";
    if(firstPart.contains("울산")) return "Ulsan";
    if(firstPart.contains("세종")) return "Sejong";

    return "Seoul";  // 기본값
}

//헬퍼 함수
QString AIService::callOpenAI(const QString &prompt){
    if(openai_apikey.isEmpty()) {
        qWarning() << "OpenAI API key is not set!";
        return QString();
    }

    QString urlString = "https://api.openai.com/v1/chat/completions";
    QUrl url(urlString);
    QNetworkRequest request(url);

    // 헤더 설정
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", QString("Bearer %1").arg(openai_apikey).toUtf8());

    // JSON 요청 본문 작성
    QJsonObject json;
    json["model"] = "gpt-4o-mini";
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
    QNetworkReply* reply = m_networkManager->post(request, data);
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

QString AIService::getWeather(const QString &location){
    if(weather_apikey.isEmpty()) {
        qWarning() << "Weather API key is not set!";
        return "날씨 정보를 가져올 수 없습니다.";
    }

    qDebug() << "Fetching weather for location:" << location;

    // OpenWeatherMap API URL 구성
    QString urlString = QString("https://api.openweathermap.org/data/2.5/weather?q=%1&appid=%2&units=metric&lang=kr")
                            .arg(location, weather_apikey);

    QUrl url(urlString);
    QNetworkRequest request(url);

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

            QString description = "맑음";
            if(!weather.isEmpty()) {
                QJsonObject weatherObj = weather[0].toObject();
                description = weatherObj["description"].toString();
            }

            // 수정된 부분: arg를 올바르게 사용
            weatherInfo = QString("%1, 기온 %2°C, 습도 %3%")
                              .arg(description)
                              .arg(temp, 0, 'f', 1)  // 소수점 1자리
                              .arg(humidity);

            qDebug() << "Weather info created:" << weatherInfo;
        } else {
            qDebug() << "JSON structure invalid. Full response:" << doc.toJson();
        }
    } else {
        qWarning() << "Weather API Error:" << reply->errorString();
    }

    reply->deleteLater();
    return weatherInfo;
}
