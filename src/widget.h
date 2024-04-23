#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "LMediaPlayer.h"
#include "MusicSearcher.h"
#include "LibvioSearcher.h"
#include "PlaylistDatabase.h"
#include "FileDownloader.h"
#include "DataDisplayWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    void init_ui();
    void hide_controls();
    void show_controls();
    void play(const QString &url);
    void set_aspect_ratio(const QString &ratio);
    void play_playlist(const QModelIndex &index);
    void play_searchlist(const QModelIndex &index);
    DataDisplayWidget *current_list_widget() const;
    void play_current_list(int step = 0);
private slots:
    void do_playBtn_clicked();
    void do_orderBtn_clicked();
    void do_fullscreenBtn_clicked();
    void do_popSlider_clicked();
    void do_searchBtn_clicked();
    void do_addPlaylistItem();
    void do_durationChanged(int duration);
    void do_seekSlider_valueChanged(int value);
    void do_seekSliderReleased();
    void do_mediaPlayer_positionChanged(float position);
    void do_playbackStateChanged(LMediaPlayer::PlaybackState state);
    void do_mediatypeChanged(LMediaPlayer::MediaType type);
    void do_volumeSlider_valueChanged(int value);
    void searchlist_downloadAction_triggered();
private:
    Ui::Widget *ui;
    QTimer *timer;
    LMediaPlayer *media_player;
    MusicSearcher *netease_music_searcher, *kugou_music_searcher;
    LibvioSearcher *libvio_searcher;
    PlaylistDatabase *playlist_database;
    FileDownloader *file_downloader;
};
#endif // WIDGET_H
