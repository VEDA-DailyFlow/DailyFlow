#include "aiservice.h"

AIService& AIService::instance(){
    static AIService instance;
    return instance;
}

AIService::AIService(){
    openai_apikey = "";
    weather_apikey = "";
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
