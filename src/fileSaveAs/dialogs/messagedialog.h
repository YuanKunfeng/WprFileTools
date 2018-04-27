#ifndef MESSAGEDIALOG_H
#define MESSAGEDIALOG_H

#include "shadowdialog.h"

class QLabel;

/*******************************************************************************
 * @brief MsgBox  自定义带阴影消息框
 *******************************************************************************/
class MessageDialog : public ShadowDialog
{
    Q_OBJECT
public:
    explicit MessageDialog(const QString &titleInfo, const QString &info, const QPixmap &img,
                    DialogButtons buttons = btnOk, QWidget *parent = 0);

    static int warning(const QString &info, QWidget *parent = 0);
    static int question(const QString &info, QWidget *parent = 0);
    static int infomation(const QString &info, QWidget *parent = 0);
    static int error(const QString &info, QWidget *parent = 0);

protected:
    void createMainSpace();
private:
    QPixmap m_img;
    QString m_info;
    QLabel *msgLabel;                 // 提示图片
    QLabel *infoLabel;                // 提示信息
};

#endif // MESSAGEDIALOG_H
