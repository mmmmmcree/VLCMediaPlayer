#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H
#include <QObject>
#include <QNetworkAccessManager>


class FileDownloader : public QObject
{
    Q_OBJECT
public:
    FileDownloader(QObject *parent = nullptr);
    void download(const QString &url, const QString &file_name = "");
signals:
    void downloadFinished(const QString &file_path);
private slots:
    void onDownloadFinished(QNetworkReply *reply);
private:
    QNetworkAccessManager *manager;
    QHash<QNetworkReply *, QPair<QString, QString>> map;
};

#endif // FILEDOWNLOADER_H