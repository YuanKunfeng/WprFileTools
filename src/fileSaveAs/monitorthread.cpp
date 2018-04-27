/**
 * @file monitorthread.cpp
 * @brief 工作线程相关类
 * @author 苑昆峰
 * @version 2016.06.08
 *
 * #修改记录
 * [2015.11.13] (YKF) 初始版本
 * [2016.06.08] (YKF) 增加功能
 *
 */

#include "monitorthread.h"
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include "managers/configdepository.h"
#include "managers/logmanager.h"

MonitorThread::MonitorThread(QObject *parent):
    QThread(parent),m_isQuit(true)
{}

MonitorThread::~MonitorThread()
{
    stopMonitor();
    quit();
    wait();
}

/**
 * @brief MonitorThread::startMonitor 开始线程
 *
 */
void MonitorThread::startMonitor()
{
    readParams();
    m_isQuit = false;
    if (!isRunning())
        start();
}

/**
 * @brief MonitorThread::stopMonitor 停止线程
 */
void MonitorThread::stopMonitor()
{
    m_mutex.lock();
    m_isQuit = true;
    m_mutex.unlock();
    //quit();
}

/**
 * @brief MonitorThread::run
 *      线程运行主函数，完成所有的处理工作
 */
void MonitorThread::run()
{
    //循环处理原数据目录文件，将文件移动到相应目录
    while(!m_isQuit)
    {
        sleep(2);
        m_inputFileList.clear();
        foreach (QString path, m_inputDirList)
        {
            findFilesInPath(path);
        }

        if(m_inputFileList.count() != 0)
        {
            //按顺序处理文件
            for(int i = 0; i < m_inputFileList.count(); ++i)
            {
                QString filePathName = m_inputFileList.at(i);    //注意，filePathName带路径

                //获取目标文件名
                QFileInfo fileInfo(filePathName);
                QFile srcfile(filePathName);
                QString destFileName = getDestFileName(fileInfo).toUpper();

                //备份文件。如果指定了分类存放，按照规则分目录存放
                QString backDir;
                if(m_isBackupEnable)
                {
                    if(m_isBackupUseSubDir)
                    {
                        backDir = getBackupDir(destFileName).toUpper();
                    }
                    else
                    {
                        backDir = m_backupDir;
                    }

                    QDir bdir(backDir);
                    if(!bdir.exists())
                    {
                        bdir.mkpath(backDir);
                    }
                    QFile::remove(backDir + "/" + destFileName);//删除同名文件，防止复制失败
                    srcfile.copy(backDir + "/" + destFileName); //备份
                }

                if(!(m_isBackupOnly && m_isBackupEnable))
                {
                    //复制到目标目录。成功后删除原文件，否则直接进行下一个文件，原文件保留待下次处理
                    QString outSubDir = getOutputSubDir(destFileName).toUpper();
                    foreach(QString dir, m_outputDirList)
                    {
                        QString destDir = dir + outSubDir;
                        QDir outdir(destDir);
                        if(!outdir.exists())
                        {
                            outdir.mkpath(destDir);
                        }

                        QFile::remove(destDir + "/" + destFileName);//删除同名文件，防止复制失败
                        srcfile.copy(destDir + "/" + destFileName);
                    }
                }
                msleep(100);
                srcfile.remove();
                qDebug() << i << fileInfo.fileName();
                emit step(i+1, m_inputFileList.count(), fileInfo.fileName());

                //每次循环均判断中止标识，便于及时响应界面退出按钮
                if(m_isQuit)
                {
                    break;
                }
            }
            if(m_isQuit)
            {
                //如果中止，不走进度
                emit showInfo(tr("%1 停止").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
            else
            {
                emit step(m_inputFileList.count(), m_inputFileList.count(),
                          tr("%1 处理完成").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")));
            }
        }

        //如果文件nodelete存在。不删除文件夹，否则删除原目录下所有空目录
        foreach (QString path, m_inputDirList)
        {
            QFile flagFile(path + "/nodelete");
            if(!flagFile.exists())
            {
                deleteEmptyDirs(path);
            }
        }

    }
}

/**
 * @brief MonitorThread::getDestDir 根据文件名，得到对应的输出目录
 * @param fileName 输入参数, 原文件名
 * @return 文件对应的输出目录，失败返回空字符串
 */
QString MonitorThread::getOutputSubDir(const QString &fileName)
{
    QString dirString = "";
    QFileInfo fileinfo(fileName);

    //按照分割符"_"拆分文件名
    QStringList fileNameParts;
    fileNameParts = fileinfo.baseName().split("_");
    if(fileNameParts.count() < 9)
        return dirString;       //文件名格式错误？

    QString fsite, fdate0, fdate1, ftype;      // 站号,年月, 年月日, 文件类型
    fdate0 = fileNameParts.at(4).mid(0, 6); //201511
    fdate1 = fileNameParts.at(4).mid(0, 8); //20151111
    ftype = fileNameParts.at(8);    //FFT/STA/HALF...
    fsite = fileNameParts.at(3);    //59758

    //按照规则组合，获得输出目录
    if(fileinfo.suffix() == "BIN")
    {
        if(ftype == "STA")
        {
            dirString = "/STATUS/" + fsite + "/" + fdate0 + "/" + fdate1;
        }
        else
        {
            dirString = "/FFT/" + fsite + "/" + fdate0 + "/" + fdate1;
        }
    }
    else if(fileinfo.suffix() == "FTS")
    {
        dirString = "/PRODUCT/" + fsite + "/BINARY/" + ftype + "/" + fdate0 + "/" + fdate1;
    }
    else if(fileinfo.suffix() == "TXT")
    {
        dirString = "/PRODUCT/" + fsite + "/TXT/" + ftype + "/" + fdate0 + "/" + fdate1;
    }

    return dirString;
}

/**
 * @brief MonitorThread::getBackupDir 根据文件名，得到对应的备份目录
 * @param fileName  输入参数, 原文件名
 * @return 文件对应的备份目录，失败返回空字符串
 */
QString MonitorThread::getBackupDir(const QString &fileName)
{
    QString dirString = "";
    QFileInfo fileinfo(fileName);

    //按照分割符"_"拆分文件名
    QStringList fileNameParts;
    fileNameParts = fileinfo.baseName().split("_");
    if(fileNameParts.count() < 9)
        return dirString;       //文件名格式错误？

    QString fsite, fdate0, fdate1, ftype;      // 站号,年月, 年月日, 文件类型
    fdate0 = fileNameParts.at(4).mid(0, 6); //201511
    fdate1 = fileNameParts.at(4).mid(0, 8); //20151111
    ftype = fileNameParts.at(8);    //FFT/STA/HALF...
    fsite = fileNameParts.at(3);    //59758

    //按照规则组合，获得输出目录
    if(fileinfo.suffix() == "BIN")
    {
        if(ftype == "STA")
        {
            dirString = m_backupDir + "/STATUS/" + fsite + "/" + fdate0 + "/" + fdate1;
        }
        else
        {
            dirString = m_backupDir + "/FFT/" + fsite + "/" + fdate0 + "/" + fdate1;
        }
    }
    else if(fileinfo.suffix() == "FTS")
    {
        dirString = m_backupDir + "/PRODUCT/" + fsite + "/BINARY/" + ftype + "/" + fdate0 + "/" + fdate1;
    }
    else if(fileinfo.suffix() == "TXT")
    {
        dirString = m_backupDir + "/PRODUCT/" + fsite + "/TXT/" + ftype + "/" + fdate0 + "/" + fdate1;
    }

    return dirString;
}

/**
 * @brief MonitorThread::findFileInPath
 *      递归遍历指定文件夹下所有文件，写入m_SourceFileList列表
 * @param path 待处理文件夹 *
 * @return 成功返回true, 失败返回false
 */
bool MonitorThread::findFilesInPath(const QString &path)
{
    QDir dir(path);
    if(!dir.exists())
    {
        return false;
    }

    QStringList nameFilter;
    nameFilter << "TXT" << "FTS" << "BIN";

    //获得所有子目录和文件
    QFileInfoList list = dir.entryInfoList(QDir::Dirs|QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot,
                                           QDir::DirsFirst);

    bool isDir = false;
    foreach(QFileInfo fileInfo, list)
    {
        isDir = fileInfo.isDir();
        if(isDir)
        {
            //如果是子目录，递归进入子目录查找
            findFilesInPath(fileInfo.filePath());
        }
        else
        {
            //如果是文件，将文件名写入m_SourceFileList列表
            if(nameFilter.contains(fileInfo.suffix().toUpper()))
            {
                m_inputFileList += fileInfo.filePath();
            }
        }
    }

    return true;
}

/**
 * @brief MonitorThread::deleteEmptyDirs  遍历目录，删除所有空目录
 * @param path 待处理文件夹
 * @return 成功返回true, 失败返回false。
 * @note 如果目录不存在，返回true。因为效果和删除相同。
 */
bool MonitorThread::deleteEmptyDirs(const QString &path)
{
    QDir dir(path);
    if(!dir.exists())
    {
        return true;
    }

    //获得所有子目录和文件
    QFileInfoList dirList = dir.entryInfoList(QDir::Dirs|QDir::NoSymLinks|QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList(QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot);

    //忽略nodelete文件
    if((fileList.count() == 0)
            || ((fileList.count() == 1) && (fileList.at(0).fileName() == "nodelete")))
    {
        bool isAllDeleted = true;
        // 如果文件夹没有文件,但有子目录，递归进入子目录进行删除
        for(int i = 0; i < dirList.count(); ++i)
        {
            QFileInfo dirInfo = dirList.at(i);
            isAllDeleted = isAllDeleted && deleteEmptyDirs(dirInfo.filePath());
        }
        //删除本级目录,再次计数。防止删除时有新文件
        fileList = dir.entryInfoList(QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot);
        if(fileList.count() == 0)
        {
            if(isAllDeleted && (!m_inputDirList.contains(path)))
            {
                isAllDeleted = isAllDeleted && dir.rmdir(dir.absolutePath());
                //isAllDeleted = isAllDeleted && dir.removeRecursively();
            }
        }
        return isAllDeleted;
    }
    return false;
}

/**
 * @brief MonitorThread::readParams
 *      读取运行参数
 */
void MonitorThread::readParams()
{
    QMutexLocker locker(&m_mutex);
    ConfigDepository &cfg = ConfigDepository::instance();
    m_inputDirList = cfg.getValue("inputDir").toString().split(";", QString::SkipEmptyParts);
    m_outputDirList = cfg.getValue("outputDir", "").toString().split(";", QString::SkipEmptyParts);
    m_backupDir = cfg.getValue("backupDir").toString();
    m_isBackupEnable = cfg.getValue("isBackupEnable").toBool();
    m_isBackupUseSubDir = cfg.getValue("isBackupUseSubDir").toBool();
    m_isBackupOnly = cfg.getValue("isBackupOnly").toBool();
}

/**
 * @brief MonitorThread::getDestFileName 获取新文件名。
 * @param fileInfo 待处理的文件
 * @return 新文件名
 * @note 改名规则为：*_FFT.fts->*_TAD.fts, *_HOBS.fts->*_HALF.fts, *_OOBS.fts->*_HOUR.fts, *_TFT.fts->*_TRD.fts
 */
QString MonitorThread::getDestFileName(const QFileInfo &fileInfo)
{
    QString str = fileInfo.fileName();
    if(fileInfo.suffix() == "FTS")
    {
        QStringList fileNameParts;
        fileNameParts = fileInfo.baseName().split("_");
        if(fileNameParts.count() < 9)
            return str;       //文件名格式错误？

        if(fileNameParts.at(8) == "FFT")
        {
            fileNameParts.replace(8, "TAD");
        }
        else if(fileNameParts.at(8) == "HOBS")
        {
            fileNameParts.replace(8, "HALF");
        }
        else if(fileNameParts.at(8) == "OOBS")
        {
            fileNameParts.replace(8, "HOUR");
        }
        else if(fileNameParts.at(8) == "TFT")
        {
            fileNameParts.replace(8, "TRD");
        }

        str = fileNameParts.join("_") + "." + fileInfo.completeSuffix();
    }
    return str;
}

