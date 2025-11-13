#include "mainwindow.h"
#include "common.h"
#include <QApplication>

QList<Item_t> gItemList;
QJsonDocument gJsonDoc;
QSet<QString> gAllSet;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
