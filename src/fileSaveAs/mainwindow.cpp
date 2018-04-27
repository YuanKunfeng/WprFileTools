#include "mainwindow.h"
#include <QCloseEvent>
#include <QAction>
#include <QMenu>
#include <QTimer>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAction>
#include <QMenu>
#include <QProgressBar>
#include <QCheckBox>
#include <QLineEdit>
#include <QPainter>
#include <QStatusBar>
#include <QApplication>
#include <QGroupBox>
#include <QListWidget>
#include <QDebug>
#include "managers/configdepository.h"
#include "dialogs/messagedialog.h"
#include "dialogs/aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
  , m_isdebug(false)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowTitle(tr("风廓线数据文件转换程序"));
    setWindowIcon(QIcon(":/images/icon"));
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(500, 700);

    ConfigDepository &cfg = ConfigDepository::instance();
    m_inputDirList = cfg.getValue("inputDir", "").toString().split(";", QString::SkipEmptyParts);
    m_outputDirList = cfg.getValue("outputDir", "").toString().split(";", QString::SkipEmptyParts);
    m_isdebug = cfg.getValue("debug", false).toBool();

    createActions();
    createTrayIcon();
    setupFrame();

    lstInputDir->addItems(m_inputDirList);
    lstOutputDir->addItems(m_outputDirList);
    edtBackupDir->setText(cfg.getValue("backupDir", "").toString());
    grpBackup->setChecked(cfg.getValue("isBackupEnable", false).toBool());
    chkBackupUseSubdir->setChecked(cfg.getValue("isBackupUseSubDir", true).toBool());
    chkBackupOnly->setChecked(cfg.getValue("isBackupOnly", false).toBool());

    connect(m_aboutButton, SIGNAL(clicked()), this, SLOT(onAboutClick()));
    connect(m_minButton, SIGNAL(clicked()), this, SLOT(showMinimized()));
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(lstInputDir, SIGNAL(currentRowChanged(int)), this, SLOT(updateUI()));
    connect(btnAddInputDir, SIGNAL(clicked()), this, SLOT(onBtnAddInputDirClicked()));
    connect(btnDelInputDir, SIGNAL(clicked()), this, SLOT(onBtnDelInputDirClicked()));
    connect(lstOutputDir, SIGNAL(currentRowChanged(int)), this, SLOT(updateUI()));
    connect(btnAddOutputDir, SIGNAL(clicked()), this, SLOT(onBtnAddOutputDirClicked()));
    connect(btnDelOutputDir, SIGNAL(clicked()), this, SLOT(onBtnDelOutputDirClicked()));
    connect(edtBackupDir, SIGNAL(textChanged(QString)), this, SLOT(onDirChanged(QString)));
    connect(btnBackupDir, SIGNAL(clicked()), this, SLOT(onBtnBackupDirClicked()));
    connect(btnStartStop, SIGNAL(clicked(bool)), this, SLOT(onBtnStartStopClicked(bool)));
    connect(btnExit, SIGNAL(clicked()), this, SLOT(onBtnExitClicked()));
    connect(grpBackup, SIGNAL(toggled(bool)), this, SLOT(onBackupToggled(bool)));
    connect(chkBackupOnly, SIGNAL(toggled(bool)), this, SLOT(onBackupToggled(bool)));
    connect(chkBackupUseSubdir, SIGNAL(toggled(bool)), this, SLOT(onBackupToggled(bool)));
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(onTrayIconActivated(QSystemTrayIcon::ActivationReason)));
    connect(&m_thread, SIGNAL(step(int,int,QString)), this, SLOT(onStep(int,int,QString)));
    connect(&m_thread, SIGNAL(showInfo(QString)), lblProgressInfo, SLOT(setText(QString)));
    connect(&m_thread, SIGNAL(started()), this, SLOT(updateUI()));
    connect(&m_thread, SIGNAL(finished()), this, SLOT(updateUI()));

    trayIcon->show();
    updateUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible())
    {
        hide();
        event->ignore();
    }    
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // 必须是在标题栏
    if(event->pos().y() > shadowWidth+40)
        return;

    // 只能是鼠标左键移动和改变大小
    if(event->button() == Qt::LeftButton)
    {
        m_isMousePressed = true;
    }

    // 窗口移动距离
    m_moveStartPos = event->globalPos() - pos();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *)
{
    m_isMousePressed = false;
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //移动窗口
    if(m_isMousePressed)
    {
        QPoint l_MovePos = event->globalPos();
        move(l_MovePos - m_moveStartPos);
    }
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    //绘制阴影
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    //4个角
    int h = height();
    int w= width();
    painter.drawPixmap(QRect(0, 0, 2*shadowWidth, 2*shadowWidth), QPixmap(":/images/widget_shadow_lefttop"));
    painter.drawPixmap(QRect(0, h-2*shadowWidth, 2*shadowWidth, 2*shadowWidth), QPixmap(":/images/widget_shadow_leftbottom"));
    painter.drawPixmap(QRect(w-2*shadowWidth, 0, 2*shadowWidth, 2*shadowWidth), QPixmap(":/images/widget_shadow_righttop"));
    painter.drawPixmap(QRect(w-2*shadowWidth, h-2*shadowWidth, 2*shadowWidth, 2*shadowWidth), QPixmap(":/images/widget_shadow_rightbottom"));
    //4条边
    painter.drawPixmap(QRect(0, 2*shadowWidth, shadowWidth, h-4*shadowWidth), QPixmap(":/images/widget_shadow_left"));
    painter.drawPixmap(QRect(w-shadowWidth, 2*shadowWidth, shadowWidth, h-4*shadowWidth), QPixmap(":/images/widget_shadow_right"));
    painter.drawPixmap(QRect(2*shadowWidth, 0, w - 4*shadowWidth, shadowWidth), QPixmap(":/images/widget_shadow_top"));
    painter.drawPixmap(QRect(2*shadowWidth, h-shadowWidth, w-4*shadowWidth, shadowWidth), QPixmap(":/images/widget_shadow_bottom"));

    QWidget::paintEvent(event);
}

void MainWindow::onTrayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        showNormal();
        break;
    default:
        ;
    }
    updateUI();
}

/**
 * @brief MainWindow::onDirChanged 目录编辑框内容的处理
 * @param txt 编辑框的内容
 * @note 此事件响应setText和直接人工编辑两种方式
 */
void MainWindow::onDirChanged(QString txt)
{
    QDir dir(txt);
    QLineEdit *edt = qobject_cast<QLineEdit *>(sender());
    if(!dir.exists())
    {
        edt->setStyleSheet("color: red");
        lblProgressInfo->setStyleSheet("color: red");
        lblProgressInfo->setText(tr("指定的路径不存在。"));
    }
    else
    {
        edt->setStyleSheet("color: black");
        lblProgressInfo->setStyleSheet("color: black");
        lblProgressInfo->setText("");

        if(sender() == edtBackupDir)
        {
            ConfigDepository::instance().setValue("backupDir", txt);
        }
    }
    updateUI();
}

void MainWindow::updateUI()
{
    ConfigDepository &cfg = ConfigDepository::instance();
    bool isBackupEnable = cfg.getValue("isBackupEnable", false).toBool();
    bool isBackupOnly = cfg.getValue("isBackupOnly", false).toBool();
    QString backpath = cfg.getValue("backupDir").toString();
    progressBar->setVisible(m_thread.isRunning());
    restoreAction->setEnabled(isMaximized() || !isVisible());
    btnDelInputDir->setEnabled(lstInputDir->selectionModel()->currentIndex().isValid());
    btnDelOutputDir->setEnabled(lstOutputDir->selectionModel()->currentIndex().isValid());
    lblBackup->setEnabled(isBackupEnable);
    chkBackupUseSubdir->setEnabled(isBackupEnable);
    chkBackupOnly->setEnabled(isBackupEnable);
    edtBackupDir->setEnabled(isBackupEnable);
    btnBackupDir->setEnabled(isBackupEnable);
    btnStartStop->setEnabled(!(m_inputDirList.isEmpty()
                               || m_outputDirList.isEmpty()
                               || (!isBackupEnable && m_outputDirList.isEmpty())
                               || (isBackupEnable && backpath.isEmpty())));
    if(isBackupOnly && isBackupEnable && !backpath.isEmpty())
    {
        btnStartStop->setEnabled(true);
    }

    btnExit->setEnabled(!btnStartStop->isChecked());

    if(btnStartStop->isChecked())
    {
        btnStartStop->setText(tr("停止"));
    }
    else
    {
        btnStartStop->setText(tr("开始"));
    }
}

void MainWindow::onAboutClick()
{
    AboutDialog dlg;
    dlg.exec();
}

void MainWindow::createActions()
{
    restoreAction = new QAction(tr("恢复窗口(&R)"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("退出(&Q)"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(onBtnExitClicked()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/images/trayIco"));
    trayIcon->setToolTip(tr("风廓线数据文件转换程序"));
    trayIcon->setContextMenu(trayIconMenu);
}

void MainWindow::setupFrame()
{
    //顶部标题栏
    m_topWidget = new QWidget(this);
    m_topWidget->setObjectName("AppTitleBar");
    m_topWidget->setFixedHeight(50);

    QLabel *appLogoLabel = new QLabel(this);
    appLogoLabel->setPixmap(QPixmap(":/images/icon"));
    appLogoLabel->setFixedSize(30, 30);
    appLogoLabel->setScaledContents(false);
    appLogoLabel->setStyleSheet(QString("QLabel{background: transparent;}"));

    QLabel *appTitleLabel = new QLabel(this);
    appTitleLabel->setObjectName("AppTitle");
    appTitleLabel->setText(tr("风廓线数据文件归档程序"));

    m_aboutButton = new ImageButton(tr("关于"),":/images/sys_button_question", this);
    m_minButton = new ImageButton(tr("最小化"), ":/images/sys_button_min", this);
    m_closeButton = new ImageButton(tr("关闭"),":/images/sys_button_close", this);

    QHBoxLayout *systemLayout_sub = new QHBoxLayout();
    systemLayout_sub->addWidget(m_aboutButton);
    systemLayout_sub->addWidget(m_minButton);
    systemLayout_sub->addWidget(m_closeButton);
    systemLayout_sub->setSpacing(0);
    systemLayout_sub->setContentsMargins(0, 0, 0, 0);

    QVBoxLayout *systemLayout = new QVBoxLayout();
    systemLayout->addLayout(systemLayout_sub);
    systemLayout->addStretch();

    QHBoxLayout *topLayout = new QHBoxLayout();
    topLayout->setSpacing(2);
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addWidget(appLogoLabel, 0, Qt::AlignVCenter);
    topLayout->addWidget(appTitleLabel, 0, Qt::AlignVCenter);
    topLayout->addStretch();
    topLayout->addLayout(systemLayout);
    m_topWidget->setLayout(topLayout);

    //工作区
    grpIn = new QGroupBox(tr("文件输入目录"), this);
    grpIn->setFlat(true);
    lstInputDir = new QListWidget(this);
    lstInputDir->setSelectionBehavior(QAbstractItemView::SelectRows);
    lstInputDir->setSelectionMode(QAbstractItemView::SingleSelection);
    btnAddInputDir = new QPushButton(tr("添加"), this);
    btnAddInputDir->setProperty("kind", "browseBtn");
    btnDelInputDir = new QPushButton(tr("删除"), this);
    btnDelInputDir->setProperty("kind", "browseBtn");

    grpOut = new QGroupBox(tr("文件输出目录"), this);
    grpOut->setFlat(true);
    lstOutputDir = new QListWidget(this);
    lstOutputDir->setSelectionBehavior(QAbstractItemView::SelectRows);
    lstOutputDir->setSelectionMode(QAbstractItemView::SingleSelection);
    btnAddOutputDir = new QPushButton(tr("添加"), this);
    btnAddOutputDir->setProperty("kind", "browseBtn");
    btnDelOutputDir = new QPushButton(tr("删除"), this);
    btnDelOutputDir->setProperty("kind", "browseBtn");

    grpBackup = new QGroupBox(tr("备份原始文件"), this);
    grpBackup->setCheckable(true);
    grpBackup->setFlat(true);
    chkBackupOnly = new QCheckBox(tr("仅备份，不分类归档(剪切模式)"));
    chkBackupUseSubdir = new QCheckBox(tr("使用[/站号/日期]按目录存放"));
    lblBackup = new QLabel(tr("备份目录:"), this);
    edtBackupDir = new QLineEdit(this);
    edtBackupDir->setPlaceholderText(tr("请选择备份文件目录"));
    lblBackup->setBuddy(edtBackupDir);
    btnBackupDir = new QPushButton(tr("浏览"), this);
    btnBackupDir->setProperty("kind", "browseBtn");

    lblProgressInfo = new QLabel("",this);
    lblProgressInfo->setWordWrap(true);
    progressBar = new QProgressBar(this);
    progressBar->setValue(0);

    btnStartStop = new QPushButton("开始", this);
    btnStartStop->setObjectName("startBtn");
    btnStartStop->setMinimumSize(QSize(100, 0));
    btnStartStop->setCheckable(true);
    btnStartStop->setChecked(false);
    btnStartStop->setDefault(true);

    btnExit = new QPushButton("退出", this);
    btnExit->setMinimumSize(QSize(100, 0));
    btnExit->setObjectName("exitBtn");

    statusBar = new QLabel(tr("2018 v1.4 %1")
                           .arg(m_isdebug?"Dbg":""), this);
    statusBar->setObjectName("statusBar");
    statusBar->setAlignment(Qt::AlignCenter);

    //布局
    QVBoxLayout *layoutInBtns = new QVBoxLayout();
    layoutInBtns->addWidget(btnAddInputDir);
    layoutInBtns->addWidget(btnDelInputDir);
    layoutInBtns->addStretch();
    QHBoxLayout *layoutIn = new QHBoxLayout(grpIn);
    layoutIn->addWidget(lstInputDir);
    layoutIn->addLayout(layoutInBtns);

    QVBoxLayout *layoutOutBtns = new QVBoxLayout();
    layoutOutBtns->addWidget(btnAddOutputDir);
    layoutOutBtns->addWidget(btnDelOutputDir);
    layoutOutBtns->addStretch();
    QHBoxLayout *layoutOut = new QHBoxLayout(grpOut);
    layoutOut->addWidget(lstOutputDir);
    layoutOut->addLayout(layoutOutBtns);

    QHBoxLayout *layoutOption1 = new QHBoxLayout();
    layoutOption1->addWidget(lblBackup);
    layoutOption1->addWidget(edtBackupDir);
    layoutOption1->addWidget(btnBackupDir);
    QVBoxLayout *layoutBackup = new QVBoxLayout(grpBackup);
    layoutBackup->addLayout(layoutOption1);
    layoutBackup->addWidget(chkBackupOnly);
    layoutBackup->addWidget(chkBackupUseSubdir);

    QWidget *wgtProgress = new QWidget(this);
    wgtProgress->setMinimumHeight(60);
    QVBoxLayout *layoutProgress = new QVBoxLayout(wgtProgress);
    layoutProgress->addWidget(lblProgressInfo);
    layoutProgress->addWidget(progressBar);

    QHBoxLayout *layoutbuttons = new QHBoxLayout();
    layoutbuttons->addStretch();
    layoutbuttons->addWidget(btnStartStop);
    layoutbuttons->addWidget(btnExit);
    layoutbuttons->addStretch();

    m_middleWidget = new QWidget(this);
    QVBoxLayout *layoutMiddle = new QVBoxLayout();
    layoutMiddle->addWidget(grpIn);
    layoutMiddle->addWidget(grpOut);
    layoutMiddle->addWidget(grpBackup);
    layoutMiddle->addWidget(wgtProgress);
    layoutMiddle->addStretch();
    layoutMiddle->addLayout(layoutbuttons);

    m_middleWidget->setLayout(layoutMiddle);

    QVBoxLayout *layoutMain = new QVBoxLayout();
    layoutMain->setSpacing(0);
    layoutMain->setContentsMargins(shadowWidth, shadowWidth, shadowWidth, shadowWidth);
    layoutMain->addWidget(m_topWidget);
    layoutMain->addWidget(m_middleWidget);
    layoutMain->addWidget(statusBar);

    setLayout(layoutMain);
}

void MainWindow::onStep(int value, int total, QString info)
{
    if(total == 0)
    {
        progressBar->setValue(0);
    }
    else
    {
        progressBar->setValue(value * 100 / total);
        lblProgressInfo->setText(QString("<b>(%1 / %2)</b><br> %3").arg(value).arg(total).arg(info));
    }
    update();
}


void MainWindow::onBtnAddInputDirClicked()
{
    QString path;
    if(m_inputDirList.count() > 0)
        path = m_inputDirList.first();
    else
        path = QDir::homePath();
    QString txt = QFileDialog::getExistingDirectory(this, tr("选择原始文件存放目录"),
                  path,QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(txt != "")
    {
        lstInputDir->addItem(txt);
        m_inputDirList.append(txt);
    }
    updateUI();
}

void MainWindow::onBtnDelInputDirClicked()
{
    if(lstInputDir->selectionModel()->currentIndex().isValid())
    {
        int idx = lstInputDir->selectionModel()->currentIndex().row();
        QListWidgetItem *itm = lstInputDir->takeItem(idx);
        m_inputDirList.removeAt(idx);
        delete itm;
    }
    updateUI();
}

void MainWindow::onBtnAddOutputDirClicked()
{
    QString path;
    if(m_outputDirList.count() > 0)
        path = m_outputDirList.first();
    else
        path = QDir::homePath();

    QString txt = QFileDialog::getExistingDirectory(this, tr("选择输出文件存放目录"),
                  path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(txt != "")
    {
        lstOutputDir->addItem(txt);
        m_outputDirList.append(txt);
    }
    updateUI();
}

void MainWindow::onBtnDelOutputDirClicked()
{
    if(lstOutputDir->selectionModel()->currentIndex().isValid())
    {
        int idx = lstOutputDir->selectionModel()->currentIndex().row();
        QListWidgetItem *itm = lstOutputDir->takeItem(idx);
        m_outputDirList.removeAt(idx);
        delete itm;
    }
    updateUI();
}

void MainWindow::onBtnBackupDirClicked()
{
    QString backpath = ConfigDepository::instance().getValue("backupDir").toString();
    QString txt = QFileDialog::getExistingDirectory(this, tr("选择原始文件备份目录"),
                  backpath,QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    if(txt != "")
    {
         edtBackupDir->setText(txt);
    }
    updateUI();
}

void MainWindow::onBtnStartStopClicked(bool checked)
{
    if(checked)
    {
        //保存设置
        ConfigDepository &cfg = ConfigDepository::instance();
        cfg.setValue("inputDir", m_inputDirList.join(";"));
        cfg.setValue("outputDir", m_outputDirList.join(";"));
        cfg.setValue("backupDir", edtBackupDir->text());
        cfg.setValue("isBackupEnable", grpBackup->isChecked());
        cfg.setValue("isBackupUseSubDir", chkBackupUseSubdir->isChecked());
        cfg.setValue("isBackupOnly", chkBackupOnly->isChecked());
        m_thread.startMonitor();
    }
    else
    {
        m_thread.stopMonitor();
    }
    updateUI();
}

void MainWindow::onBtnExitClicked()
{
    if( MessageDialog::question(tr("确定退出程序？")) == QDialog::Accepted)
    {
        qApp->quit();
    }
}


void MainWindow::onBackupToggled(bool checked)
{
    ConfigDepository &cfg = ConfigDepository::instance();
    cfg.setValue("isBackupEnable", grpBackup->isChecked());
    cfg.setValue("isBackupOnly", chkBackupOnly->isChecked());
    cfg.setValue("isBackupUseSubDir", chkBackupUseSubdir->isChecked());

    updateUI();
}
