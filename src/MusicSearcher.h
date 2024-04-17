#ifndef MUSICSEARCHER_H
#define MUSICSEARCHER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QAbstractItemModel>
#include <QStandardItemModel>

struct MusicInfo;

class MusicSearcher : public QObject
{
    Q_OBJECT
public:
    virtual ~MusicSearcher() = default;
    virtual void searchMusic(const QString &keyword) = 0;
    QStandardItemModel *model() const { return _model; }
protected:
    QStandardItemModel *_model;
};

class NetEaseMusicSearcher : public MusicSearcher
{
    Q_OBJECT
public:
    NetEaseMusicSearcher(QObject *parent = nullptr);
    void searchMusic(const QString &keyword) override;
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