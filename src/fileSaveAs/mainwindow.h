#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QSystemTrayIcon>
#include "monitorthread.h"
#include "widgets/customctrls.h"
#include "dialogs/shadowdialog.h"

class QAction;
class QMenu;
class QProgressBar;
class QCheckBox;
class QLineEdit;
class QStatusBar;
class QListWidget;
class QGroupBox;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    QAction *restoreAction;
    QAction *quitAction;

    //标题栏
    ImageButton *m_aboutButton;      //
    ImageButton *m_minButton;        // 最小化
    ImageButton *m_closeButton;      // 关闭
    QWidget *m_topWidget;            // 标题栏

    //工作区
    QWidget *m_middleWidget;
    QListWidget *lstInputDir;
    QListWidget *lstOutputDir;
    QLineEdit *edtBackupDir;
    QPushButton *btnAddInputDir;
    QPushButton *btnDelInputDir;
    QPushButton *btnAddOutputDir;
    QPushButton *btnDelOutputDir;
    QPushButton *btnBackupDir;
    QGroupBox *grpIn;
    QGroupBox *grpOut;
    QGroupBox *grpBackup;
    QCheckBox *chkBackupUseSubdir;
    QCheckBox *chkBackupOnly;
    QLabel *lblProgressInfo;
    QLabel *lblBackup;
    QProgressBar *progressBar;
    QPushButton *btnStartStop;
    QPushButton *btnExit;
    QLabel *statusBar;
    //鼠标
    QPoint m_moveStartPos;       // 移动开始时鼠标位置
    bool m_isMousePressed;      // 按下鼠标左键
    QStringList m_inputDirList;        //原始数据输入路径
    QStringList m_outputDirList;       //output paths
    bool m_isdebug;

    MonitorThread m_thread;     //文件处理线程

private:
    void createActions();
    void createTrayIcon();
    void setupFrame();

private slots:
    void onTrayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void onDirChanged(QString txt);
    void onStep(int value, int total, QString info);        //进度条
    void updateUI();
    void onAboutClick();
    void onBtnAddInputDirClicked();
    void onBtnDelInputDirClicked();
    void onBtnAddOutputDirClicked();
    void onBtnDelOutputDirClicked();
    void onBtnStartStopClicked(bool checked);
    void onBtnExitClicked();
    void onBtnBackupDirClicked();
    void onBackupToggled(bool checked);
};

#endif // MAINWINDOW_H
