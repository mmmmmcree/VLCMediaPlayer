#ifndef TEMPLATEVIEW_H
#define TEMPLATEVIEW_H
#include <QWidget>
#include <QMouseEvent>

template <typename T>
class TemplateView : public T
{
public:
    TemplateView(QWidget *parent = nullptr);
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
};

template <typename T>
TemplateView<T>::TemplateView(QWidget *parent) : T(parent)
{
    QFont font("Microsoft YaHei", 10); 
    this->setFont(font);
    this->setStyleSheet("QAbstractItemView { border: none; background-color: transparent; }");
}

#include <QMessageBox>
template <typename T>
void TemplateView<T>::mousePressEvent(QMouseEvent *event)
{
    QModelIndex index = this->indexAt(event->pos());
    if (index.isValid()) {
        T::mousePressEvent(event);
    } else {
        this->clearSelection();
    }
}

template <typename T>
inline void TemplateView<T>::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (this->selectionModel()->hasSelection()) {
        emit T::doubleClicked(this->currentIndex());
    }
}

#endif // TEMPLATEVIEW_H