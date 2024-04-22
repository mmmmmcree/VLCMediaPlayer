#ifndef LIBVIOSEARCHER_H
#define LIBVIOSEARCHER_H

#include <QObject>
#include <QWebEngineView>
#include <QStandardItemModel>
#include <memory>

class LibvioSearcher : public QObject
{
    Q_OBJECT
public:
    LibvioSearcher(QObject *parent = nullptr);
    void search(const QString &keyword);
    void processModelIndex(const QModelIndex &index);
    QStandardItemModel *model() const;
signals:
    void mediaUrlReady(const QString &url);
private:
    void addEpisode(QStandardItem *parent, const QUrl &url);
    void addMediaUrl(QStandardItem *item, const QUrl &url);
private slots:
    void onSearchPageLoadFinished(QWebEnginePage *page, bool success);
    void onMediaPageLoadFinished(QStandardItem *item, bool success);
private:
    QStandardItemModel *m_model;
    std::unique_ptr<QWebEngineView> m_web_view;
    inline static const QString api_url = "https://www.libvio.pw/";
};

#endif // LIBVIOSEARCHER_H