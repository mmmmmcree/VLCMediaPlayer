#include "FileDownloader.h"
#include <QNetworkReply>
#include <QFileDialog>
#include <QFile>

FileDownloader::FileDownloader(QObject * parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &FileDownloader::onDownloadFinished);
}

void FileDownloader::download(const QString &url, const QString &file_name)
{
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    map.insert(reply, {url, file_name});
}

void FileDownloader::onDownloadFinished(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        const auto &[url, file_name] = map[reply];
        QString suffix = url.split(".").last();
        QString file_path = QFileDialog::getSaveFileName(nullptr, tr("Download File"), file_name, QString("(*.%1)").arg(suffix));
        if (QFile file(file_path); file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            emit downloadFinished(file_path);
        }
    }
    map.remove(reply);
    reply->deleteLater();
}
