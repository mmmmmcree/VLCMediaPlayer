#include "slider.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QStyleOptionSlider>

Slider::Slider(QWidget *parent) : QSlider(parent)
{
    this->setMouseTracking(true);
}

Slider::Slider(Qt::Orientation orientation, QWidget *parent) : QSlider(orientation, parent)
{
    this->setMouseTracking(true);
}

void Slider::setInnerHandleColor(const QColor & color)
{
    _inner_handle_color = color;
}

void Slider::setOuterHandleColor(const QColor & color)
{
    _outer_handle_color = color;
}

void Slider::setGrooveAddColor(const QColor & color)
{
    _groove_add_color = color;
}

void Slider::setLinearGradientColorAt(qreal pos, const QColor & color)
{
    _gradient.setColorAt(pos, color);
}

QPoint Slider::get_handle_pos() const {
    auto rect = get_handle_rect();
    return QPoint(rect.x() + rect.width() / 2, rect.y() + rect.height() / 2);
}

QRect Slider::get_handle_rect() const {
    QStyleOptionSlider opt;
    this->initStyleOption(&opt);
    return this->style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
}

QRect Slider::get_groove_rect() const
{
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    return style()->subControlRect(QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
}

void Slider::paint_handle()
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);

    int outer_radius = _hovering ? _handle_radius * 1.2 : _handle_radius * 0.8;
    int inner_radius = outer_radius * 0.8;

    painter.setBrush(QBrush(_outer_handle_color));
    painter.drawEllipse(get_handle_pos(), outer_radius, outer_radius);
    painter.setBrush(QBrush(_inner_handle_color));
    painter.drawEllipse(get_handle_pos(), inner_radius, inner_radius);
}

void Slider::paint_groove()
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(_groove_add_color));
    auto rect = get_groove_rect();
    if (this->orientation() == Qt::Horizontal) {
        rect.translate(0, (this->height() - _groove_width) / 2);
        rect = QRect(rect.x() + _handle_radius, rect.y(), rect.width() - 2 * _handle_radius, _groove_width);
    } else {
        rect.translate((this->width() - _groove_width) / 2, 0);
        rect = QRect(rect.x(), rect.y() + _handle_radius, _groove_width, rect.height() - 2 * _handle_radius);
    }

    painter.drawRoundedRect(rect, _groove_width / 2, _groove_width / 2);

    auto pos = get_handle_pos();
    painter.setBrush(_gradient);
    if (this->orientation() == Qt::Horizontal) {
        rect.setWidth(pos.x());
    } else {
        rect = QRect(QPoint(rect.x(), pos.y()), QSize(rect.width(), rect.height() - pos.y()));
    }
    painter.drawRoundedRect(rect, _groove_width / 2, _groove_width / 2);
}

void Slider::mousePressEvent(QMouseEvent * event)
{
    QSlider::mousePressEvent(event);
    auto cursor_pos = event->pos();
    if (get_handle_rect().contains(cursor_pos)) {
        _handle_pressed = true;
        return;
    }
    int range = this->maximum() - this->minimum();
    if (this->orientation() == Qt::Horizontal) {
        this->setValue(range * cursor_pos.x() / this->width());
    } else {
        this->setValue(range * (this->height() - cursor_pos.y()) / this->height());
    }
}

void Slider::mouseReleaseEvent(QMouseEvent * event)
{
    _handle_pressed = false;
    QSlider::mouseReleaseEvent(event);
}

void Slider::mouseMoveEvent(QMouseEvent * event)
{
    if (_handle_pressed) {
        QSlider::mouseMoveEvent(event);
    }
    _hovering = get_handle_rect().contains(event->pos());
    this->update();
}

void Slider::paintEvent(QPaintEvent * event)
{
    paint_groove();
    paint_handle();
}

void Slider::resizeEvent(QResizeEvent * event)
{
    QSlider::resizeEvent(event);
    _groove_width = _handle_radius = (this->orientation() == Qt::Horizontal) ? this->height() / 3 : this->width() / 3;
    this->setStyleSheet(QString(R"(
        QSlider::groove:horizontal { border: 1px; }
        QSlider::handle:horizontal { width: %1px; }
        QSlider::groove:vertical { border: 1px; }
        QSlider::handle:vertical { height: %1px; }
    )").arg(2.4 * _handle_radius));
    if (this->orientation() == Qt::Horizontal) {
        _gradient.setFinalStop(this->width(), 0);
    } else {
        _gradient.setFinalStop(0, this->height());
    }
    _gradient.setColorAt(0, QColor(100, 208, 249));
    _gradient.setColorAt(0.5, QColor(255, 255, 255));
    _gradient.setColorAt(1, QColor(249, 180, 255));
}
