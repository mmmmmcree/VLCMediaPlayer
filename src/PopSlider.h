#ifndef POPSLIDER_H
#define POPSLIDER_H
#include "HoverFillPushButton.h"
#include "DialogSlider.h"
#include <QSlider>

class PopSlider : public HoverFillPushButton
{
    Q_OBJECT
public:
    PopSlider(QWidget *parent = nullptr);
    QSlider *slider() const;
    bool dialog_visible() const;
protected:
    virtual void enterEvent(QEnterEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void timerEvent(QTimerEvent *event) override;
private:
    DialogSlider *dialog_slider;
};

#endif // POPSLIDER_H
