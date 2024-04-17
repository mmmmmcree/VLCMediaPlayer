#include "LineEdit.h"
#include <QPalette>
#include <QPainter>

LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent)
{
    this->setStyleSheet(QString(
        "background-color: transparent; border: none; margin-left: %1px; margin-right: %1px"
    ).arg(this->height() / 2));
    this->setFont(QFont("Microsoft YaHei"));
    QPalette palette = this->palette();
    palette.setColor(QPalette::Text, border_color);
    this->setPalette(palette);
}

void LineEdit::paintEvent(QPaintEvent *event)
{
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(border_color, border_width));
        painter.setBrush(QBrush(background_color));
        painter.drawRoundedRect(this->rect().adjusted(border_width, border_width, -border_width, -border_width), this->height() / 2, this->height() / 2);
        QLineEdit::paintEvent(event);
}

void LineEdit::resizeEvent(QResizeEvent *event)
{
    auto font = this->font();
    font.setPixelSize(this->height() / 3);
    this->setFont(font);
    QLineEdit::resizeEvent(event);
}

void LineEdit::setBorderColor(const QColor &color)
{
    border_color = color;
}

void LineEdit::setBackgroundColor(const QColor &color)
{
    background_color = color;
}

int LineEdit::borderWidth() const
{
    return border_width;
}
