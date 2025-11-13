#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QColor>
#include <QSet>
#include <QJsonDocument>

#define OXFORD_CONFIG_TYPE      0
#define CAMBRIDGE_CONFIG_TYPE   1
#define CUSTOM_CONFIG_TYPE      2

typedef struct Item{
    QColor color;
    QString name;
    QString filePath;
    QSet<QString> stdSet;
    QSet<QString> existSet;
}Item_t;

extern QList<Item_t> gItemList;
extern QJsonDocument gJsonDoc;
extern QSet<QString> gAllSet;

#endif // COMMON_H
