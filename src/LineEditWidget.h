#ifndef LINEEDITWIDGET_H
#define LINEEDITWIDGET_H

#include "LineEdit.h"
#include <QWidget>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>

class LineEditWidget : public QWidget
{
    Q_OBJECT
public:
    LineEditWidget(QWidget *parent = nullptr);
    LineEdit *lineEdit() const;
    QLabel *tipLabel() const;
    void setlineEditBorderColor(const QColor &color);
    void setLineEditBackgroundColor(const QColor &color);
    void setTipLabelColor(const QColor &color);
    QPushButton *pushButton() const;
private:
    void setAnimationProperties();
protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
private:
    LineEdit *edit;
    QLabel *tip;
    QPropertyAnimation *edit_animation, *tip_animation;
    QPushButton *button;
    int animation_duration = 200;
    bool hovering = false;
};

#endif