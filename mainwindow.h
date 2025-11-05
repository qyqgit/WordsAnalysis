#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSet>
#include <QtCharts/QtCharts>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void loadData(QString path, QSet<QString>& set);
    void asignAgain(const QString str, const QString subStr);
    void asignWord(QString prototype, const QString str);
    void setTextBackgroundColor(QTextEdit *textEdit, const QString &searchText, const QColor &color);
    void saveToFile(QString path, QString fileName, QSet<QString>& set);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QSet<QString> a1Set;
    QSet<QString> a2Set;
    QSet<QString> b1Set;
    QSet<QString> b2Set;
    QSet<QString> b2pSet;
    QSet<QString> c1Set;
    QSet<QString> allSet;

    QSet<QString> a1;
    QSet<QString> a2;
    QSet<QString> b1;
    QSet<QString> b2;
    QSet<QString> b2p;
    QSet<QString> c1;
    QSet<QString> other;
};
#endif // MAINWINDOW_H
