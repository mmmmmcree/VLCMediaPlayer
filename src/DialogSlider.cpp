#include "DialogSlider.h"
#include <QLayout>
#include <QEvent>
#include <QWheelEvent>
#include <QApplication>
#include <QPainter>

DialogSlider::DialogSlider(QWidget *parent) : QDialog(parent)
{
    auto *layout = new QVBoxLayout(this);
    _slider = new Slider(Qt::Vertical, this);
    // layout->addSpacerItem(new QSpacerItem(40, 20));
    layout->addWidget(_slider);
    // layout->addSpacerItem(new QSpacerItem(40, 20));
    this->setLayout(layout);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::ToolTip);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setAttribute(Qt::WA_StyledBackground);
}

QSlider *DialogSlider::slider() const
{
    return _slider;
}

bool DialogSlider::event(QEvent *event)
{
    if (event->type() == QEvent::Wheel) {
        return QApplication::sendEvent(_slider, static_cast<QWheelEvent*>(event));
    }
    return QDialog::event(event);
}

void DialogSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(110, 169, 247, 100));
    painter.drawRoundedRect(this->rect(), this->width() / 4, this->width() / 4);
}