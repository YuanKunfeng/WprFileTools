#include "utilities.h"
#include <QElapsedTimer>
#include <QCoreApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QPoint>
#include <QRect>

QString GPSToString(double latitude, double longitude)
{
    //TODO: GPS转字符串
    return QObject::tr("%1, %2").arg(latitude).arg(longitude);
}


int stringToChars(QString from, char *to, int maxCount)
{
    QByteArray tmpchars = from.toLatin1();
    int charCount = qMin(tmpchars.size(), maxCount);

    for (int i = 0; i < charCount; ++i)
    {
        to[i] = tmpchars.at(i);
    }

    for (int i = charCount; i < maxCount; ++i)
    {
        to[i] = 0x0;
    }

    return charCount;
}

QString colorToString(QColor color)
{
    return QString("rgba(%1, %2, %3, %4)")
        .arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
}

QColor colorWithAlpha(QColor color, int alpha)
{
    QColor clr(color);
    clr.setAlpha(alpha);
    return clr;
}


void ShowMessage(QString msg)
{
    QMessageBox::information(NULL, "debug", msg);
}


void waitSometime(uint msec)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed() < msec)
    {
        QCoreApplication::processEvents();
    }
}

QList<QRgb> getColorList(ColorListType listType)
{
    QList<QRgb> list;
    switch(listType)
    {
    case clSingle:
    {
        // 单调色柱，主要用于单值变化产品
        for(int i = 0; i < 42; i++)
            list.append(qRgb(230, 25 + 5 * i, 25));
        for(int i = 42; i < 84; i++)
            list.append(qRgb(230 - 5 * (i - 42), 230, 25));
        for(int i = 84; i < 126; i++)
            list.append(qRgb(25, 230, 25 + 5 * (i - 84)));
        for(int i = 126; i < 168; i++)
            list.append(qRgb(25, 230 - 5 * (i - 126), 230));
        for(int i = 168; i < 180; i++)
            list.append(qRgb(25 + 5 * (i - 168), 25, 230));
        break;
    }
    case clWarmCold:
    {
        // 冷暖色柱，主要用于对称产品
        for(int i = 0; i < 20; i++)
            list.append(qRgb(0, 0, 195 + 3 * i));
        for(int i = 20; i < 55; i++)
            list.append(qRgb(0, 5 * (i - 19), 255));
        for(int i = 55; i < 90; i++)
            list.append(qRgb(8 + 7 * (i - 54), 175 + (i - 54) * 2, 255));
        for(int i = 90; i < 120; i++)
            list.append(qRgb(255, 255 - (i - 89), 255 - (i - 89) * 8));
        for(int i = 120; i < 150; i++)
            list.append(qRgb(255, 224 - (i - 119) * 7, 0));
        for(int i = 150; i < 180; i++)
            list.append(qRgb(255 - (i - 149) * 5, 14, 0));
        break;
    }
    case clCycle:
    {
        // 风向色柱，色柱颜色构成一个循环，主要用于描述风向
        for(int i = 0; i < 25; i++)
            list.append(qRgb(200 - 6 * i, 100, 255));
        for(int i = 25; i < 45; i++)
            list.append(qRgb(54 - 2 * (i - 25), 105 + 5 * (i - 25), 255));
        for(int i = 45; i < 70; i++)
            list.append(qRgb(16, 200 + 2 * (i - 44), 250 - (i - 45) * 4));
        for(int i = 70; i < 95; i++)
            list.append(qRgb(16 + 4 * (i - 69), 255, 150 - 4 * (i - 70)));
        for(int i = 95; i < 115; i++)
            list.append(qRgb(115 + 7 * (i - 94), 255, 90 - 4 * (i - 95)));
        for(int i = 115; i < 135; i++)
            list.append(qRgb(255, 255 - 6 * (i - 114), 10 + 4 * (i - 114)));
        for(int i = 135; i < 155; i++)
            list.append(qRgb(255, 130 - 4 * (i - 134), 90 + 3 * (i - 135)));
        for(int i = 155; i < 180; i++)
            list.append(qRgb(255 - 2 * (i - 154), 50 + 2 * (i - 154), 150 + 4 * (i - 154)));
        break;
    }
    case clCompond:
    {
        // 渐变色标1
        for(int i = 0; i < 18; i++)
            list.append(qRgb(244 - 10 * i, 244 - 10 * i, 244 - 10 * i));
        for(int i = 18; i < 54; i++)
            list.append(qRgb(0, 253 - 7 * (i - 18), 255 - 3 * (i - 18)));
        for(int i = 54; i < 60; i++)
            list.append(qRgb(35 - 7 * (i - 54), 255 - 2 * (i - 54), 0));
        for(int i = 60; i < 90; i++)
            list.append(qRgb(0, 239 - 6 * (i - 60), 0));
        for(int i = 90; i < 126; i++)
            list.append(qRgb(255, 255 - 4 * (i - 90), 0));
        for(int i = 126; i < 162; i++)
            list.append(qRgb(255 - 5 * (i - 126), 0, 0));
        for(int i = 162; i < 171; i++)
            list.append(qRgb(221, 32, 180 + 5 * (i - 162)));
        for(int i = 171; i < 180; i++)
            list.append(qRgb(221 - 5 * (i - 171), 32, 220));
        break;
    }
    default:
        break;
    }
    return list;
}

