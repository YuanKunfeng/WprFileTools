#include "configdepository.h"
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMessageBox>

static ConfigDepository *s_configDepoInstance;

/**
 * @brief ConfigDepository::ConfigDepository 普通的构造函数
 * @param parent
 */
ConfigDepository::ConfigDepository(QObject *parent) :
    QObject(parent), m_isLoaded(false)
{
    m_fileDestination = QDir::currentPath() + "/config.xml";
    load();
}

/**
 * @brief ConfigDepository::ConfigDepository 指定参数文件的构造函数
 * @param fileDestination 配置参数保存的文件
 * @param parent
 */
ConfigDepository::ConfigDepository(const QString &fileDestination, QObject *parent) :
    QObject(parent), m_fileDestination(fileDestination), m_isLoaded(false)
{
    load();
}

/**
 * @brief ConfigDepository::save 把所有参数保存进文件
 * @return 成功返回true， 失败返回false
 */
bool ConfigDepository::save()
{
    QFile file(m_fileDestination);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        return false;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("CONFIGS");
    QMapIterator<QString, QVariant> pos(m_values);
    while(pos.hasNext())
    {
        pos.next();
        xmlWriter.writeTextElement(pos.key(), pos.value().toString());
    }

    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file.close();

    return true;
}

/**
 * @brief ConfigDepository::load 从文件中读取所有配置参数
 * @return 成功返回true, 失败返回false
 */
bool ConfigDepository::load()
{
    m_values.clear();

    QFile file(m_fileDestination);
    if(!file.exists())
    {
        file.open(QFile::WriteOnly | QFile::Text);
        file.close();
    }
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        m_isLoaded = false;
        return false;
    }

    QXmlStreamReader xmlReader;
    xmlReader.setDevice(&file);

    while(!xmlReader.atEnd() && !xmlReader.hasError())
    {
        QXmlStreamReader::TokenType token = xmlReader.readNext();
        if(token == QXmlStreamReader::StartElement && xmlReader.name() != "CONFIGS")
        {
            QString key = xmlReader.name().toString();
            QString value = xmlReader.readElementText();
            m_values.insert(key, value);
        }
    }

    if(xmlReader.hasError())
    {
        m_isLoaded = false;
        file.close();
        return false;
    }
    m_isLoaded = true;
    return true;
}

/**
 * @brief ConfigDepository::instance 使用单例模式，获取单一的参数实例，保证全局只存在一个设置库
 * @return 参数库的全局唯一实例
 */
ConfigDepository &ConfigDepository::instance()
{
    if(!s_configDepoInstance)
    {
        s_configDepoInstance = new ConfigDepository();
    }
    return *s_configDepoInstance;
}

void ConfigDepository::destroyInstance()
{
    delete s_configDepoInstance;
    s_configDepoInstance = NULL;
}

/**
 * @brief ConfigDepository::setValue 设置参数key的值为value
 * @param key
 * @param value
 * @return 保存成功返回true，否则返回false
 * @note 发出ValueChanged信号
 */
bool ConfigDepository::setValue(const QString &key, const QVariant &value)
{
    QVariant oldVal;
    if(m_values.contains(key))
    {
        oldVal = m_values[key];
        m_values[key] = value;
    }
    else
    {
        m_values.insert(key, value);
    }

    bool isSaved = save();
    if(oldVal != value)
    {
        emit ConfigChanged(key);
    }
    return isSaved;
}

/**
 * @brief ConfigDepository::getValue 读取参数key的值。如果不存在，返回指定的默认值
 * @param key
 * @param defaultValue
 * @return 读到的参数值
 */
QVariant ConfigDepository::getValue(const QString &key, const QVariant &defaultValue) const
{
    if(m_values.contains(key))
    {
        return m_values[key];
    }
    return defaultValue;
}

/**
 * @brief ConfigDepository::contains 库中是否包含参数key
 * @param key
 * @return 包含返回true， 否则返回false
 */
bool ConfigDepository::contains(const QString &key) const
{
    return m_values.contains(key);
}

/**
 * @brief ConfigDepository::allKeys 返回库中所有的参数名称
 * @return 参数名称列表
 */
QStringList ConfigDepository::allKeys() const
{
    QStringList keys;
    QMapIterator<QString, QVariant> pos(m_values);
    while(pos.hasNext())
    {
        pos.next();
        keys.append(pos.key());
    }
    return keys;
}


void setDebugConfigration()
{
//    ConfigDepository::instance().setValue("saveElements", false);
//    ConfigDepository::instance().setPrivilage(upFactory);
}
