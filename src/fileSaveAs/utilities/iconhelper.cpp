#include "iconhelper.h"
#include <QPushButton>
#include <QLabel>
#include <QFontDatabase>

IconHelper::IconHelper(const QString &family, QObject* parent):
    QObject(parent)
{
    int fontId = QFontDatabase::addApplicationFont(family);
    QString fontName = QFontDatabase::applicationFontFamilies(fontId).at(0);
    m_iconFont = QFont(fontName);
}

void IconHelper::setIcon(QLabel* label, QChar c, int size)
{
    m_iconFont.setPointSize(size);
    label->setFont(m_iconFont);
    label->setText(c);
}

void IconHelper::setIcon(QPushButton* button, QChar c, int size)
{
    m_iconFont.setPointSize(size);
    button->setFont(m_iconFont);
    button->setText(c);
}
