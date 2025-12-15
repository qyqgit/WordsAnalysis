#ifndef FAVORITEDIALOG_H
#define FAVORITEDIALOG_H

#include <QDialog>

namespace Ui {
class FavoriteDialog;
}

class FavoriteDialog : public QDialog
{
    Q_OBJECT

signals:
    void refreshTable();

public:
    explicit FavoriteDialog(QWidget *parent = nullptr);
    ~FavoriteDialog();

    void updateTable();
public slots:
    void updateLangue();
protected:
    // 重写 showEvent 函数
    void showEvent(QShowEvent *event) override;
private slots:
    void on_tableWidget_cellClicked(int row, int column);

    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

    void on_pushButton_clicked();

private:
    Ui::FavoriteDialog *ui;
};

#endif // FAVORITEDIALOG_H
