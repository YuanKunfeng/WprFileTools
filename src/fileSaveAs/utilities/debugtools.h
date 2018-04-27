/**
 * @file debugtools.h
 * @brief 开发过程中的调试工具
 * @author Yuan Kunfeng
 * @version 2016.03.04
 *
 * #修改记录
 * [2016.03.04] (YKF) 初始版本, CLifeCircleLogger类
 *
 */

#ifndef DEBUGTOOLS_H
#define DEBUGTOOLS_H

#include <QDebug>
#include <QString>

/************************************************************************************
 * @brief CLifeCircleLogger
 *        用于在输出台输出调试信息，查看函数使用的生命周期.
 *
 * @note 从原来的CLifeCircleLogger.h复制。作者DWX
 */
class LifeCircleLogger
{
    QString m_msg;
public:
    LifeCircleLogger() {}
    LifeCircleLogger(const QString &msg): m_msg(msg)
    {
        qDebug() << msg << "BEGIN!";
    }

    ~LifeCircleLogger() { qDebug()<< m_msg << "END!"; }
};

#define LOG_FUNCTION_LIFE LifeCircleLogger(__FUNCTION__);

#endif // DEBUGTOOLS_H
