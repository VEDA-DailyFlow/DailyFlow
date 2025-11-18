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
            QString value = line.mid(equalPos + 1).trimmed();  // ✅ 수정!

            // 따옴표 제거
            if((value.startsWith('"') && value.endsWith('"')) ||
                (value.startsWith('\'') && value.endsWith('\''))) {
                value = value.mid(1, value.length() - 2);
            }

            envVars[key] = value;
            qDebug() << "Loaded:" << key << "=" << value.left(10) << "...";  // 디버그 출력
        }
    }

    file.close();
    qDebug() << "Total env vars loaded:" << envVars.size();
    return true;
}

QString EnvLoader::get(const QString &key, const QString &defaultValue){
    return envVars.value(key, defaultValue);
}
