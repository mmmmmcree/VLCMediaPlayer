#ifndef LMEDIAPLAYER_H
#define LMEDIAPLAYER_H

#ifdef _WIN32
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif // for MSVC

#include "vlc/vlc.h"
#include <QObject>
#include <QString>

class LMediaPlayer : public QObject
{
    Q_OBJECT
public:
    enum PlaybackState
    {
        StoppedState = libvlc_state_t::libvlc_Stopped,
        PlayingState = libvlc_state_t::libvlc_Playing,
        PausedState = libvlc_state_t::libvlc_Paused,
        EndedState = libvlc_state_t::libvlc_Ended
    };
    enum PlaybackOrder
    {
        LoopingOrder,
        SequentialOrder,
        RandomOrder,
        SingleLoopOrder,
        SIZE
    };
    enum MediaType
    {
        None,
        Audio,
        Video
    };
signals:
    void playbackStateChanged(PlaybackState state);
    void mediaTypeChanged(MediaType type);
    void durationChanged(int duration);
    void positionChanged(float position);
public:
    LMediaPlayer(QObject *parent = nullptr);
    ~LMediaPlayer();
    void block_hwnd_event_filter();
    void restore_hwnd_event_filter();
    void set_hwnd(quintptr hwnd);
    bool set_source(const QString &path, bool is_streaming = false);
    bool play();
    void pause();
    void set_position(float position);
    void set_volume(int volume);
    void set_aspect_ratio(int width, int height);
    void set_aspect_ratio(const QString &ratio);
    void set_playback_order_next();
    PlaybackOrder playback_order() const;
    libvlc_media_t *media() const;
    libvlc_media_player_t *player() const;
    PlaybackState playback_state() const;
private:
    libvlc_instance_t *_instance = nullptr;
    libvlc_media_player_t *_player = nullptr;
    libvlc_media_t *_media = nullptr;
    PlaybackOrder _playback_order = LoopingOrder;
    // MediaType _media_type = None;
};

#endif // LMEDIAPLAYER_H