#ifndef CUSTOMCTRLS_H
#define CUSTOMCTRLS_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QPixmap>
#include <QSignalMapper>
#include <QMap>

#include "basicdefines.h"

/*******************************************************************************
 * @brief ImageButton
 *        图像按钮
 * @note 每个按钮图片必须由大小相等的4个小图片组成，分别为正常状态、鼠标进入、按下、无效
 *******************************************************************************/
class ImageButton : public QPushButton
{
    Q_OBJECT
public:
    enum ButtonStatus {NORMAL, ENTER, PRESS, DISABLE};

    explicit ImageButton(QWidget *parent = 0);
    explicit ImageButton(const QString &text, const QString &image, QWidget *parent = 0);
    void setImageName(const QString &image);
    void setButtonStatus(ButtonStatus status);
protected:
    void changeEvent(QEvent *event);
    void checkStateSet();
    void enterEvent(QEvent *);
    void leaveEvent(QEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);

private:
    int m_imgIndex;
    QList<QPixmap> m_imgList;
};

/*******************************************************************************
 * @brief ImageTabBar
 *        使用图片绘制自定义的标签页
 * @note 每个图片必须由大小相等的4个小图片组成，分别为正常状态、鼠标进入、按下、无效
 *******************************************************************************/
class ImageTabBar : public QWidget
{
    Q_OBJECT
public:
    explicit ImageTabBar(QWidget* parent=0);
    void addImageTab(const QString &text, const QString &image, int id = -1);
    void setSpacing(int spacing, int startSpace = 20);
    void setActiveTab(int id);
signals:
    void tabClicked(int id);

protected:
    void paintEvent(QPaintEvent *);

private:
    QSignalMapper *m_mapper;
    QList< QPair<int, ImageButton *> > m_buttons;  //id, tab
    int m_spacing;
    int m_startSpace;
    int m_buttonIdx;

private slots:
    void onButtonClicked(int id);
};

/*******************************************************************************
 * @brief ValueLabel
 *        含一个图标的Label。包括一个图标，一个文本描述，一个值
 *******************************************************************************/
class ValueLabel : public QWidget
{
    Q_OBJECT
public:
    explicit ValueLabel(QString text, QString iconName = "", QString value = "", QWidget *parent = 0);

    QString text() const {return m_text; }
    void setText(const QString &text) { m_text = text;  updateUI();}
    void setTextWidth(int w) {m_textLabel->setFixedWidth(w);}
    QString iconName() const {return m_iconName; }
    void setIconName(const QString &iconName) { m_iconName = iconName; }
    QString value() const {return m_value; }
    void setValue(const QString &value) { m_value = value; updateUI();}

    void updateUI();
private:
    QString m_iconName;
    QString m_text;
    QString m_value;
    QLabel *m_iconLabel;
    QLabel *m_textLabel;
    QLabel *m_valueLabel;
};

/*******************************************************************************
 * @brief TraceableWidget
 *        随着鼠标移动，显示焦点框的Widget
 *******************************************************************************/
class TraceableWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TraceableWidget(QWidget *parent = 0) :
         QWidget(parent), m_isMouseEnter(false)
    {}

signals:
    void clicked();

private:
    bool m_isMouseEnter;

    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *);
};

#endif // CUSTOMCTRLS_H
