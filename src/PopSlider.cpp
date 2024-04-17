#include "PopSlider.h"
#include <QTimer>
#include <QTimerEvent>
#include <QResizeEvent>

PopSlider::PopSlider(QWidget *parent) : HoverFillPushButton(parent)
{
    dialog_slider = new DialogSlider(this);
}

QSlider *PopSlider::slider() const
{
    return dialog_slider->slider();
}

bool PopSlider::dialog_visible() const
{
    return dialog_slider->isVisible();
}

void PopSlider::enterEvent(QEnterEvent *event)
{
    HoverFillPushButton::enterEvent(event);
    QPoint p = this->mapToGlobal(QPoint(0, 0));
    QRect rect = this->rect(), slider_rect = dialog_slider->rect();
    p += QPoint((this->width() - dialog_slider->width()) / 2, -dialog_slider->height());
    dialog_slider->move(p);
    dialog_slider->show();
    this->startTimer(250);
}

void PopSlider::resizeEvent(QResizeEvent *event)
{
    HoverFillPushButton::resizeEvent(event);
    auto size = event->size();
    dialog_slider->setFixedSize(size.width() * 0.7, size.width() * 2);
}

void PopSlider::timerEvent(QTimerEvent *event)
{
    HoverFillPushButton::timerEvent(event);
    if (not dialog_slider or not dialog_slider->isVisible()) {
        killTimer(event->timerId());
        return;
    }
    QPoint p = dialog_slider->mapToGlobal(QPoint(0, 0));
    QRect area(p, QSize(dialog_slider->width(), dialog_slider->height() + this->height()));
    if (not area.contains(QCursor::pos()) and not dialog_slider->slider()->isSliderDown()) {
        dialog_slider->hide();
    }
}