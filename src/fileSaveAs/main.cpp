#include "mainwindow.h"
#include <QApplication>
#include <QSharedMemory>
#include "managers/logmanager.h"
#include "dialogs/messagedialog.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resource);

    QApplication a(argc, argv);

    //加载QSS样式表
    QFile qss(":/style/default");
    qss.open(QFile::ReadOnly);
    qApp->setStyleSheet(qss.readAll());
    qss.close();

    //程序单实例
    QSharedMemory sharedMem;
    sharedMem.setKey(QString("NRT-FSA"));
    if(sharedMem.attach())
    {
        MessageDialog::infomation(QObject::tr("软件已经运行."));
        return 1;
    }
    else
    {
        if(sharedMem.create(1))
        {
            //日志
//            QDir dir(QDir::currentPath());
//            if (!dir.exists("logs"))
//                dir.mkdir("logs");
//            QString logFile = QDir::currentPath() + "/logs/FileSaveAs.log";
//            LogManager::instance().addLogInstance(logFile, MOD_MAIN, lgDebug);
//            logInfo(MOD_MAIN, "Application start.");

            //系统托盘
            if (!QSystemTrayIcon::isSystemTrayAvailable())
            {
                MessageDialog::error(QObject::tr("没有找到可用的系统托盘。"));
                return 1;
            }

            QApplication::setQuitOnLastWindowClosed(false);

            MainWindow w;
            w.show();

            return a.exec();
        }
    }
    return 0;
}
