#include <QJsonDocument>
#include <QFile>

QJsonDocument getJsonDoc(){
    QJsonParseError jsonError;
    QFile file("config.json");
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    file.close();
    return QJsonDocument::fromJson(data,&jsonError);
}

void setJsonDoc(QJsonDocument doc){
    QFile file("config.json");
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson(QJsonDocument::Indented));
    file.close();
}
