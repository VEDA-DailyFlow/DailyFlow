#include "datamanager.h"
#include <QSqlQuery>  // <-- 1. SQL 쿼리 실행 헤더
#include <QSqlError>  // <-- 2. SQL 에러 확인 헤더
#include <QDebug>     // <-- 3. 디버그 메시지 출력 헤더

DataManager::DataManager(QWidget *parent)
    : QWidget{parent}
{

}
