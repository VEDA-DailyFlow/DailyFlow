#ifndef ENVLOADER_H
#define ENVLOADER_H

#include <QString>
#include <QMap>

class EnvLoader
{
public:
    // 스태틱으로 만들어서 프로그램 전역에서 사용 가능
    static bool load(const QString &filePath = ".env");
    static QString get(const QString &key, const QString &defaultValue = "");

private:
    static QMap<QString, QString> envVars;
};

#endif // ENVLOADER_H
