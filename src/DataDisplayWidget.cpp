#include <QLayout>
#include <QContextMenuEvent>
#include <QMenu>
#include "DataDisplayWidget.h"


DataDisplayWidget::DataDisplayWidget(QWidget *parent) : QWidget(parent)
{
    menu = new QMenu(this);
    on_clicked_menu =  new QMenu(this);
    top_layout = new QHBoxLayout;
    layout = new QVBoxLayout(this);
    layout->addLayout(top_layout);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setStretch(0, 1);

    QAction *add_action = menu->addAction("add");
    on_clicked_menu->addAction(add_action);
    QAction *delete_action = on_clicked_menu->addAction("delete");
    QAction *rename_action = on_clicked_menu->addAction("rename");

    connect(add_action, QAction::triggered, this, addItem);
    connect(delete_action, QAction::triggered, this, deleteAction_triggered);
    connect(rename_action, QAction::triggered, this, renameAction_triggered);
}

void DataDisplayWidget::setView(QAbstractItemView *item_view)
{
    if (view) { layout->removeWidget(view); }
    view = item_view;
    layout->addWidget(view);
}

QHBoxLayout *DataDisplayWidget::topLayout() const
{
    return top_layout;
}

void DataDisplayWidget::selectNextItem(int step)
{
    if (not view) { return; }
    QModelIndex index = view->currentIndex();
    if (index.isValid()) {
        int next_row = (index.row() + step + view->model()->rowCount()) % view->model()->rowCount();
        QModelIndex next_index = view->model()->index(next_row, index.column());
        view->setCurrentIndex(next_index);
    }
}

void DataDisplayWidget::selectPreviousItem()
{
    selectNextItem(-1);
}

void DataDisplayWidget::deleteAction_triggered()
{
    if (not view or not view->selectionModel()->hasSelection()) { return; }
    view->model()->removeRow(view->currentIndex().row());
    emit deleteItem();
}

void DataDisplayWidget::renameAction_triggered()
{
    if (not view) { return; }
    QModelIndexList indexes = view->selectionModel()->selectedIndexes();
    if (not indexes.isEmpty()) {
        view->edit(indexes.first()); 
    }
}

void DataDisplayWidget::contextMenuEvent(QContextMenuEvent *event)
{
    if (not view) { return; }
    QModelIndex index = view->indexAt(view->mapFromGlobal(event->globalPos()));
    if (index.isValid()) {
        view->setCurrentIndex(index);
        on_clicked_menu->exec(event->globalPos());
    } else {
        view->clearSelection();
        menu->exec(event->globalPos());
    }
}
