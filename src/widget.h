#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "LMediaPlayer.h"
#include "HoverFillPushButton.h"
#include "PopSlider.h"
#include "Slider.h"
#include "StreamerBar.h"
#include "MusicSearcher.h"
#include "PlaylistDatabase.h"

#include "TreeView.h"

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
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual bool eventFilter(QObject *obj, QEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
private:
    void init_ui();
    void hide_controls();
    void show_controls();
    void play(const QString &url);
    void set_aspect_ratio(const QString &ratio);
private slots:
    void do_playBtn_clicked();
    void do_orderBtn_clicked();
    void do_fullscreenBtn_clicked();
    void do_searchBtn_clicked();
    void do_addPlaylistItem();
    void do_playlistView_doubleClicked(const QModelIndex &index);
    void do_searchlistView_doubleClicked(const QModelIndex &index);
    void do_durationChanged(int duration);
    void do_seekSlider_valueChanged(int value);
    void do_seekSliderReleased();
    void do_mediaPlayer_positionChanged(float position);
    void do_playbackStateChanged(LMediaPlayer::PlaybackState state);
    void do_mediatypeChanged(LMediaPlayer::MediaType type);
    void do_volumeSlider_valueChanged(int value);
private:
    Ui::Widget *ui;
    QTimer *timer;
    LMediaPlayer *media_player;
    MusicSearcher *netease_music_searcher;
    PlaylistDatabase *playlist_database;
};
#endif // WIDGET_H
