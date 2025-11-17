#include "aiservice.h"
#include "envloader.h"
#include <QDebug>

AIService& AIService::instance(){
    static AIService instance;
    return instance;
}

AIService::AIService(){
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

QString AIService::generateDailySummary(const QString& userID){

}

QString AIService::generateTodaysFortune(const QString& userID){

}

//헬퍼 함수
QString AIService::callOpenAI(const QString &prompt){

}
QString AIService::getWeather(const QString &loc){

}
