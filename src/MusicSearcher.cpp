#include "MusicSearcher.h"
#include "Constants.h"
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>

NetEaseMusicSearcher::NetEaseMusicSearcher(QObject *parent) : MediaSearcher(parent)
{
    id_manager = new QNetworkAccessManager(this);
    url_manager = new QNetworkAccessManager(this);
    request.setHeader(QNetworkRequest::UserAgentHeader, Network::user_agent);
    connect(id_manager, &QNetworkAccessManager::finished, this, &NetEaseMusicSearcher::onGetMusicIdFinished);
    connect(url_manager, &QNetworkAccessManager::finished, this, &NetEaseMusicSearcher::onGetMusicUrlFinished);
}

void NetEaseMusicSearcher::search(const QString &keyword)
{
    QUrl url(api_url);
    QUrlQuery query;
    query.addQueryItem("msg", keyword);
    query.addQueryItem("count", "30");
    url.setQuery(query);
    request.setUrl(url);
    id_manager->get(request);
}

void NetEaseMusicSearcher::searchMusicUrl(int id)
{
    QUrl url(api_url);
    QUrlQuery query;
    query.addQueryItem("type", "songid");
    query.addQueryItem("id", QString::number(id));
    url.setQuery(query);
    request.setUrl(url);
    QNetworkReply *reply = url_manager->get(request);
    reply_id_map[reply] = id;
}

void NetEaseMusicSearcher::onGetMusicIdFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(nullptr, "Error", "Failed to get song id.");
        reply->deleteLater();
        return;
    }
    QByteArray response_data = reply->readAll();
    reply->deleteLater();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonObject json_obj = json_doc.object();
    for (const auto &data : json_obj["data"].toArray()) {
        QJsonObject music_info_obj = data.toObject();
        int id = music_info_obj["id"].toInt();
        id_info_map.insert(id, MusicInfo(id, music_info_obj["name"].toString(), music_info_obj["singername"].toString()));
        searchMusicUrl(id);
    }
}

void NetEaseMusicSearcher::onGetMusicUrlFinished(QNetworkReply *reply)
{
    MusicInfo music_info = id_info_map[reply_id_map[reply]];
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response_data = reply->readAll();
        QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
        QJsonObject json_obj = json_doc.object();
        music_info.music_url = json_obj["song_url"].toString();
        if (not music_info.music_url.isEmpty()) {
            QStandardItem *item = new QStandardItem(music_info.music_name + " - " + music_info.singer_name);
            item->setData(music_info.music_url, Qt::UserRole);
            this->model()->appendRow(item); 
        }
    }
    reply_id_map.remove(reply);
    id_info_map.remove(music_info.id);
    reply->deleteLater();
}

KugouMusicSearcher::KugouMusicSearcher(QObject *parent)
{
    manager = new QNetworkAccessManager(this);
    request.setHeader(QNetworkRequest::UserAgentHeader, Network::user_agent);
    connect(manager, &QNetworkAccessManager::finished, this, &KugouMusicSearcher::onGetMusicUrlFinished);
}

void KugouMusicSearcher::search(const QString &keyword)
{
    QUrl url(api_url);
    QUrlQuery query;
    query.addQueryItem("msg", keyword);
    int n = 30;
    query.addQueryItem("count", QString::number(n));
    for (int i = 0; i < n; ++i) {
        QUrlQuery item_query = query;
        item_query.addQueryItem("n", QString::number(i));
        url.setQuery(item_query);
        request.setUrl(url);
        manager->get(request);
    }
}

void KugouMusicSearcher::onGetMusicUrlFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response_data = reply->readAll();
        QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
        QJsonObject json_obj = json_doc.object();
        QJsonObject data = json_obj["data"].toObject();
        QString music_name = data["name"].toString(), music_url = data["song_url"].toString();
        if (not music_url.isEmpty() and music_url.startsWith("http")) {
            QStandardItem *item = new QStandardItem(music_name);
            item->setData(music_url, Qt::UserRole);
            this->model()->appendRow(item); 
        }
        QString mv_url = data["mv_url"].toString();
        if (not mv_url.isEmpty() and mv_url.startsWith("http")) {
            QStandardItem *item = new QStandardItem(music_name + " MV");
            item->setData(mv_url, Qt::UserRole);
            this->model()->appendRow(item); 
        }
    }
    reply->deleteLater();
}
