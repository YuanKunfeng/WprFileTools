/**
 * @file basicdefines.h
 * @brief 常量和字符串定义
 * @author Yuan Kunfeng
 * @version 2016.03.07
 *
 * #修改记录
 * [2015-03-07] (YKF) 初始版本
 *
 */
#ifndef BASICDEFINES_H
#define BASICDEFINES_H

#include <QColor>
#include <QString>
#include <QDir>
#include <QList>

/*******************************************************************************
 * 常量
 *******************************************************************************/
#define EARTH_RADIUS 6378.137  // 地球半径

/*******************************************************************************
 * 枚举值定义
 *******************************************************************************/
enum ColorListType { clSingle, clWarmCold, clCycle, clCompond};  //ColorListType:0(单调),1(冷暖),2(循环),3(复合)

/*******************************************************************************
 * 色彩定义
 *******************************************************************************/
//Flat UI
const QColor colorWindowBackground(235, 245, 250);
const QColor colorTitleBar(88, 115, 201);
const QColor colorMidNightBlue(44, 62, 80);     //#2C3E50 蓝黑
const QColor colorWetAsphalt(52, 73, 94);       //#34495E 蓝灰
const QColor colorConcrete(149, 165, 166);      //#95A5A6 中灰
const QColor colorAsBestOs(127, 140, 141);      //#7F8C8D 深灰
const QColor colorClouds(236, 240, 241);        //#ECF0F1 浅灰
const QColor colorSliver(189, 195, 199);        //#BDC3C7 银
const QColor colorTurquoise(26, 188, 156);      //#1ABC9C 青绿
const QColor colorGreenSea(22, 160, 133);       //#16A085 深水绿
const QColor colorEmerald(46, 204, 113);        //#2ECC71 翡翠绿
const QColor colorNephrite(39, 174, 96);        //#27AE60 深翠绿
const QColor colorPertermannRiver(52, 152, 216);//#3498D8 水蓝
const QColor colorBelizeHole(41, 128, 185);     //#2980B9 深水蓝
const QColor colorAmethyst(155, 89, 182);       //#9B59B6 水晶紫
const QColor colorWisteria(142, 68, 173);       //#8E44AD 紫藤
const QColor colorPomergranate(192, 57, 43);    //#C0392B 石榴红
const QColor colorAlizarin(231, 76, 60);        //#E74C3C 茜素红
const QColor colorCarrot(230, 126, 34);         //#E67E22 胡萝卜橙
const QColor colorPumpkin(211, 84, 0);          //#D35400 南瓜橙
//const QColor colorSunFlower(241, 196, 15);      //#F1C40F 向日葵黄
const QColor colorOrange(243, 156, 18);         //#F39C12 桔黄

//BootFlat UI
const QColor colorBlueJeans(93, 156, 236);      //#5D9CEC蓝
const QColor colorBlueJeansDark(74, 137, 220);  //#4A89DC深蓝
const QColor colorAqua(79, 193, 233);           //#4FC1E9蓝偏绿
const QColor colorAquaDark(59, 175, 218);       //#3BAFDA深蓝偏绿
const QColor colorMint(72, 207, 173);           //#48CFAD薄荷绿
const QColor colorMintDark(55, 188, 155);       //#37BC9B深薄荷绿
const QColor colorGrass(160, 212, 104);         //#A0D468草绿
const QColor colorGrassDark(140, 193, 82);      //#8CC152深草绿
const QColor colorSunflower(255, 206, 84);      //#FFCE54向日葵黄
const QColor colorSunflowerDark(246, 187, 66);  //#F6BB42深向日葵黄
const QColor colorBitterSweet(252, 110, 81);    //#FC6E51红
const QColor colorBitterSweetDark(233, 87, 63); //#E9573F深红
const QColor colorGrapefruit(237, 85, 101);     //#ED5565砖红
const QColor colorGrapefruitDark(218, 68, 83);  //#DA4453深砖红
const QColor colorLightGray(245, 247, 250);     //#F5F7FA浅灰白
const QColor colorLightGrayDark(230, 233, 237); //#E6E9ED浅灰
const QColor colorMediumGray(204, 209, 217);    //#CCD1D9中灰
const QColor colorMediumGrayDark(170, 178, 189);//#AAB2BD深中灰
const QColor colorDarkGray(101, 109, 120);      //#656D78深灰
const QColor colorDarkGrayDark(67, 74, 84);     //#434A54深深灰
const QColor colorGrayText(87,87,87);

/*******************************************************************************
 * 数据结构定义
 *******************************************************************************/

/** 风相关信息 */
struct WindData
{
    double speed;   //风速
    double angleH;  //水平角度（和正北之间逆时针夹角0~360）
    double angleV;  //垂直角度（风向上吹正，向下负, -90~+90）

    WindData() { speed = 1100; angleH = 400;  angleV = 100; }
    WindData(double aspeed, double aangleH, double aangleV)
    { speed = aspeed; angleH = aangleH; angleV = aangleV; }
};
Q_DECLARE_METATYPE(WindData)

/** 风在各个方向的分量 */
struct WindVector
{
    double speed_x;     //迎头风，相对机头方向，顺正逆负
    double speed_y;     //侧风，相对机头方向，左负右正
    double speed_z;     //垂直风，上正下负
    double angle_alpha; //水平投影相对机头方向的夹角，+-180，左负右正
    double angle_beta;  //相对垂直投影的夹角，+-180，下负上正
};

/** 风统计信息 */
struct WindStatData
{
    int period;         //统计时间
    double maxSpeed;
    double minSpeed;
    double maxAngle;
    double minAngle;
};
Q_DECLARE_METATYPE(WindStatData)

/** 自动气象站信息 */
struct AwsData
{
    float temperature;  //温度,单位度，负数为零下
    float humidity;     //湿度，百分比
    float pressure;     //气压,单位KPa(?需要文档确认)
    float rainfall;     //一分钟累加降水量，单位毫米
    WindData wind;      //即时风速风向
};
Q_DECLARE_METATYPE(AwsData)

/** 站点的类型 */
enum SiteType { AutoStationSite, ProfilerSite, WeatherRadarSite };

struct SiteInfo
{
    QString name;
    SiteType type;
    double latitude;
    double longitude;
};
Q_DECLARE_METATYPE(SiteInfo)

#endif // BASICDEFINES_H
