#include <QLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include "DataDisplayWidget.h"


DataDisplayWidget::DataDisplayWidget(QWidget *parent) : QWidget(parent)
{
    _menu = new QMenu(this);
    on_clicked_menu =  new QMenu(this);
    top_layout = new QHBoxLayout;
    layout = new QVBoxLayout(this);
    layout->addLayout(top_layout);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setStretch(0, 1);

    QAction *add_action = _menu->addAction("add");
    on_clicked_menu->addAction(add_action);
    QAction *delete_action = on_clicked_menu->addAction("delete");
    QAction *rename_action = on_clicked_menu->addAction("rename");

    _menu->installEventFilter(this);
    on_clicked_menu->installEventFilter(this);

    connect(add_action, &QAction::triggered, this, &DataDisplayWidget::addItem);
    connect(delete_action, &QAction::triggered, this, &DataDisplayWidget::deleteAction_triggered);
    connect(rename_action, &QAction::triggered, this, &DataDisplayWidget::renameAction_triggered);
}

void DataDisplayWidget::setView(QAbstractItemView *item_view)
{
    if (_view) { layout->removeWidget(_view); }
    _view = item_view;
    layout->addWidget(_view);
}

QAbstractItemView *DataDisplayWidget::view() const
{
    return _view;
}

QHBoxLayout *DataDisplayWidget::topLayout() const
{
    return top_layout;
}

QMenu *DataDisplayWidget::onClickedMenu() const
{
    return on_clicked_menu;
}

bool DataDisplayWidget::menuVisible() const
{
    return _menu->isVisible() or on_clicked_menu->isVisible();
}

QModelIndex DataDisplayWidget::selectNextModelIndex(int step) const
{
    //todo 树形view需要不同的处理
    if (not _view) { return QModelIndex(); }
    QModelIndex index = _view->currentIndex();
    if (not index.isValid()) { return QModelIndex(); }
    int next_row = (index.row() + step + rowCount()) % rowCount();
    QModelIndex next_index = _view->model()->index(next_row, index.column());
    _view->setCurrentIndex(next_index);
    return next_index;
}

int DataDisplayWidget::rowCount() const
{
    return _view->model()->rowCount();
}

int DataDisplayWidget::currentRow() const
{
    return _view->currentIndex().row();
}

void DataDisplayWidget::deleteAction_triggered()
{
    if (not _view or not _view->selectionModel()->hasSelection()) { return; }
    _view->model()->removeRow(_view->currentIndex().row());
    emit deleteItem();
}

void DataDisplayWidget::renameAction_triggered()
{
    if (not _view) { return; }
    QModelIndexList indexes = _view->selectionModel()->selectedIndexes();
    if (not indexes.isEmpty()) {
        _view->edit(indexes.first()); 
    }
}
#include <QApplication>
void DataDisplayWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (not _view) { return; }
    QModelIndex index = _view->indexAt(_view->mapFromGlobal(event->globalPos()));
    if (index.isValid()) {
        _view->setCurrentIndex(index);
        on_clicked_menu->exec(event->globalPos());
    } else {
        _view->clearSelection();
        _menu->exec(event->globalPos());
    }
}