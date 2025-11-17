#ifndef ENVLOADER_H
#define ENVLOADER_H

#include <QString>
#include <QMap>

class EnvLoader
{
public:
    static bool load(const QString &filePath = ".env");
    static QString get(const QString &key, const QString &defaultValue = "");

private:
    static QMap<QString, QString> envVars;
};

#endif // ENVLOADER_H
