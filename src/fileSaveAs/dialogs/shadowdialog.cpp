#include "shadowdialog.h"

#include <QLabel>
#include <QPainter>
#include <QImage>
#include <QtMath>
#include <QApplication>
#include "basicdefines.h"
#include "utilities/utilities.h"

/*******************************************************************************
 * ShadowDialog
 *******************************************************************************/
ShadowDialog::ShadowDialog(QWidget *parent) :
    QDialog(parent)
{
    ShadowDialog("", btnOk, parent);
}

ShadowDialog::ShadowDialog(QString titleInfo, DialogButtons buttons, QWidget *parent) :
    QDialog(parent), m_titleColor(colorMidNightBlue)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);

    //初始化为未按下鼠标左键
    m_isMousePressed = false;

    // 创建标题栏
    titleLabel = new QLabel(titleInfo, this);
    titleLabel->setFixedHeight(titleHeight);
    titleLabel->setStyleSheet(QString("QLabel{background: transparent; color: %1;}").arg(colorToString(colorClouds)));

    titleIconLabel = new QLabel(this);
    titleIconLabel->setPixmap(qApp->windowIcon().pixmap(18, 18));
    titleIconLabel->setFixedSize(18, 18);
    titleIconLabel->setScaledContents(true);
    titleIconLabel->setStyleSheet(QString("QLabel{background: transparent;}"));

    closeButton = new ImageButton(tr("关闭"), ":/images/sys_button_close", this);
    connect(closeButton, SIGNAL(clicked()), this, SLOT(onCancel()));

    titleLayout = new QHBoxLayout();
    titleLayout->addWidget(titleIconLabel, 0, Qt::AlignVCenter);
    titleLayout->addWidget(titleLabel, 0, Qt::AlignVCenter);
    titleLayout->addStretch();
    titleLayout->addWidget(closeButton, 0, Qt::AlignTop);
    titleLayout->setSpacing(10);
    titleLayout->setContentsMargins(10, 0, 0, 0);

    //中部空白
    mainSpace = new QWidget(this);
    mainSpace->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainSpace->setStyleSheet(QString("background: transparent;"));

    // 按钮区
    okButton = NULL;
    cancelButton = NULL;
    bottomLayout = new QHBoxLayout();
    bottomLayout->addStretch();
    setButtons(buttons);
    bottomLayout->setSpacing(20);
    bottomLayout->setContentsMargins(0, 0, 10, 10);

    dialogLayout = new QVBoxLayout();
    dialogLayout->addLayout(titleLayout);
    dialogLayout->addWidget(mainSpace);
    dialogLayout->addLayout(bottomLayout);
    dialogLayout->setSpacing(0);
    dialogLayout->setContentsMargins(shadowWidth, shadowWidth, shadowWidth, shadowWidth);
    setLayout(dialogLayout);
}

void ShadowDialog::setButtons(DialogButtons buttons)
{
    if (buttons.testFlag(btnNone))
    {
        if(okButton) okButton->setVisible(false);
        if(cancelButton) cancelButton->setVisible(false);
    }

    if (buttons.testFlag(btnOk))
    {
        if(!okButton)
        {
            okButton = new QPushButton(tr("确定(&O)"), this);
            okButton->setDefault(true);
            okButton->setObjectName("dialogOk");
            connect(okButton, SIGNAL(clicked()), this, SLOT(onOk()));
            bottomLayout->addWidget(okButton);
        }
        okButton->setVisible(true);
    }

    if (buttons.testFlag(btnCancel))
    {
        if(!cancelButton)
        {
            cancelButton = new QPushButton(tr("取消(&C)"), this);
            cancelButton->setObjectName("dialogCancel");
            connect(cancelButton, SIGNAL(clicked()), this, SLOT(onCancel()));
            bottomLayout->addWidget(cancelButton);
        }
        cancelButton->setVisible(true);
    }
}

void ShadowDialog::onOk()
{
    accept();
}

void ShadowDialog::onCancel()
{
    reject();
}

void ShadowDialog::paintEvent(QPaintEvent *event)
{
    //绘制阴影
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawPixmapShadow(painter);
    painter.setPen(Qt::NoPen);
    //窗体背景
    painter.setBrush(colorClouds);
    painter.drawRect(QRect(shadowWidth, titleHeight + shadowWidth, width() - 2 * shadowWidth,
                           height() - titleHeight - 2 * shadowWidth));
    //标题栏背景
    painter.setBrush(m_titleColor);
    painter.drawRect(QRect(shadowWidth, shadowWidth, width() - 2 * shadowWidth, titleHeight));
}

//使用图片绘制阴影
void ShadowDialog::drawPixmapShadow(QPainter &painter)
{
    //4个角
    painter.drawPixmap(QRect(0, 0, 2*shadowWidth, 2*shadowWidth), QPixmap(":/images/widget_shadow_lefttop"));
    painter.drawPixmap(QRect(0, height()-2*shadowWidth, 2*shadowWidth, 2*shadowWidth), QPixmap(":/images/widget_shadow_leftbottom"));
    painter.drawPixmap(QRect(width()-2*shadowWidth, 0, 2*shadowWidth, 2*shadowWidth), QPixmap(":/images/widget_shadow_righttop"));
    painter.drawPixmap(QRect(width()-2*shadowWidth, height()-2*shadowWidth, 2*shadowWidth, 2*shadowWidth), QPixmap(":/images/widget_shadow_rightbottom"));
    //4条边
    painter.drawPixmap(QRect(0, 2*shadowWidth, shadowWidth, height()-4*shadowWidth), QPixmap(":/images/widget_shadow_left"));
    painter.drawPixmap(QRect(width()-shadowWidth, 2*shadowWidth, shadowWidth, height()-4*shadowWidth), QPixmap(":/images/widget_shadow_right"));
    painter.drawPixmap(QRect(2*shadowWidth, 0, width() - 4*shadowWidth, shadowWidth), QPixmap(":/images/widget_shadow_top"));
    painter.drawPixmap(QRect(2*shadowWidth, height()-shadowWidth, width()-4*shadowWidth, shadowWidth), QPixmap(":/images/widget_shadow_bottom"));
}

void ShadowDialog::mousePressEvent(QMouseEvent *event)
{
    // 必须是在标题栏
    if(event->pos().y() > shadowWidth+titleHeight)
        return;

    // 只能是鼠标左键移动和改变大小
    if(event->button() == Qt::LeftButton)
    {
        m_isMousePressed = true;
    }

    // 窗口移动距离
    m_moveStartPos = event->globalPos() - pos();
}

void ShadowDialog::mouseReleaseEvent(QMouseEvent *)
{
    m_isMousePressed = false;
}

void ShadowDialog::mouseMoveEvent(QMouseEvent *event)
{
    //移动窗口
    if(m_isMousePressed)
    {
        QPoint l_MovePos = event->globalPos();
        move(l_MovePos - m_moveStartPos);
    }
}

