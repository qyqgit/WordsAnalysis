#include "configdialog.h"
#include "ui_configdialog.h"
#include "common.h"

#include <QFile>
#include <QJsonObject>
#include <QJsonArray>

ConfigDialog::ConfigDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ConfigDialog)
{
    ui->setupUi(this);
    qDebug() << "ConfigDialog init.";
    updateLoadingData();
    updateChart();

    QStringList labels;
    ui->tableWidget->setColumnCount(4);
    labels << "名称" << "路径" << "颜色" << "删除";
    ui->tableWidget->setHorizontalHeaderLabels(labels);
    updateTable();

    if(ui->radioButton_3->isChecked()){
        enableTable(true);
    }else{
        enableTable(false);
    }
}

ConfigDialog::~ConfigDialog()
{
    delete ui;
}
void ConfigDialog::updateLoadingData(){
    QJsonParseError jsonError;
    gItemList.clear();
    gAllSet.clear();
    QFile file("config.json");
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    file.close();
    gJsonDoc = QJsonDocument::fromJson(data,&jsonError);
    if(gJsonDoc.isNull()){
        return;
    }

    QJsonObject configObj = gJsonDoc.object();
    quint8 configType = configObj.value("ConfigType").toInt();
    QJsonArray itemArray;
    if(configType == OXFORD_CONFIG_TYPE){
        ui->radioButton_1->setChecked(true);
        itemArray = configObj.value("Items0").toArray();
    }else if(configType == CAMBRIDGE_CONFIG_TYPE){
        ui->radioButton_2->setChecked(true);
        itemArray = configObj.value("Items1").toArray();
    }else if(configType == CUSTOM_CONFIG_TYPE){
        ui->radioButton_3->setChecked(true);
        itemArray = configObj.value("Items2").toArray();
    }
    for(int i = 0; i < itemArray.size(); i++){
        Item_t item;
        QString colorStr = itemArray[i].toObject().value("Color").toString();
        item.color = QColor(colorStr);
        item.name = itemArray[i].toObject().value("Name").toString();
        item.filePath = itemArray[i].toObject().value("FilePath").toString();
        loadData(item.filePath, item.stdSet);
        gItemList.append(item);
    }
    foreach (Item_t item, gItemList) {
        gAllSet += item.stdSet;
    }

    qDebug() << "Total:" << gAllSet.size();
}
void ConfigDialog::loadData(const QString path, QSet<QString>& set){
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

void ConfigDialog::updateConfigType(quint8 type){
    QJsonObject jsonObj = gJsonDoc.object();
    jsonObj["ConfigType"] = type;
    gJsonDoc.setObject(jsonObj);

    QFile file("config.json");
    file.open(QIODevice::WriteOnly);
    file.write(gJsonDoc.toJson(QJsonDocument::Indented));
    file.close();
}

void ConfigDialog::updateChart(){
    QHorizontalPercentBarSeries* barSeries = new QHorizontalPercentBarSeries(this);
    barSeries->setLabelsVisible(true);
    barSeries->setBarWidth(0.2);
    for(int i = 0; i < gItemList.size(); i++){
        QBarSet* barSet = new QBarSet(QString("%1(%2)").arg(gItemList[i].name).arg(gItemList[i].stdSet.size()), this);
        barSet->setColor(gItemList[i].color);
        *barSet << gItemList[i].stdSet.size();
        barSeries->append(barSet);
    }

    QChart* chart = new QChart();
    chart->addSeries(barSeries);
    chart->setTitle("统计结果");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    QStringList categories;
    categories << QString("百分比(%1)").arg(gAllSet.size());

    QBarCategoryAxis* axisY = new QBarCategoryAxis(this);
    axisY->append(categories);
    chart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);

    QValueAxis* axisX = new QValueAxis(this);
    chart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->setChart(chart);
}

void ConfigDialog::updateTable(){

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    QJsonParseError jsonError;
    QFile file("config.json");
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    file.close();
    gJsonDoc = QJsonDocument::fromJson(data,&jsonError);
    if(gJsonDoc.isNull()){
        return;
    }

    QJsonObject configObj = gJsonDoc.object();
    QJsonArray itemArray = configObj.value("Items2").toArray();
    for(int i = 0; i < itemArray.size() - 1; i++){
        Item_t item;
        QFrame* frame = new QFrame(this);
        QString colorStr = itemArray[i].toObject().value("Color").toString();
        item.name = itemArray[i].toObject().value("Name").toString();
        item.filePath = itemArray[i].toObject().value("FilePath").toString();
        frame->setStyleSheet(QString("background-color: %1;").arg(colorStr));
        QPushButton* btnDel = new QPushButton("删除", this);
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(item.name));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(item.filePath));
        ui->tableWidget->setCellWidget(i, 2, frame);
        ui->tableWidget->setCellWidget(i, 3, btnDel);
    }
    ui->tableWidget->resizeColumnsToContents();
}

void ConfigDialog::enableTable(bool on){
    if(on){
        ui->pushButtonAdd->setEnabled(true);
        ui->pushButtonConfirm->setEnabled(true);
        ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::CurrentChanged);
        ui->tableWidget->blockSignals(false);
    }else{
        ui->pushButtonAdd->setEnabled(false);
        ui->pushButtonConfirm->setEnabled(false);
        ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableWidget->blockSignals(true);
    }
}

void ConfigDialog::on_radioButton_1_clicked()
{
    updateConfigType(OXFORD_CONFIG_TYPE);
    updateLoadingData();
    updateChart();

    enableTable(false);
}

void ConfigDialog::on_radioButton_2_clicked()
{
    updateConfigType(CAMBRIDGE_CONFIG_TYPE);
    updateLoadingData();
    updateChart();

    enableTable(false);
}


void ConfigDialog::on_radioButton_3_clicked()
{
    updateConfigType(CUSTOM_CONFIG_TYPE);
    updateLoadingData();
    updateChart();
    updateTable();

    enableTable(true);
}


void ConfigDialog::on_pushButtonAdd_clicked()
{
    int randomNumber = QRandomGenerator::global()->bounded(0, gItemList.size()); // 或者使用bounded(0, 256)
    quint32 row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    QPushButton* btnDel = new QPushButton("删除", this);
    QFrame* frame = new QFrame(this);
    frame->setStyleSheet(QString(" background-color: %1;").arg(gItemList[randomNumber].color.name(QColor::NameFormat::HexArgb)));
    ui->tableWidget->setCellWidget(row, 3, btnDel);
    ui->tableWidget->setCellWidget(row, 2, frame);
}


void ConfigDialog::on_pushButtonConfirm_clicked()
{
    QJsonObject jsonObj = gJsonDoc.object();
    QJsonArray items2;

    for(int i = 0; i < ui->tableWidget->rowCount(); i++){
        if(!ui->tableWidget->item(i, 0) || !ui->tableWidget->item(i, 1)) continue;
        QString name = ui->tableWidget->item(i, 0)->text();
        QString path = ui->tableWidget->item(i, 1)->text();
        QString color = ui->tableWidget->cellWidget(i, 2)->styleSheet();
        color = color.mid(color.indexOf(':') + 2, 9);
        QJsonObject item;
        item["Color"] = color;
        item["FilePath"] = path;
        item["Name"] = name;
        items2.append(item);
    }
    QJsonObject item;
    item["Color"] = QColor(Qt::red).name();
    item["FilePath"] = "";
    item["Name"] = "Other";
    items2.append(item);

    jsonObj["Items2"] = items2;
    gJsonDoc.setObject(jsonObj);

    QFile file("config.json");
    file.open(QIODevice::WriteOnly);
    file.write(gJsonDoc.toJson(QJsonDocument::Indented));
    file.close();

    ui->radioButton_3->click();
}

void ConfigDialog::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if(currentColumn == 3){
        ui->tableWidget->removeRow(currentRow);
    }
}



void ConfigDialog::on_tableWidget_cellPressed(int row, int column)
{
    if(column == 1){
        QString path = QFileDialog::getOpenFileName(this, "选择文件", "", "Text files (*.txt);; All files (*.*)");
        if(path.isEmpty()) return;
        ui->tableWidget->item(row, column)->setText(path);
        ui->tableWidget->resizeColumnsToContents();
    }else if(column == 2){
        QColor color = QColorDialog::getColor(Qt::white, this, "选择颜色");
        if(color.isValid()){
            QFrame* frame = qobject_cast<QFrame*>(ui->tableWidget->cellWidget(row, 2));
            frame->setStyleSheet(QString("background-color: %1;").arg(color.name(QColor::NameFormat::HexArgb)));
        }
    }
}

void ConfigDialog::on_pushButtonReload_clicked()
{
    updateTable();
}

