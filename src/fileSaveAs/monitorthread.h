
#ifndef MONITORTHREAD_H
#define MONITORTHREAD_H

#include <QThread>
#include <QMutex>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QFileInfo;
QT_END_NAMESPACE

class MonitorThread : public QThread
{
    Q_OBJECT
public:
    explicit MonitorThread(QObject *parent = 0);
    ~MonitorThread();

    void startMonitor();
    void stopMonitor();
signals:
    void step(int value, int total, QString info);  //进度条
    void showInfo(const QString & info);            //提示信息信号
protected:
    void run();

private:
    QMutex m_mutex;
    bool m_isQuit;                  //退出标识

    QStringList m_inputFileList;   //原始数据文件列表
    QStringList m_inputSubdirList; //原始数据文件夹子目录列表，删除空目录使用
    QStringList m_inputDirList;    //原始数据输入路径
    QStringList m_outputDirList;    //数据输出路径
    QString m_backupDir;            //备份路径
    bool m_isBackupEnable;          //备份原始文件
    bool m_isBackupUseSubDir;       //使用子目录分类备份
    bool m_isBackupOnly;

    QString getOutputSubDir(const QString &fileName);
    QString getBackupDir(const QString &fileName);
    QString getDestFileName(const QFileInfo &fileInfo);
    bool findFilesInPath(const QString &path);
    bool deleteEmptyDirs(const QString &path);
    void readParams();
};

#endif // MONITORTHREAD_H
