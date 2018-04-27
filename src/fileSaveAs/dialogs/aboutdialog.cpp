#include "aboutdialog.h"
#include <QPlainTextEdit>
#include <QDir>
#include <QTextStream>
#include <QVBoxLayout>

AboutDialog::AboutDialog():
    ShadowDialog(tr("关于"))
{
    setFixedSize(500, 500);
    texteditor = new QPlainTextEdit(this);
    texteditor->setReadOnly(true);
    QFile file(QDir::currentPath() + "/FileSaveAs.txt");
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        texteditor->setPlainText(tr("帮助文件FileSaveAs.txt丢失。"));
    }
    else
    {
        QTextStream in(&file);
        texteditor->setPlainText(in.readAll());
    }

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(texteditor);
    mainSpace->setLayout(layout);
}
