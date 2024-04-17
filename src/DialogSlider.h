#ifndef DIALOGSLIDER_H
#define DIALOGSLIDER_H
#include <QDialog>
#include "Slider.h"

class DialogSlider : public QDialog
{
    Q_OBJECT
public:
    DialogSlider(QWidget *parent = nullptr);
    QSlider *slider() const;
protected:
    virtual bool event(QEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
private:
    Slider *_slider;
};

#endif