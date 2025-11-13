#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QRegularExpression>
#include <QFile>
#include <QBarSet>
#include <QTextFormat>
#include <QUrl>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include "common.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    configDialog = new ConfigDialog(this);
    connect(ui->action, &QAction::triggered, this, [=]{
        configDialog->show();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(!gItemList.size()) return;
    ui->pushButton_2->setEnabled(true);

    QString str = ui->textEdit->toPlainText();
    QRegularExpression reg1 = QRegularExpression("[\\p{P}\\p{S}\\d]");
    QRegularExpression reg2 = QRegularExpression("[\\s]");
    str = str.replace(reg1, " ");
    qDebug() << str;
    QList<QString> words = str.split(reg2);
    words.removeAll("");
    QSet<QString> wordsSet;
    for(int i = 0; i < words.size(); i++){
        wordsSet.insert(words[i]);
    }
    qDebug() << "Total words:" << words.size();
    qDebug() << "Words:" << wordsSet.size();

    for(int i = 0; i < gItemList.size() - 1; i++){
        gItemList[i].existSet = (wordsSet & gItemList[i].stdSet);
    }

    gItemList.last().existSet = (wordsSet - gAllSet);

    foreach (QString str, gItemList.last().existSet) {
        if(str[0].isUpper()){
            QString prototype = str.toLower();
            asignWord(prototype, str);
        }

        //可数名词变复数
        if(str.endsWith("s")){
            asignAgain(str, "s");
        }
        if(str.endsWith("es")){
            asignAgain(str, "es");
        }
        if(str.endsWith("ies")){
            asignAgain(str, "ies");
        }
        if(str.endsWith("ves")){
            asignAgain(str, "ves");
        }
        if(str.endsWith("ing")){
            asignAgain(str, "ing");
        }
        if(str.endsWith("ying")){
            asignAgain(str, "ying");
        }
        if(str.endsWith("ed")){
            asignAgain(str, "ed");
        }
        if(str.endsWith("ied")){
            asignAgain(str, "ied");
        }
        if(str.endsWith("er")){
            asignAgain(str, "er");
        }
        if(str.endsWith("ier")){
            asignAgain(str, "ier");
        }
        if(str.endsWith("est")){
            asignAgain(str, "est");
        }
        if(str.endsWith("iest")){
            asignAgain(str, "iest");
        }
    }

    for(int i = 0; i < gItemList.size(); i++){
        qDebug() << gItemList[i].name << gItemList[i].existSet << gItemList[i].existSet.size();
    }

    //画柱状图
    QBarSeries* barSeries = new QBarSeries(this);
    barSeries->setLabelsVisible(true);
    barSeries->setBarWidth(0.5);
    for(int i = 0; i < gItemList.size(); i++){
        QBarSet* barSet = new QBarSet(gItemList[i].name, this);
        barSet->setColor(gItemList[i].color);

                *barSet << gItemList[i].existSet.size();

        barSeries->append(barSet);
    }

    QChart* chart = new QChart();
    chart->addSeries(barSeries);
    chart->setTitle("统计结果");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QStringList categories;
    categories << "总计";

    QBarCategoryAxis* axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, barSeries);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setChart(chart);

    //画饼图
    QPieSeries* pieSeries = new QPieSeries();
    for(int i = 0; i < gItemList.size(); i++){
        pieSeries->append(gItemList[i].name, gItemList[i].existSet.size());
    }
    pieSeries->setLabelsVisible(true);

    QList<QPieSlice*> slices = pieSeries->slices();
    foreach (QPieSlice* s, slices) {
        if(s->value()){
            s->setLabelVisible(true);
        }else{
            s->setLabelVisible(false);
        }
        s->setLabel(QString("%1 (%2%)").arg(s->label()).arg(QString::number(s->percentage() * 100, 'f', 2)));
    }


    QChart* pieChart = new QChart();
    pieChart->addSeries(pieSeries);
    pieChart->setTitle("统计结果");
    pieChart->legend()->setVisible(true);

    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView_2->setChart(pieChart);

    //更新结果
    ui->label->setText(QString("单词数:%1 总单词数:%2").arg(wordsSet.size()).arg(words.size()));

    ui->textEdit->textCursor().beginEditBlock();
    for(int i = gItemList.size() - 1; i >= 0; i--){
        foreach (QString word, gItemList[i].existSet) {
            QColor color = gItemList[i].color;
            setTextBackgroundColor(ui->textEdit, word, QColor(color.red(), color.green(), color.blue(), 64));
        }
    }
    ui->textEdit->textCursor().endEditBlock();
}

void MainWindow::setTextBackgroundColor(QTextEdit *textEdit, const QString &searchText, const QColor &color)
{
    QTextCursor cursor = textEdit->textCursor();
    cursor.movePosition(QTextCursor::Start); // 或者使用 cursor.setPosition(0);

    // 查找指定的文本
    QTextDocument *document = textEdit->document();
    cursor = document->find(searchText, cursor, QTextDocument::FindWholeWords | QTextDocument::FindCaseSensitively);
    // 设置背景色
    QTextCharFormat format;
    format.setBackground(color);

    // 遍历所有匹配的文本并设置背景色
    while (!cursor.isNull()) {
        cursor.mergeCharFormat(format);  // 合并背景色
        cursor = document->find(searchText, cursor, QTextDocument::FindWholeWords | QTextDocument::FindCaseSensitively);  // 查找下一个匹配的文本
    }
}

void MainWindow::asignAgain(const QString str, QString subStr){

    QString prototype;

    //可数名词变复数
    //friends buses candies tomatoes photos lives knives leaf stops teaches flies goes
    if(subStr == "s"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        asignWord(prototype, str);
    }
    if(subStr == "es"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        asignWord(prototype, str);
    }
    if(subStr == "ies"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        prototype.append('y');
        asignWord(prototype, str);
    }
    if(subStr == "ves"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        prototype.append('f');
        asignWord(prototype, str);
    }
    if(subStr == "ves"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        prototype.append("fe");
        asignWord(prototype, str);
    }
    //实义动词第三人称单数变化规则

    //实意动词现在分词
    //studying working writing moving getting beginning lying dying tying
    if(subStr == "ing"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        asignWord(prototype, str);
    }
    if(subStr == "ing"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        prototype.append('e');
        asignWord(prototype, str);
    }
    if(subStr == "ing"){
        prototype = str.mid(0, str.lastIndexOf(subStr) - 1);
        asignWord(prototype, str);
    }
    if(subStr == "ying"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        prototype.append("ie");
        asignWord(prototype, str);
    }
    //实意动词过去式
    //worked lived stopped studied
    if(subStr == "ed"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        asignWord(prototype, str);
    }
    if(subStr == "ed"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        prototype.append('e');
        asignWord(prototype, str);
    }
    if(subStr == "ed"){
        prototype = str.mid(0, str.lastIndexOf(subStr) - 1);
        asignWord(prototype, str);
    }
    if(subStr == "ied"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        prototype.append('y');
        asignWord(prototype, str);
    }
    //实意动词过去分词

    //形容词副词比较级最高级
    //smaller shorter cleverer larger nicer bigger hotter easier heavier smallest shortest cleverest narrowest largest nicest biggest hottest easiest heaviest
    if(subStr == "er" || subStr == "est"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        asignWord(prototype, str);
    }
    if(subStr == "er" || subStr == "est"){
        prototype = str.mid(0, str.lastIndexOf(subStr) + 1);
        asignWord(prototype, str);
    }
    if(subStr == "er" || subStr == "est"){
        prototype = str.mid(0, str.lastIndexOf(subStr) - 1);
        asignWord(prototype, str);
    }
    if(subStr == "ier" || subStr == "iest"){
        prototype = str.mid(0, str.lastIndexOf(subStr));
        prototype.append('y');
        asignWord(prototype, str);
    }
}

void MainWindow::asignWord(QString prototype, const QString str){
    if(prototype[0].isUpper()){
        prototype = prototype.toLower();
    }
    for(int i = 0; i < gItemList.size(); i++) {
        if(gItemList[i].stdSet.contains(prototype)){
            gItemList[i].existSet.insert(str);
            gItemList.last().existSet.remove(str);
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "保存位置", "");
    if(path.isEmpty()){
        return;
    }
    foreach (Item_t item, gItemList) {
        saveToFile(path, item.name, item.existSet);
    }

    QMessageBox::information(this, "消息", "导出完成!");
}

void MainWindow::saveToFile(QString path, QString fileName, QSet<QString>& set){
    if(set.size()){
        QStringList words = set.toList();
        words.sort();
        QFile file(path + QDir::separator() + fileName.append(".txt"));
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        out.setCodec("UTF-8");
        foreach (QString word, words) {
            out << word << "\n";
        }
        file.close();
    }
}
