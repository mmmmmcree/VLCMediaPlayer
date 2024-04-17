#include "PlaylistDatabase.h"
#include "Constants.h"
#include <QMessageBox>
#include <QSqlError>

PlaylistDatabase::PlaylistDatabase(QObject *parent)
{
    if (QSqlDatabase::contains(Sql::database_name)) {
        db = QSqlDatabase::database(Sql::database_name);
    } else {
        db = QSqlDatabase::addDatabase(Sql::database_driver);
        db.setDatabaseName(QString(Sql::database_name) + ".db");
    }
    if (not db.open()) {
        QMessageBox::critical(nullptr, "Error", SqlError::open_database_failed);
        return;
    }
    QSqlQuery query(db);
    if (not query.exec(Sql::create_playlist_table)) {
        QMessageBox::critical(nullptr, "Error", db.lastError().text());
    }
    _model = new QSqlTableModel(this);
    _model->setTable(Sql::table_name);
    _model->setEditStrategy(QSqlTableModel::OnFieldChange);
    _model->select();
}

PlaylistDatabase::~PlaylistDatabase()
{
    db.close();
}

QSqlTableModel *PlaylistDatabase::model()
{
    return _model;
}

void PlaylistDatabase::insertData(const QString &name, const QString &url)
{
    int row = _model->rowCount();
    _model->insertRow(row);
    _model->setData(_model->index(row, 0), name);
    _model->setData(_model->index(row, 1), url);
    _model->submit();
}

void PlaylistDatabase::clear()
{
    _model->clear();
}
