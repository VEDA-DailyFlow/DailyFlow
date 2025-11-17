#include "envloader.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>

QMap<QString, QString> EnvLoader::envVars;

bool EnvLoader::load(const QString &filePath){
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        qDebug() << "Cannot open .env file:" << filePath;
        return false;
    }

    QTextStream in(&file);
    while(!in.atEnd()){
        QString line = in.readLine().trimmed();

        // 주석이나 빈 줄 무시
        if(line.isEmpty() || line.startsWith("#")) {
            continue;
        }

        int equalPos = line.indexOf('=');
        if(equalPos != -1){
            QString key = line.left(equalPos).trimmed();
            QString value = line.right(equalPos).trimmed();

            if((value.startsWith('"') && value.endsWith('"')) || (value.startsWith('\'') && value.endsWith('\''))){
                value = value.mid(1, value.length() - 2);
            }
            envVars[key] = value;
        }

    }

    file.close();
    return true;
}

QString EnvLoader::get(const QString &key, const QString &defaultValue){
    return envVars.value(key, defaultValue);
}
