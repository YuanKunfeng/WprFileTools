/**
 * @file configdepository.h
 * @brief 参数设置库
 * @author Yuan Kunfeng
 * @version 2015.08.18
 *
 * #修改记录
 * [2015-07-14] (YKF, 1.0) 初始版本。
 * [2015-08-10] (YKF, 1.1) 将全局变量修改为singleton模式。
 * [2015-08-18] (YKF, 1.2) 全部重写。使用QVariant列表方式，以便于自由增加设置参数。
 */

#ifndef CONFIGDEPOSITORY_H
#define CONFIGDEPOSITORY_H

#include <QObject>
#include <QMap>
#include <QVariant>

class ConfigDepository : public QObject
{
    Q_OBJECT
public:
    static ConfigDepository &instance();
    static void destroyInstance();

    bool setValue(const QString &key, const QVariant &value);
    QVariant getValue(const QString &key, const QVariant &defaultValue = QVariant()) const;
    bool contains(const QString &key) const;
    QStringList allKeys() const;

signals:
    void ConfigChanged(const QString &key);

private:
    QMap<QString, QVariant> m_values;
    QString m_fileDestination;
    bool m_isLoaded;

    explicit ConfigDepository(QObject *parent = 0);
    explicit ConfigDepository(const QString &fileDestination,QObject * parent=0);
    bool save();
    bool load();
};

/*****************************************************************************
 * 调试
 *****************************************************************************/
void setDebugConfigration();

#endif // CONFIGDEPOSITORY_H
