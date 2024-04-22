#include "LibvioSearcher.h"
#include "Constants.h"
#include <QWebEnginePage>
#include <QUrl>
#include <QUrlQuery>
#include <QMessageBox>
#include <QStandardItem>

LibvioSearcher::LibvioSearcher(QObject *parent) : QObject(parent)
{
    m_web_view = std::make_unique<QWebEngineView>();
    m_web_view->hide();
    m_web_view->setUrl(QUrl("about:blank"));
    m_model = new QStandardItemModel(this);
}

void LibvioSearcher::search(const QString &keyword)
{
    QUrl url(api_url + "/search/-------------.html");
    QUrlQuery query;
    query.addQueryItem("wd", keyword);
    url.setQuery(query);
    QWebEnginePage *page = new QWebEnginePage(this);
    connect(page, &QWebEnginePage::loadFinished, this, [this, page](bool success) {
        onSearchPageLoadFinished(page, success);
    });
    page->load(url);
}

int treeDepth(QStandardItem *item) {
    int depth = 0;
    while (item->parent()) {
        item = item->parent();
        ++depth;
    }
    return depth;
};

void LibvioSearcher::processModelIndex(const QModelIndex &index)
{
    if (not index.isValid()) { return; }
    QStandardItem *item = m_model->itemFromIndex(index);
    QUrl url = item->data(Qt::UserRole).toUrl();
    if (url.isEmpty()) { return; }
    switch(treeDepth(item)) {
        case 0: {
            item->setData(QString(), Qt::UserRole);
            addEpisode(item, url);
        } break;
        case 1: {
            addMediaUrl(item, url);
        } break;
    }
}

QStandardItemModel *LibvioSearcher::model() const
{
    return m_model;
}

void LibvioSearcher::addEpisode(QStandardItem *parent, const QUrl &url)
{
    QWebEnginePage *page = new QWebEnginePage(this);
    connect(page, &QWebEnginePage::loadFinished, this, [page, parent](bool success) mutable {
        if (not success) { QMessageBox::warning(nullptr, "Warning", "Failed to load episode page."); return; }
        page->runJavaScript(JavaScript::get_episode_links, [page, parent](const QVariant &result) mutable {
            auto links = result.toList();
            for (int i = 0; i < links.size(); ++i) {
                QStandardItem *item = new QStandardItem(QString::number(i + 1));
                item->setData(links[i].toString(), Qt::UserRole);
                parent->appendRow(item);
            }
            delete page; page = nullptr;
        });
    });
    page->load(url);
}

void LibvioSearcher::addMediaUrl(QStandardItem *item, const QUrl &url)
{
    QString media_url = item->data(Qt::UserRole + 1).toString();
    if (not media_url.isEmpty()) { emit mediaUrlReady(media_url); return; }
    m_web_view->setUrl(url);
    connect(m_web_view->page(), &QWebEnginePage::loadFinished, this, [this, item](bool success) {
        onMediaPageLoadFinished(item, success);
    });
}

void LibvioSearcher::onMediaPageLoadFinished(QStandardItem *item, bool success)
{
    if (not success) { QMessageBox::warning(nullptr, "Warning", "Failed to load search page."); return; }
    m_web_view->page()->runJavaScript(JavaScript::get_media_url, [this, item](const QVariant &result) {
        QString media_url = result.toString();
        emit mediaUrlReady(media_url);
        item->setData(media_url, Qt::UserRole + 1);
        m_web_view->setPage(nullptr);
    });
}

void LibvioSearcher::onSearchPageLoadFinished(QWebEnginePage *page, bool success)
{
    if (not success) { QMessageBox::warning(nullptr, "Warning", "Failed to load search page."); return; }
    page->runJavaScript(JavaScript::get_search_results, [this, page](const QVariant &result) mutable {
        for (const QVariant &item : result.toList()) {
            QVariantMap map = item.toMap();
            QString href = map["href"].toString(), title = map["title"].toString();
            QStandardItem *item = new QStandardItem(title);
            item->setData(QUrl(href), Qt::UserRole);
            m_model->appendRow(item);
        }
        delete page; page = nullptr;
    });
}