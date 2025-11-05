#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QRegularExpression>
#include <QFile>
#include <QBarSet>
#include <QTextFormat>
#include <QUrl>
#include <QDesktopServices>

void MainWindow::loadData(QString path, QSet<QString>& set){
    QFile file(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream in(&file);
    in.setCodec("UTF-8");
    while(!in.atEnd()){
        QString line = in.readLine();
        QString word = line.mid(0, line.indexOf(' '));
        if(set.contains(word)){
            qDebug() << "Contain:" << word;
        }else{
            set.insert(word);
        }
    }
    file.close();
    qDebug() << path << set.size();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    loadData(".\\data\\A1.txt", a1Set);
    loadData(".\\data\\A2.txt", a2Set);
    loadData(".\\data\\B1.txt", b1Set);
    loadData(".\\data\\B2.txt", b2Set);
    loadData(".\\data\\B2+.txt", b2pSet);
    loadData(".\\data\\C1.txt", c1Set);

    allSet = a1Set + a2Set + b1Set + b2Set + b2pSet + c1Set;
    qDebug() << "Total:" << allSet.size();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
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

    a1 = (wordsSet & a1Set);
    a2 = (wordsSet & a2Set);
    b1 = (wordsSet & b1Set);
    b2 = (wordsSet & b2Set);
    b2p = (wordsSet & b2pSet);
    c1 = (wordsSet & c1Set);
    other = (wordsSet - allSet);

    foreach (QString str, other) {
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

    qDebug() << "A1:" << a1 << a1.size();
    qDebug() << "A2:" << a2 << a2.size();
    qDebug() << "B1:" << b1 << b1.size();
    qDebug() << "B2:" << b2 << b2.size();
    qDebug() << "B2+:" << b2p << b2p.size();
    qDebug() << "C1:" << c1 << c1.size();
    qDebug() << "\r\nOther:" << other << other.size();

    //画柱状图
    QBarSet* levelA1 = new QBarSet("Level A1");
    QBarSet* levelA2 = new QBarSet("Level A2");
    QBarSet* levelB1 = new QBarSet("Level B1");
    QBarSet* levelB2 = new QBarSet("Level B2");
    QBarSet* levelB2P = new QBarSet("Level B2+");
    QBarSet* levelC1 = new QBarSet("Level C1");
    QBarSet* levelOther = new QBarSet("Other");

    levelC1->setColor(Qt::yellow);
    levelOther->setColor(Qt::red);

    *levelA1 << a1.size() << 0 << 0 << 0 << 0 << 0 << 0;
    *levelA2 << 0 << a2.size()  << 0 << 0 << 0 << 0 << 0;
    *levelB1 << 0 << 0  << b1.size()<< 0 << 0 << 0 << 0;
    *levelB2 << 0 << 0 << 0  << b2.size()<< 0 << 0 << 0;
    *levelB2P << 0 << 0 << 0 << 0  << b2p.size()<< 0 << 0;
    *levelC1 << 0 << 0 << 0 << 0 << 0  << c1.size()<< 0;
    *levelOther << 0 << 0 << 0 << 0 << 0 << 0 << other.size();

    QBarSeries* series = new QBarSeries();
    series->append(levelA1);
    series->append(levelA2);
    series->append(levelB1);
    series->append(levelB2);
    series->append(levelB2P);
    series->append(levelC1);
    series->append(levelOther);
    series->setLabelsVisible(true);
    series->setBarWidth(1);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("统计结果");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QStringList categories;
    categories << "Level A1" << "Level A2" << "Level B1" << "Level B2" << "Level B2+" << "Level C1" << "Other";
    QBarCategoryAxis* axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setChart(chart);

    //画饼图
    QPieSeries* pieSeries = new QPieSeries();
    pieSeries->append("Level A1", a1.size());
    pieSeries->append("Level A2", a2.size());
    pieSeries->append("Level B1", b1.size());
    pieSeries->append("Level B2", b2.size());
    pieSeries->append("Level B2+", b2p.size());
    pieSeries->append("Level C1", c1.size());
    pieSeries->append("Other", other.size());
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
    foreach (QString key, other) {
        QColor color = levelOther->color();
        setTextBackgroundColor(ui->textEdit, key, QColor(color.red(), color.green(), color.blue(), 64));
    }
    foreach (QString key, c1) {
        QColor color = levelC1->color();
        setTextBackgroundColor(ui->textEdit, key, QColor(color.red(), color.green(), color.blue(), 64));
    }
    foreach (QString key, b2p) {
        QColor color = levelB2P->color();
        setTextBackgroundColor(ui->textEdit, key, QColor(color.red(), color.green(), color.blue(), 64));
    }
    foreach (QString key, b2) {
        QColor color = levelB2->color();
        setTextBackgroundColor(ui->textEdit, key, QColor(color.red(), color.green(), color.blue(), 64));
    }
    foreach (QString key, b1) {
        QColor color = levelB1->color();
        setTextBackgroundColor(ui->textEdit, key, QColor(color.red(), color.green(), color.blue(), 64));
    }
    foreach (QString key, a2) {
        QColor color = levelA2->color();
        setTextBackgroundColor(ui->textEdit, key, QColor(color.red(), color.green(), color.blue(), 64));
    }
    foreach (QString key, a1) {
        QColor color = levelA1->color();
        setTextBackgroundColor(ui->textEdit, key, QColor(color.red(), color.green(), color.blue(), 64));
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

    if(a1Set.contains(prototype)){
        a1.insert(str);
        other.remove(str);
    }
    if(a2Set.contains(prototype)){
        a2.insert(str);
        other.remove(str);
    }
    if(b1Set.contains(prototype)){
        b1.insert(str);
        other.remove(str);
    }
    if(b2Set.contains(prototype)){
        b2.insert(str);
        other.remove(str);
    }
    if(b2pSet.contains(prototype)){
        b2p.insert(str);
        other.remove(str);
    }
    if(c1Set.contains(prototype)){
        c1.insert(str);
        other.remove(str);
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QString path = QFileDialog::getExistingDirectory(this, "保存位置", "");
    if(path.isEmpty()){
        return;
    }

    saveToFile(path, "Level A1.txt", a1);
    saveToFile(path, "Level A2.txt", a2);
    saveToFile(path, "Level B1.txt", b1);
    saveToFile(path, "Level B2.txt", b2);
    saveToFile(path, "Level B2+.txt", b2p);
    saveToFile(path, "Level C1.txt", c1);
    saveToFile(path, "Other.txt", other);

    QMessageBox::information(this, "消息", "导出完成!");
}

void MainWindow::saveToFile(QString path, QString fileName, QSet<QString>& set){
    if(set.size()){
        QStringList words = set.toList();
        words.sort();
        QFile file(path + QDir::separator() + fileName);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream out(&file);
        foreach (QString word, words) {
            out << word << "\n";
        }
        file.close();
    }
}
