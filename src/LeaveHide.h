#ifndef LEAVEHIDE_H
#define LEAVEHIDE_H
template <typename T>
class LeaveHide : public T
{
public:
    LeaveHide(QWidget *parent = nullptr) : T(parent) { }
    void hide() {
        if (not hovering) { T::hide(); }
    }
protected:
    void enterEvent(QEnterEvent *event) override
    {
        hovering = true;
        T::enterEvent(event);
    }
    void leaveEvent(QEvent *event) override
    {
        hovering = false;
        T::leaveEvent(event);
    }
private:
    bool hovering = false;
};
#endif // TABWIDGET_H