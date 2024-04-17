#ifndef SLIDER_H
#define SLIDER_H

#include <QSlider>
#include <QColor>
#include <QPainter>

class Slider : public QSlider
{
    Q_OBJECT
public:
    Slider(QWidget *parent = nullptr);
    Slider(Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = nullptr);
    void setInnerHandleColor(const QColor &color);
    void setOuterHandleColor(const QColor &color);
    void setGrooveAddColor(const QColor &color);
    void setLinearGradientColorAt(qreal pos, const QColor &color);
protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
private:
    QPoint get_handle_pos() const;
    QRect get_handle_rect() const;
    QRect get_groove_rect() const;
    void paint_handle();
    void paint_groove();
private:
    QColor _groove_add_color{255, 255, 255};
    QLinearGradient _gradient;
    QColor _inner_handle_color{148, 59, 142};
    QColor _outer_handle_color{250, 157, 182};
    int _handle_radius = 10, _groove_width;
    bool _handle_pressed = false, _hovering = false;
};

#endif // SLIDER_H
