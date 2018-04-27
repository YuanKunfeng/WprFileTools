/**
 * @file shadowdialog.h
 * @brief 带阴影对话框
 * @author Yuan Kunfeng
 * @version 2015.09.10
 *
 * #修改记录
 * [2015-04-14] (YKF) 初始版本
 * [2015-05-06] (YKF) 重新实现，从继承自QWidget改为继承自QDialog，增加按钮定义等。类改名DropShadowWidget->DropShadowDialog
 * [2015-09-10] (YKF) 更改外观和配色。类改名DropShadowDialog->ShadowDialog
 */
#ifndef SHADOWDIALOG_H
#define SHADOWDIALOG_H

#include <QDialog>
#include <QMouseEvent>
#include <QVBoxLayout>
#include "basicdefines.h"
#include "widgets/customctrls.h"

class QLabel;

const int shadowWidth = 18;
const int titleHeight = 26;

/*******************************************************************************
 * @brief ShadowDialog 带阴影的对话框基类
 *******************************************************************************/
class ShadowDialog : public QDialog
{
    Q_OBJECT
    //自定义属性，用于样式表
    Q_PROPERTY(QColor titleColor MEMBER m_titleColor SCRIPTABLE true)

public:
    enum DialogButton { btnNone, btnOk, btnCancel };
    Q_DECLARE_FLAGS(DialogButtons, DialogButton)

    explicit ShadowDialog(QWidget *parent = 0);
    explicit ShadowDialog(QString titleInfo, DialogButtons buttons = btnOk, QWidget *parent = 0);
    void setButtons(DialogButtons buttons);

public slots:
    virtual void onOk();
    virtual void onCancel();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);

    void drawPixmapShadow(QPainter &painter);
protected:
    //界面组件
    QLabel *titleLabel;
    QLabel *titleIconLabel;
    ImageButton *closeButton;       // 提示框上的关闭按钮
    QPushButton *okButton;          // 确定按钮
    QPushButton *cancelButton;      // 取消按钮
    QWidget *mainSpace;             //对话框内容区
    QHBoxLayout *titleLayout;
    QHBoxLayout *bottomLayout;
    QVBoxLayout *dialogLayout;      //对话框的布局

    //鼠标
    QPoint m_moveStartPos;       // 移动开始时鼠标位置
    bool m_isMousePressed;      // 按下鼠标左键

    //Style
    QColor m_titleColor;

};

#endif // SHADOWDIALOG_H
