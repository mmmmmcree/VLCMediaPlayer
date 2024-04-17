// 自定义按钮类头文件
#ifndef STREAMER_H
#define STREAMER_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

template <typename T>
class Streamer : public T {
public:
    Streamer(QWidget *parent = nullptr);
    void setStreamerOrientation(Qt::Orientation orientation);
    void setStartColor(const QColor &color);
    void setEndColor(const QColor &color);
    void setStreamerSpeed(int speed);
    QLinearGradient getGradient() const;
    void hide();
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void leaveEvent(QEvent *event) override;
private slots:
    void updateGradientShift();
private:
    Qt::Orientation _orientation = Qt::Horizontal;
    QTimer *_timer;
    int _gradient_shift = 0, _step = 10;
    QColor _start_color{172, 199, 245, 150}, _end_color{245, 172, 231, 150};
    bool hovering = false;
};

template <typename T>
Streamer<T>::Streamer(QWidget *parent) : T(parent)
{
    _timer = new QTimer(this);
    _timer->setInterval(20);
    T::connect(_timer, QTimer::timeout, this, updateGradientShift);
    _timer->start();
}

template <typename T>
inline void Streamer<T>::setStreamerOrientation(Qt::Orientation orientation)
{
    _orientation = orientation;
}

template <typename T>
void Streamer<T>::setStartColor(const QColor &color)
{
    _start_color = color;
}

template <typename T>
void Streamer<T>::setEndColor(const QColor &color)
{
    _end_color = color;
}

template <typename T>
inline void Streamer<T>::setStreamerSpeed(int speed)
{
    _step = speed;
}

template <typename T>
QLinearGradient Streamer<T>::getGradient() const
{
    QList<QColor> colors{_start_color, _end_color, _start_color};
    QLinearGradient gradient;
    for (int i = 0, n = colors.size(); i < n; ++i) {
        qreal pos = 0.5 * i / (n - 1);
        gradient.setColorAt(pos, colors[i]);
        gradient.setColorAt(pos + 0.5, colors[i]);
    }
    if (_orientation == Qt::Horizontal) {
        gradient.setStart(-this->width() + _gradient_shift, this->height());
        gradient.setFinalStop(this->width() + _gradient_shift, this->height());
    } else {
        gradient.setStart(this->width(), -this->height() + _gradient_shift);
        gradient.setFinalStop(this->width(), this->height() + _gradient_shift);
    }
    return gradient;
}

template <typename T>
inline void Streamer<T>::hide()
{
    if (not hovering) { T::hide(); }
}

template <typename T>
void Streamer<T>::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setBrush(getGradient());
    painter.setPen(Qt::NoPen);
    painter.drawRect(this->rect());
    T::paintEvent(event);
}

template <typename T>
inline void Streamer<T>::hideEvent(QHideEvent *event)
{
    _timer->stop();
    T::hideEvent(event);
}

template <typename T>
inline void Streamer<T>::showEvent(QShowEvent *event)
{
    _timer->start();
    T::showEvent(event);
}

template <typename T>
inline void Streamer<T>::enterEvent(QEnterEvent *event)
{
    hovering = true;
    T::enterEvent(event);
}

template <typename T>
inline void Streamer<T>::leaveEvent(QEvent *event)
{
    hovering = false;
    T::leaveEvent(event);
}

template <typename T>
void Streamer<T>::updateGradientShift()
{
    _gradient_shift = (_gradient_shift + _step) % (_orientation == Qt::Horizontal? this->width() : this->height());
    this->update();
}


#endif // SHINYBUTTON_H
