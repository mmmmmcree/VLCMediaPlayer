#ifndef LINEEDIT_H
#define LINEEDIT_H
#include <QLineEdit>
#include <QColor>

class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    LineEdit(QWidget *parent = nullptr);
    void setBorderColor(const QColor &color);
    void setBackgroundColor(const QColor &color);
    int borderWidth() const;
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    QColor border_color{57, 200, 135}, background_color{202, 156, 231, 100};
    int border_width = 2;
};

#endif // LINEEDIT_H