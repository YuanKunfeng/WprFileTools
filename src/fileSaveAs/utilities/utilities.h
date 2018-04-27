/**
 * @file utilities.h
 * @brief 各种方便函数
 * @author Yuan Kunfeng
 * @version 2015.11.23
 *
 * #修改记录
 * [2015-10-29] (YKF) 初始版本
 *
 */
#ifndef UTILITIES_H
#define UTILITIES_H

#include <QString>
#include <QMessageBox>
#include "basicdefines.h"

/*******************************************************************************
 * 转换函数
 */
QString GPSToString(double latitude, double longitude);
int stringToChars(QString from, char* to, int maxCount);
QString colorToString(QColor color);
QColor colorWithAlpha(QColor color, int alpha = 100);
QList<QRgb> getColorList(ColorListType listType);
/*******************************************************************************
 * 方便函数
 */
void ShowMessage(QString msg);

void waitSometime(uint msec);


#endif // UTILITIES_H
