#ifndef MUSICSEARCHER_H
#define MUSICSEARCHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include "MediaSearcher.h"

struct MusicInfo;

class NetEaseMusicSearcher : public MediaSearcher
{
    Q_OBJECT
public:
    NetEaseMusicSearcher(QObject *parent = nullptr);
    void search(const QString &keyword) override;
private:
    void searchMusicUrl(int id);
private slots:
    void onGetMusicIdFinished(QNetworkReply *reply);
    void onGetMusicUrlFinished(QNetworkReply *reply);
private:
    QNetworkAccessManager *id_manager, *url_manager;
    QNetworkRequest request;
    QHash<QNetworkReply*, int> reply_id_map;
    QHash<int, MusicInfo> id_info_map;
    static constexpr auto  api_url = "https://dataiqs.com/api/netease/music/";
};

class KugouMusicSearcher : public MediaSearcher
{
    Q_OBJECT
public:
    KugouMusicSearcher(QObject *parent = nullptr);
    void search(const QString &keyword) override;
private slots:
    void onGetMusicUrlFinished(QNetworkReply *reply);
private:
    QNetworkAccessManager *manager;
    QNetworkRequest request;
    static constexpr auto  api_url = "https://dataiqs.com/api/kgmusic/";
};

struct MusicInfo
{
    friend class NetEaseMusicSearcher;
    MusicInfo() = default;
    MusicInfo(int i, const QString &mn, const QString &sn, const QString &mu = "")
        : id(i), music_name(mn), singer_name(sn), music_url(mu) { }
private:
    int id;
    QString music_name, singer_name, music_url;
};

#endif // MUSICSEARCHER_H