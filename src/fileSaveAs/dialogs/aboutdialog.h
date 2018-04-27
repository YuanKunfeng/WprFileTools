#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H
#include <dialogs/shadowdialog.h>
class QPlainTextEdit;

class AboutDialog: public ShadowDialog
{
public:
    AboutDialog();

private:
    QPlainTextEdit *texteditor;
};

#endif // ABOUTDIALOG_H
