#include "LineEditWidget.h"
#include <QLayout>
#include <QPainter>
#include <QTimer>
#include <QKeyEvent>

LineEditWidget::LineEditWidget(QWidget *parent) : QWidget(parent)
{


    edit = new LineEdit(this);
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    tip = new QLabel(this);
    tip->setAlignment(Qt::AlignCenter);
    auto tip_font = tip->font();
    tip_font.setPixelSize(edit->height() / 2);
    tip->setFont(tip_font);

    edit_animation = new QPropertyAnimation(edit, "geometry", this);
    edit_animation->setDuration(animation_duration);
    tip_animation = new QPropertyAnimation(tip, "pos", this);
    tip_animation->setDuration(animation_duration);

    button = new QPushButton(this);
    button->setFlat(true);
    button->hide();
    auto hlayout = new QHBoxLayout(this);
    hlayout->setSpacing(0);
    hlayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    hlayout->addWidget(button);
    this->setLayout(hlayout);

    setAnimationProperties();
}

LineEdit *LineEditWidget::lineEdit() const
{
    return edit;
}

QLabel *LineEditWidget::tipLabel() const
{
    return tip;
}

void LineEditWidget::setlineEditBorderColor(const QColor &color)
{
    edit->setBorderColor(color);
}

void LineEditWidget::setLineEditBackgroundColor(const QColor &color)
{
    edit->setBackgroundColor(color);
}

void LineEditWidget::setTipLabelColor(const QColor &color)
{
    QPalette palette = tip->palette();
    palette.setColor(QPalette::WindowText, color);
    tip->setPalette(palette);
}

QPushButton *LineEditWidget::pushButton() const
{
    return button;
}

void LineEditWidget::setAnimationProperties()
{
    int edit_y = this->height() - edit->borderWidth();
    edit_animation->setStartValue(QRect(QPoint(0, edit_y), QSize(this->width(), edit->borderWidth())));
    edit_animation->setEndValue(QRect(QPoint(0, 0), QSize(this->width(), this->height())));
    edit->setGeometry(edit_animation->startValue().toRect());
    int label_y = (this->height() - tip->height()) / 1.5;
    tip_animation->setStartValue(QPoint(0, label_y));
    tip_animation->setEndValue(QPoint(0, 0));
    tip->move(tip_animation->startValue().toPoint());
}

void LineEditWidget::enterEvent(QEnterEvent *event)
{
    QWidget::enterEvent(event);
    hovering = true;
    edit_animation->setDirection(QAbstractAnimation::Forward);
    tip->hide();
    edit_animation->start();
    QTimer::singleShot(animation_duration, [this]() { if (hovering) { button->show();} });
}

void LineEditWidget::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    hovering = false;
    button->hide();
    tip->show();
    edit_animation->setDirection(QAbstractAnimation::Backward);
    tip_animation->setDirection(QAbstractAnimation::Backward);
    edit_animation->start();
    tip_animation->start();
    QTimer::singleShot(animation_duration, [this]() { button->hide(); });
}

void LineEditWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    tip->setFixedWidth(this->width());
    button->setFixedSize(this->height() / 2, this->height() / 2);
    setAnimationProperties();
}

void LineEditWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter or event->key() == Qt::Key_Return) {
        button->click();
    }
    QWidget::keyPressEvent(event);
}
