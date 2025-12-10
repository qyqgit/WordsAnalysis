#include "favoritedialog.h"
#include "ui_favoritedialog.h"
#include "common.h"

#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QPushButton>


FavoriteDialog::FavoriteDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FavoriteDialog)
{
    ui->setupUi(this);

    QStringList labels;
    ui->tableWidget->setColumnCount(2);
    labels << "名称" << "删除";
    ui->tableWidget->setHorizontalHeaderLabels(labels);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

FavoriteDialog::~FavoriteDialog()
{
    delete ui;
}

void FavoriteDialog::updateTable(){
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    QJsonDocument doc = getJsonDoc();
    if(doc.isNull()){
        return;
    }
    QJsonObject configObj = doc.object();
    QStringList keys = configObj.keys();
    QStringList favorites;
    foreach (QString key, keys) {
        if(key.startsWith("Favorite", Qt::CaseInsensitive)){
            favorites.append(key);
        }
    }
    for(int i = 0; i < favorites.size(); i++) {
        QPushButton* btnDel = new QPushButton("删除", this);
        ui->tableWidget->insertRow(i);
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(favorites[i]));
        ui->tableWidget->setCellWidget(i, 1, btnDel);
    }
    ui->tableWidget->resizeColumnsToContents();
}

void FavoriteDialog::showEvent(QShowEvent *e){

    updateTable();

}

void FavoriteDialog::on_tableWidget_cellClicked(int row, int column)
{
    if(column == 0){
        QString name = ui->tableWidget->item(row, column)->text();

        QJsonDocument doc = getJsonDoc();
        QJsonObject jsonObj = doc.object();
        QJsonArray favorite = jsonObj.value(name).toArray();

        jsonObj["Items2"] = favorite;
        doc.setObject(jsonObj);

        setJsonDoc(doc);

        emit refreshTable();
    }
}


void FavoriteDialog::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if(currentColumn == 1){
        QString name = ui->tableWidget->item(currentRow, 0)->text();

        QJsonDocument doc = getJsonDoc();
        QJsonObject jsonObj = doc.object();

        jsonObj.remove(name);
        doc.setObject(jsonObj);

        setJsonDoc(doc);

        updateTable();
    }
}


void FavoriteDialog::on_pushButton_clicked()
{
    updateTable();
}

