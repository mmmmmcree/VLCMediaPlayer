#ifndef PLAYLISTDATABASE_H
#define PLAYLISTDATABASE_H
#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>

class PlaylistDatabase: public QObject
{
    Q_OBJECT
public:
    PlaylistDatabase(QObject *parent = nullptr);
    ~PlaylistDatabase();
    void insertData(const QString &name, const QString &url);
    void clear();
    QSqlTableModel *model();
private:
    QSqlDatabase db;
    QSqlTableModel *_model;
};

#endif // DATABASE_H 