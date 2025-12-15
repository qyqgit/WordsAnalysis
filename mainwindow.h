#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>
#include <QtCharts/QtCharts>
#include "configdialog.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void updateLangue();

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void asignAgain(const QString str, const QString subStr);
    void asignWord(QString prototype, const QString str);
    void setTextBackgroundColor(QTextEdit *textEdit, const QString &searchText, const QColor &color);
    void saveToFile(QString path, QString fileName, QSet<QString>& set);
private slots:
    void on_pushButtonStat_clicked();
    void on_pushButtonExport_clicked();
    void on_pushButtonPrint_clicked();

private:
    Ui::MainWindow *ui;
    ConfigDialog* configDialog;
    QTranslator trans;
};
#endif // MAINWINDOW_H
