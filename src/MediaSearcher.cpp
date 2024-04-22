#include "MediaSearcher.h"

MediaSearcher::MediaSearcher(QObject *parent) : QObject(parent)
{
    if (not _model) { _model = std::make_unique<QStandardItemModel>(); }
}

QStandardItemModel *MediaSearcher::model() const
{
    return _model.get();
}

QStandardItem *MediaSearcher::item(const QModelIndex &index) const
{
    return _model->itemFromIndex(index);
}