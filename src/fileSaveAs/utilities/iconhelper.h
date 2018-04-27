#ifndef ICONHELPER_H
#define ICONHELPER_H

#include <QObject>
#include <QFont>

class QLabel;
class QPushButton;

/**
 * @brief IconHelper 利用图像字体为按钮和标签设置图标
 */
class IconHelper : public QObject
{
    Q_OBJECT
public:    
    explicit IconHelper(const QString &family, QObject *parent = 0);
    void setIcon(QLabel* label, QChar c, int size);
    void setIcon(QPushButton* button, QChar c, int size);

private:
    QFont m_iconFont;
};

#endif // ICONHELPER_H
