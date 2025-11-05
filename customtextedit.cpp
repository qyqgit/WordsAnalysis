#include "customtextedit.h"
#include <QContextMenuEvent>
#include <QMenu>
#include <QMessageBox>
#include <QWidget>
#include <QDebug>
#include <QDesktopServices>

CustomTextEdit::CustomTextEdit(QWidget *parent) :  QTextEdit(parent){

}

void CustomTextEdit::contextMenuEvent(QContextMenuEvent *event){

    // 获取默认菜单
    QMenu *menu = this->createStandardContextMenu();

    QAction *customActionBaidu = menu->addAction("百度搜索");
    QAction *customActionBing = menu->addAction("必应搜索");

    // 添加分隔符和自定义操作
    menu->insertSeparator(menu->actions()[0]);


    menu->insertAction(menu->actions()[0], customActionBaidu);
    menu->insertAction(menu->actions()[1], customActionBing);



    connect(customActionBing, &QAction::triggered, this, [this]() {
        QString selectedText = this->textCursor().selectedText();
        QUrl url("www.bing.com/search?q=" + selectedText);
        QDesktopServices::openUrl(url);

    });
    connect(customActionBaidu, &QAction::triggered, this, [this]() {
        QString selectedText = this->textCursor().selectedText();
        QUrl url("www.baidu.com/s?wd=" + selectedText);
        QDesktopServices::openUrl(url);
    });

    // 执行菜单
    menu->exec(event->globalPos());
}
