#ifndef DATADISPLAYWIDGET_H
#define DATADISPLAYWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QHBoxLayout>
#include <QAbstractItemModel>
#include <QAbstractItemView>

class DataDisplayWidget : public QWidget
{
    Q_OBJECT
public:
    DataDisplayWidget(QWidget *parent = nullptr);
    void setView(QAbstractItemView *item_view);
    QAbstractItemView *view() const;
    QHBoxLayout *topLayout() const;
    QMenu *onClickedMenu() const;
    bool menuVisible() const;
    QModelIndex selectNextModelIndex(int step = 1) const;
    int rowCount() const;
    int currentRow() const;
signals:
    void addItem();
    void deleteItem();
public slots:
    void deleteAction_triggered();
    void renameAction_triggered();
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
private:
    QAbstractItemView *_view;
    QMenu *_menu, *on_clicked_menu;
    QHBoxLayout *top_layout;
    QVBoxLayout *layout;
};

#endif // LISTWIDGET_H