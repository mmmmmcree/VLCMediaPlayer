#ifndef ANIMATEDSHOW_H
#define ANIMATEDSHOW_H
#include <QWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QResizeEvent>

enum ShowType {
    BottomToTop,
    TopToBottom,
    LeftToRight,
    RightToLeft
};

template <typename T>
class AnimatedShow : public T
{
public:
    AnimatedShow(QWidget *parent = nullptr);
    void setShowType(ShowType type);
    void setShowDuration(int duration);
    void hide();
protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;
private:
    void setAnimationProperties();
private:
    QPropertyAnimation *show_animation;
    QWidget *mask;
    ShowType show_type = BottomToTop;
    int show_duration = 900;
};

template <typename T>
AnimatedShow<T>::AnimatedShow(QWidget *parent) : T(parent)
{
    mask = new QWidget(this);
    mask->setStyleSheet("background-color: white;");
    show_animation = new QPropertyAnimation(mask, "geometry", this);
    show_animation->setEasingCurve(QEasingCurve::InOutQuad);
    show_animation->setDuration(show_duration);
    T::connect(show_animation, &QAbstractAnimation::finished, [this]() {
        if (show_animation->direction() == QAbstractAnimation::Backward) { T::hide(); }
    });
}

template <typename T>
void AnimatedShow<T>::setShowType(ShowType type)
{
    show_type = type;
}

template <typename T>
void AnimatedShow<T>::setShowDuration(int duration)
{
    show_animation->setDuration(duration);
    show_duration = duration;
}

template <typename T>
void AnimatedShow<T>::hide()
{
    show_animation->setDirection(QAbstractAnimation::Backward);
    show_animation->start();
}

template <typename T>
void AnimatedShow<T>::resizeEvent(QResizeEvent *event)
{
    T::resizeEvent(event);
    setAnimationProperties();
}

template <typename T>
void AnimatedShow<T>::showEvent(QShowEvent *event)
{
    mask->setGeometry(show_animation->startValue().toRect());
    T::showEvent(event);
    show_animation->setDirection(QAbstractAnimation::Forward);
    show_animation->start();
}

template <typename T>
void AnimatedShow<T>::setAnimationProperties()
{
    show_animation->setStartValue(QRect(0, 0, this->width(), this->height()));
    QRect end_rect;
    switch (show_type) {
        case BottomToTop: { end_rect = QRect(0, 0, this->width(), 0); } break;
        case TopToBottom: { end_rect = QRect(0, this->height(), this->width(), 0); } break;
        case LeftToRight: { end_rect = QRect(this->width(), 0, 0, this->height()); } break;
        case RightToLeft: { end_rect = QRect(0, 0, 0, this->height()); } break;
    }
    show_animation->setEndValue(end_rect);
}

#endif // ANIMATEDSHOW_H