#ifndef MEDIASEARCHER_H
#define MEDIASEARCHER_H
#include <QObject>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QModelIndex>
#include <memory>

class MediaSearcher : public QObject
{
public:
    MediaSearcher(QObject *parent = nullptr);
    virtual ~MediaSearcher() = default;
    virtual void search(const QString &keyword) = 0;
    QStandardItemModel *model() const;
    QStandardItem *item(const QModelIndex& index) const;
private:
    inline static std::unique_ptr<QStandardItemModel> _model;
};

#endif // MEDIASEARCHER_H