/**
 * @file logmanager.h
 * @brief 文本日志
 * @author Yuan Kunfeng
 * @version 2016.03.09
 *
 * #修改记录
 * [2016-03-09] (YKF) 初始版本
 */
#ifndef LogManager_H
#define LogManager_H

#include<QStringList>
#include<QObject>
#include<QMap>
#include<QThread>
#include<QMutex>

#define MOD_MAIN "main"
const int LOGFILE_MAX_SIZE = 1024 * 1024;       //文件大小最大1MB


enum LogLevel { lgTrace, lgDebug, lgInfo, lgWarn, lgError, lgFatal ,lgUnkown};

void logTrace(const QString &module, const QString &message);
void logDebug(const QString &module, const QString &message);
void logInfo(const QString &module, const QString &message);
void logWarning(const QString &module, const QString &message);
void logError(const QString &module, const QString &message);
void logFatal(const QString &module, const QString &message);
void logByLevel(const QString &module, LogLevel level, const QString &message);

/**
 * @brief The Logger class 日志读写，复制把日志信息写入\读出文件
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    explicit Logger(const QString &fileDestination, LogLevel level,QObject *parent=0);
    explicit Logger(LogLevel level,QObject *parent=0);
    LogLevel getLevel() const { return m_level; }

    bool write(const QString &module, const QString &message);
    bool write(const QString &i_module,const LogLevel i_level, const QString &i_message);
    bool read(QString &o_message);
    bool read(QStringList &o_message);
private:
    QString m_fileDestination;
    LogLevel m_level;
};

/**
 * @brief The LogManager class 日志管理。负责组织多个日志模块和目标日志文件
 */
class LogManager : public QThread
{
public:
    static LogManager &instance();
    static QString levelToText(const LogLevel &level);

    bool addLogInstance(const QString &fileDest, const QStringList &modules, LogLevel level);
    bool addLogInstance(const QString &fileDest, const QString &module, LogLevel level);
    bool addLogInstance(const QString &module,LogLevel level);

    Logger *getLogInstance(const QString &module);
    void closeLogger();
    QMutex mutex;

private:
    static LogManager *m_instance;
    QMap<QString,Logger*> m_moduleDests;
    LogManager();
};

#endif // LogManager_H
