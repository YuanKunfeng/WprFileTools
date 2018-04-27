#include "logmanager.h"
#include <QDir>
#include <QDateTime>
#include <QTextStream>

/*******************************************************************************
 * 方便函数
 *******************************************************************************/
void logTrace(const QString &module, const QString &message)
{
    logByLevel(module, lgTrace, message);
}

void logDebug(const QString &module, const QString &message)
{
    logByLevel(module, lgDebug, message);
}

void logInfo(const QString &module, const QString &message)
{
    logByLevel(module, lgInfo, message);
}

void logWarning(const QString &module, const QString &message)
{
    logByLevel(module, lgWarn, message);
}

void logError(const QString &module, const QString &message)
{
    logByLevel(module, lgError, message);
}

void logFatal(const QString &module, const QString &message)
{
    logByLevel(module, lgFatal, message);
}

void logByLevel(const QString &module, LogLevel level, const QString &message)
{
    LogManager &manager = LogManager::instance();

    QMutexLocker(&(manager.mutex));

    Logger *logWriter = manager.getLogInstance(module);

    if (logWriter && logWriter->getLevel() <= level)
            logWriter->write(module, level, message);
}

/*******************************************************************************
 * LogManager
 *******************************************************************************/
LogManager * LogManager::m_instance = NULL;

LogManager::LogManager() : QThread(), mutex(QMutex::Recursive)
{
    start();
}

LogManager &LogManager::instance()
{
    if (!m_instance)
        m_instance = new LogManager();

    return *m_instance;
}

QString LogManager::levelToText(const LogLevel &level)
{
    switch (level)
    {
        case lgTrace: return "Trace";
        case lgDebug: return "Debug";
        case lgInfo: return "Info";
        case lgWarn: return "Warning";
        case lgError: return "Error";
        case lgFatal: return "Fatal";
        default: return QString();
    }
}

bool LogManager::addLogInstance(const QString &fileDest, const QString &module, LogLevel level)
{
    Logger *log;

    if (!m_moduleDests.contains(module))
    {
        log = new Logger(fileDest,level);
        m_moduleDests.insert(module, log);
        return true;
    }

    return false;
}

bool LogManager::addLogInstance(const QString &fileDest, const QStringList &modules, LogLevel level)
{
    bool added = false;
    foreach (QString module, modules)
    {
        if (!m_moduleDests.contains(module))
        {
            Logger *log = new Logger(fileDest,level);
            m_moduleDests.insert(module, log);
            added = true;
        }
    }
    return added;
}

bool LogManager::addLogInstance(const QString &module,LogLevel level)
{
    Logger *log;
    if(!m_moduleDests.contains(module))
    {
        QDir dir(QDir::currentPath());
        if (!dir.exists("logs"))
            dir.mkdir("logs");

        QString fileDestination = QDir::currentPath() + "/logs/"+ module +".log";
        log = new Logger(fileDestination,level);
        m_moduleDests.insert(module,log);
        return true;
    }
    return false;
}

Logger *LogManager::getLogInstance(const QString &module)
{
    return m_moduleDests.value(module);
}

void LogManager::closeLogger()
{
    exit(0);
    deleteLater();
}

/*******************************************************************************
 * Logger
 *******************************************************************************/
Logger::Logger(const QString &fileDestination, LogLevel level,QObject *parent)
    :QObject(parent)
{
    m_fileDestination = fileDestination;
    m_level = level;
}

Logger::Logger(LogLevel level,QObject *parent) :QObject(parent)
{
     m_level = level;
    QDir dir(QDir::currentPath());
    if (!dir.exists("logs"))
        dir.mkdir("logs");

    m_fileDestination = QDir::currentPath() + "/logs/system.log";

}

bool Logger::write(const QString &module,  const QString &message)
{
    QString fileName = m_fileDestination;
    QFile file(fileName);
    QString toRemove = fileName.section('.',-1);
    QString fileNameAux = fileName.left(fileName.size() - toRemove.size()-1);
    bool renamed = false;
    QString newName = fileNameAux + "_%1__%2.log";

    if (file.size() >= LOGFILE_MAX_SIZE)
    {
        QDateTime currentTime = QDateTime::currentDateTime();
        newName = newName.arg(currentTime.date().toString("yy_MM_dd")).arg(currentTime.time().toString("hh_mm_ss"));
        renamed = file.rename(fileName, newName);

    }

    file.setFileName(fileName);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
    {
        QTextStream out(&file);
        QString dtFormat = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

        if (renamed)
            out << QString("%1 - Previuous log %2\n").arg(dtFormat).arg(newName);

        QString logLevel = LogManager::levelToText(m_level);
        QString text = QString("[%1] [%2] {%3} %4\n").arg(dtFormat).arg(logLevel).arg(module).arg(message);
        out << text;
        file.close();
        return true;
    }
    else
    {
        //Log失败
        return false;
    }
}

bool Logger::write(const QString &i_module, const LogLevel i_level, const QString &i_message)
{
    QString fileName = m_fileDestination;
    QFile file(fileName);
    QString toRemove = fileName.section('.',-1);
    QString fileNameAux = fileName.left(fileName.size() - toRemove.size()-1);
    bool renamed = false;
    QString newName = fileNameAux + "_%1__%2.log";

    if (file.size() >= LOGFILE_MAX_SIZE)
    {
        QDateTime currentTime = QDateTime::currentDateTime();
        newName = newName.arg(currentTime.date().toString("yy_MM_dd")).arg(currentTime.time().toString("hh_mm_ss"));
        renamed = file.rename(fileName, newName);
    }

    file.setFileName(fileName);
    if (file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
    {
        QTextStream out(&file);
        QString dtFormat = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");

        if (renamed)
            out << QString("%1 - Previuous log %2\n").arg(dtFormat).arg(newName);

        QString logLevel = LogManager::levelToText(i_level);
        QString text = QString("[%1] [%2] {%3} %4\n").arg(dtFormat).arg(logLevel).arg(i_module).arg(i_message);
        out << text;
        file.close();
        return true;
    }
    else
    {
        return false;
    }
}

bool Logger::read(QString &o_message)
{
    o_message="";
    QString _fileName = m_fileDestination;
    QFile file(_fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
       while(!in.atEnd()){
           o_message += in.readLine() + "\n";
       }

        file.close();
        return true;
    }
    else//read fail
    {
        return false;
    }
}

bool Logger::read(QStringList &o_message)
{
    o_message.clear();
    QString _fileName = m_fileDestination;
    QFile file(_fileName);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
       while(!in.atEnd()){
           o_message.append(in.readLine() + "\n");
       }

        file.close();
        return true;
    }
    else//read fail
    {
        return false;
    }
}
