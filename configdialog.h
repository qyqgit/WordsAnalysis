#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QtCharts/QtCharts>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

    void loadData(const QString path, QSet<QString>& set);
    void updateConfigType(quint8 type);
    void updateChart();
    void updateLoadingData();
    void updateTable();
    void enableTable(bool on);
private slots:
    void on_radioButton_1_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_pushButtonAdd_clicked();

    void on_pushButtonConfirm_clicked();

    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_tableWidget_cellPressed(int row, int column);

    void on_pushButtonReload_clicked();

private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIGDIALOG_H
