#include "MusicSearcher.h"
#include "Constants.h"
#include <QNetworkReply>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>

NetEaseMusicSearcher::NetEaseMusicSearcher(QObject *parent)
{
    _model = new QStandardItemModel(this);
    id_manager = new QNetworkAccessManager(this);
    url_manager = new QNetworkAccessManager(this);
    request.setHeader(QNetworkRequest::UserAgentHeader, Network::user_agent);
    connect(id_manager, QNetworkAccessManager::finished, this, onGetMusicIdFinished);
    connect(url_manager, QNetworkAccessManager::finished, this, onGetMusicUrlFinished);
}

void NetEaseMusicSearcher::searchMusic(const QString &keyword)
{
    QUrl url(api_url);
    QUrlQuery query;
    query.addQueryItem("msg", keyword);
    query.addQueryItem("count", "20");
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
    if (reply->error() != QNetworkReply::NoError) {
        QMessageBox::warning(nullptr, "Error", "Failed to get music url.");
        reply->deleteLater();
        return;
    }
    QByteArray response_data = reply->readAll();
    reply->deleteLater();
    QJsonDocument json_doc = QJsonDocument::fromJson(response_data);
    QJsonObject json_obj = json_doc.object();
    MusicInfo music_info = id_info_map[reply_id_map[reply]];
    music_info.music_url = json_obj["song_url"].toString();

    QStandardItem *item = new QStandardItem(music_info.music_name + " - " + music_info.singer_name);
    item->setData(music_info.music_url, Qt::UserRole);
    _model->appendRow(item); 
    
    reply_id_map.remove(reply);
    id_info_map.remove(music_info.id);
}