#ifndef HOVERFILL_H
#define HOVERFILL_H

#include <QTimer>
#include <QPoint>
#include <QCursor>
#include <QPainterPath>
#include <QPainter>
#include <QBrush>
#include <QGraphicsDropShadowEffect>
#include <QEnterEvent>
#include <QEvent>
#include <cmath>

template <typename T>
class HoverFill : public T
{
public:
    enum FillType {CircularFill, CrossFill};
    enum Shape { Rect, RoundedRect, Ellipse };
    HoverFill(QWidget *parent = nullptr, Shape s = RoundedRect, FillType f = CircularFill);
    void setShape(Shape shape);
    void setFillType(FillType fill_type);
    void addShadow(int right, int down, qreal blur_radius, const QColor &shadow_color);
    void setFillSpeed(int fill_speed);
    void setFillBrush(const QBrush &brush);
private slots:
    void radiusIncrease();
    void radiusDecrease();
protected:
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;
private:
    void init();
private:
    FillType _fill_type = CircularFill;
    Shape _shape = RoundedRect;
    int _max_radius, _radius, _step = 1;
    QPointF _cursor_pos;
    QBrush _brush{QColor(255, 124, 118, 50)};
    QPainterPath _path;
    QTimer *_timer;
};

template <typename T>
HoverFill<T>::HoverFill(QWidget *parent, Shape s, FillType f)
: T(parent), _shape(s), _fill_type(f)
{
    this->setStyleSheet("background-color: transparent;");
    this->addShadow(0, 0, 100, QColor(255, 150, 150));
    init();
    _timer = new QTimer(this);
    _timer->setInterval(2);
}

template <typename T>
inline void HoverFill<T>::setShape(Shape shape)
{
    _shape = shape;
}

template <typename T>
inline void HoverFill<T>::setFillType(FillType fill_type)
{
    _fill_type = fill_type;
}

template <typename T>
void HoverFill<T>::addShadow(int right, int down, qreal blur_radius, const QColor &shadow_color)
{
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(right, down);    // 阴影的偏移量（右，下）
    shadow->setColor(shadow_color);     // 阴影的颜色
    shadow->setBlurRadius(blur_radius); // 控制阴影的模糊程度（光源距离）
    this->setGraphicsEffect(shadow);
}

template <typename T>
void HoverFill<T>::setFillSpeed(int fill_speed)
{
    _step = fill_speed;
}

template <typename T>
void HoverFill<T>::setFillBrush(const QBrush &brush)
{
    _brush = brush;
}

template <typename T>
void HoverFill<T>::radiusIncrease()
{
    _radius += _step;
    if (_radius >= _max_radius) {
        _radius = _max_radius;
        _timer->stop();
    }
    this->update();
}

template <typename T>
void HoverFill<T>::radiusDecrease()
{
    _radius -= _step;
    if (_radius <= 0) {
        _radius = 0;
        _timer->stop();
    }
    this->update();
}

template <typename T>
void HoverFill<T>::enterEvent(QEnterEvent *event)
{
    T::enterEvent(event);
    _timer->disconnect();
    T::connect(_timer, QTimer::timeout, this, radiusIncrease);
    _cursor_pos = event->position();
    _timer->start();
}

template <typename T>
void HoverFill<T>::leaveEvent(QEvent *event)
{
    T::leaveEvent(event);
    _timer->disconnect();
    T::connect(_timer, QTimer::timeout, this, radiusDecrease);
    _cursor_pos = this->mapFromGlobal(QCursor::pos());
    _timer->start();
}

template <typename T>
void HoverFill<T>::paintEvent(QPaintEvent *event)
{
    T::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setClipPath(_path);
    painter.setBrush(_brush);
    switch(_fill_type) {
        case CircularFill: {
            if (not _cursor_pos.isNull()) { painter.drawEllipse(_cursor_pos, _radius, _radius); }
        } break;
        case CrossFill: {
            painter.drawEllipse({0, 0}, _radius, _radius);
            painter.drawEllipse({this->width(), this->height()}, _radius, _radius);
        } break;
    }
    T::paintEvent(event);
}

template <typename T>
void HoverFill<T>::resizeEvent(QResizeEvent *event)
{
    T::resizeEvent(event);
    init();
}

template <typename T>
inline void HoverFill<T>::hideEvent(QHideEvent *event)
{
    _timer->stop();
    T::hideEvent(event);
}

template <typename T>
void HoverFill<T>::init()
{
    _max_radius = sqrt(pow(this->width(), 2) + pow(this->height(), 2));
    _path.clear();
    switch(_shape) {
        case Rect: _path.addRect(this->rect()); break;
        case RoundedRect: {
            int radius = std::min(this->width(), this->height() / 3);
            _path.addRoundedRect(this->rect(), radius, radius);
        } break;
        case Ellipse: _path.addEllipse(this->rect()); break;
    }
}

#endif // HOVERFILL_H
