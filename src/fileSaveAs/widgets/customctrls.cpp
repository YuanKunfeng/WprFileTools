#include "customctrls.h"

#include <QMouseEvent>
#include <QButtonGroup>
#include <QPainter>
#include <QHBoxLayout>
#include <QDebug>
#include <QMapIterator>
#include "dialogs/messagedialog.h"

/*******************************************************************************
 * ImageButton
 *******************************************************************************/
ImageButton::ImageButton(QWidget *parent)
    : QPushButton(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

ImageButton::ImageButton(const QString &text, const QString &image, QWidget *parent)
    : QPushButton(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setToolTip(text);
    setImageName(image);
}

void ImageButton::setImageName(const QString &image)
{
    QPixmap pixmap(image);
    if(m_imgList.count() != 0)
    {
        m_imgList.clear();
    }
    for(int i=0; i < 4 ; i++)
    {
        m_imgList.append(pixmap.copy(i*(pixmap.width()/4), 0, pixmap.width()/4, pixmap.height()));
    }
    m_imgIndex = 0;
    setFixedSize(m_imgList.at(0).size());
}

void ImageButton::setButtonStatus(ImageButton::ButtonStatus status)
{
    if(isEnabled())
    {
        switch (status)
        {
        case NORMAL: m_imgIndex = 0; break;
        case ENTER:  m_imgIndex = 1; break;
        case PRESS:  m_imgIndex = 2; break;
        default:                     break;
        }
    }
    else
    {
        m_imgIndex = 3;
    }
    update();
}

void ImageButton::changeEvent(QEvent *event)
{
    if((event->type() == QEvent::EnabledChange) && (!isEnabled()))
    {
        setButtonStatus(DISABLE);
    }
}

void ImageButton::checkStateSet()
{
    if(isChecked())
        setButtonStatus(ENTER);
    else
        setButtonStatus(NORMAL);
}

void ImageButton::enterEvent(QEvent *)
{
    setButtonStatus(ENTER);
}

void ImageButton::leaveEvent(QEvent *)
{
    if(isChecked())
        setButtonStatus(ENTER);
    else
        setButtonStatus(NORMAL);
}

void ImageButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }
    setButtonStatus(PRESS);
}

void ImageButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton)
    {
        event->ignore();
        return;
    }

    if(rect().contains(event->pos()))
    {
        if(isEnabled())
            emit clicked();
        setButtonStatus(ENTER);
        event->accept();
    }
    else
    {
        setButtonStatus(NORMAL);
        event->ignore();
    }
}

void ImageButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(rect(), m_imgList.at(m_imgIndex));
}

/*******************************************************************************
 * ImageTabBar
 *******************************************************************************/
ImageTabBar::ImageTabBar(QWidget *parent)
    : QWidget(parent), m_spacing(-5), m_buttonIdx(-1), m_startSpace(20)
{
    m_mapper = new QSignalMapper(this);
    connect(m_mapper, SIGNAL(mapped(int)), this, SLOT(onButtonClicked(int)));
}

void ImageTabBar::addImageTab(const QString &text, const QString &image, int id)
{
    ImageButton *tabButton = new ImageButton(text, image, this);
    tabButton->setCheckable(true);
    QPair<int, ImageButton *> pair(id, tabButton);
    m_buttons.append(pair);
    m_mapper->setMapping(tabButton, id);
    connect(tabButton, SIGNAL(clicked()), m_mapper, SLOT(map()));

    if(minimumWidth() < tabButton->width())
        setMinimumWidth(tabButton->width());
}

void ImageTabBar::setSpacing(int spacing, int startSpace)
{
    m_startSpace = startSpace;
    m_spacing = spacing;
    update();
}

void ImageTabBar::setActiveTab(int id)
{
    onButtonClicked(id);
}

void ImageTabBar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect tmprect;

    for(int i=0; i<m_buttons.count(); ++i)
    {
        ImageButton *tabButton = m_buttons.at(i).second;
        QSize tabSize = tabButton->size();
        if(i==0)
        {
            tmprect.setRect(rect().topLeft().x(),
                            m_startSpace + rect().topLeft().y(),
                            tabSize.width(),
                            tabSize.height());
        }
        else
        {
            tmprect.adjust(0, tmprect.height() + m_spacing,
                           0, tabSize.height() + m_spacing);
        }
        tabButton->setGeometry(tmprect);
        tabButton->update();
    }
}

void ImageTabBar::onButtonClicked(int id)
{
    m_buttonIdx = id;
    for(int i=0; i<m_buttons.count(); ++i)
    {
        ImageButton *tabButton = m_buttons.at(i).second;
        if(m_buttons.at(i).first==id)
        {
            tabButton->setChecked(true);
            tabButton->raise();
        }
        else
        {
            tabButton->setChecked(false);
            tabButton->lower();
        }
        tabButton->update();
    }
    emit tabClicked(id);
}

/********************************************************
 * ValueLabel 显示参数数值的Label
 ********************************************************/
ValueLabel::ValueLabel(QString text, QString iconName, QString value, QWidget *parent) :
     QWidget(parent), m_iconName(iconName), m_text(text), m_value(value)
{
    setAttribute(Qt::WA_TranslucentBackground);
    m_iconLabel = new QLabel(this);
    m_iconLabel->setVisible(m_iconName != "");
    if(m_iconName != "")
    {
        m_iconLabel->setPixmap(QPixmap(m_iconName));
        m_iconLabel->setMaximumSize(20, 20);
    }
    m_iconLabel->setScaledContents(true);

    m_textLabel = new QLabel(this);
    m_textLabel->setStyleSheet("QLabel{font:bold 微软雅黑;}");
    m_textLabel->setWordWrap(true);
//    m_textLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    m_valueLabel = new QLabel(this);
    m_valueLabel->setWordWrap(true);
//    m_valueLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_iconLabel);
    layout->addWidget(m_textLabel);
    layout->addWidget(m_valueLabel);
    setLayout(layout);
    setContentsMargins(2, 0, 2, 0);
    updateUI();
}

void ValueLabel::updateUI()
{
    m_textLabel->setText(m_text);
    m_valueLabel->setText(m_value);
}

/********************************************************
 * TraceableWidget 随着鼠标移动，显示焦点框的Widget
 ********************************************************/
void TraceableWidget::enterEvent(QEvent *event)
{
    m_isMouseEnter = true;
    update();
    QWidget::enterEvent(event);
}

void TraceableWidget::leaveEvent(QEvent *event)
{
    m_isMouseEnter = false;
    update();
    QWidget::leaveEvent(event);
}

void TraceableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    update();
    emit clicked();
    QWidget::mouseReleaseEvent(event);
}

void TraceableWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    if(m_isMouseEnter)
    {
        painter.setPen(colorConcrete);
        painter.drawRect(rect());
    }
}


