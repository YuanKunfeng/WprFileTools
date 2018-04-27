#include "messagedialog.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QImage>


/*******************************************************************************
 * MessageDialog
 *******************************************************************************/
MessageDialog::MessageDialog(const QString &titleInfo, const QString &info, const QPixmap &img,
               DialogButtons buttons, QWidget *parent) :
    ShadowDialog(titleInfo, buttons, parent)
{
    resize(300, 170);
    m_img = img;
    m_info = info;

    // 设置提示图片
    msgLabel = new QLabel();
    msgLabel->setPixmap(m_img);
    msgLabel->setFixedSize(45, 45);
    msgLabel->setScaledContents(true);

    //  设置提示信息，让QLabel能够自动判断并换行显示：
    infoLabel = new QLabel(m_info);
    infoLabel->setWordWrap(true);

    QHBoxLayout *hboxLayout = new QHBoxLayout();
    hboxLayout->addWidget(msgLabel);
    hboxLayout->addWidget(infoLabel);
    hboxLayout->setSpacing(20);
    hboxLayout->setContentsMargins(20, 0, 15, 0);
    mainSpace->setLayout(hboxLayout);
}

int MessageDialog::warning(const QString &info, QWidget *parent)
{
    MessageDialog dlg(tr("警告"), info, QPixmap(":/images/msgWarning"), btnOk, parent);
    return dlg.exec();
}

int MessageDialog::question(const QString &info, QWidget *parent)
{
    MessageDialog dlg(tr("确认"), info, QPixmap(":/images/msgQuestion"), btnOk, parent);
    return dlg.exec();
}

int MessageDialog::infomation(const QString &info, QWidget *parent)
{
    MessageDialog dlg(tr("信息"), info, QPixmap(":/images/msgInfo"), btnOk, parent);
    return dlg.exec();
}

int MessageDialog::error(const QString &info, QWidget *parent)
{
    MessageDialog dlg(tr("错误"), info, QPixmap(":/images/msgError"), btnOk, parent);
    return dlg.exec();
}



