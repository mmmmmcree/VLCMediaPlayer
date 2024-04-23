#include "LMediaPlayer.h"
#include "Constants.h"
#include <cstdlib>
#include <stdexcept>
#include <QDir>

#include <QMessageBox>

void event_filter(const libvlc_event_t *event, void *user_data) {
    LMediaPlayer *lmp = static_cast<LMediaPlayer*>(user_data);
    auto player = lmp->player();
    switch(event->type) {
        case libvlc_MediaParsedChanged: {
            int duration = libvlc_media_get_duration(lmp->media());
            emit lmp->durationChanged(duration);

            libvlc_media_track_t **tracks;
            int count = libvlc_media_tracks_get(lmp->media(), &tracks);
            for (int i = 0; i < count; ++i) {
                if (tracks[i]->i_type == libvlc_track_video) {
                    emit lmp->mediaTypeChanged(LMediaPlayer::Video);
                    break;
                }
                if (i == count - 1) {
                    emit lmp->mediaTypeChanged(LMediaPlayer::Audio);
                }
            }
            libvlc_media_tracks_release(tracks, count);
        } break;
        case libvlc_MediaPlayerPositionChanged: {
            float position = libvlc_media_player_get_position(player);
            emit lmp->positionChanged(position);
        } break;
        case libvlc_MediaPlayerPlaying: {
            emit lmp->playbackStateChanged(LMediaPlayer::PlayingState);
        } break;
        case libvlc_MediaPlayerPaused: {
            emit lmp->playbackStateChanged(LMediaPlayer::PausedState);
        } break;
        case libvlc_MediaPlayerEndReached: {
            emit lmp->playbackStateChanged(LMediaPlayer::EndedState);
        } break;
    }
}

LMediaPlayer::LMediaPlayer(QObject *parent) : QObject(parent)
{
    // putenv(vlc::plugins_env_path);
    _instance = libvlc_new(0, nullptr);
    if (not _instance) { throw std::runtime_error(vlc::Warning::create_instance_failure); }
    _player = libvlc_media_player_new(_instance);
    if (not _player) { throw std::runtime_error(vlc::Warning::create_player_failure); }
    libvlc_event_manager_t *em = libvlc_media_player_event_manager(_player);

    libvlc_event_attach(em, libvlc_MediaPlayerPositionChanged, event_filter, this);
    libvlc_event_attach(em, libvlc_MediaPlayerPaused, event_filter, this);
    libvlc_event_attach(em, libvlc_MediaPlayerPlaying, event_filter, this);
    libvlc_event_attach(em, libvlc_MediaPlayerEndReached, event_filter, this);
}

LMediaPlayer::~LMediaPlayer()
{
    libvlc_release(_instance);
    libvlc_free(_player);
}

#ifdef _WIN32
#include <windows.h>
void CALLBACK Block(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (not hwnd) { return; }
    EnableWindow(hwnd, false);
    KillTimer(nullptr, 0);
}
void CALLBACK Restore(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
    if (not hwnd) { return; }
    EnableWindow(hwnd, true);
    KillTimer(nullptr, 0);
}
void LMediaPlayer::block_hwnd_event_filter()
{
    SetTimer(reinterpret_cast<HWND>(libvlc_media_player_get_hwnd(_player)), 0, 100, Block);
}
void LMediaPlayer::restore_hwnd_event_filter()
{
    SetTimer(reinterpret_cast<HWND>(libvlc_media_player_get_hwnd(_player)), 0, 100, Restore);
}
#endif

void LMediaPlayer::set_hwnd(quintptr hwnd)
{
    libvlc_media_player_set_hwnd(_player, reinterpret_cast<void*>(hwnd));
}

bool LMediaPlayer::set_source(const QString &path, bool is_streaming)
{
    if (is_streaming) {
        _media = libvlc_media_new_location(_instance, path.toStdString().c_str());
    } else {
        _media = libvlc_media_new_path(_instance, path.toStdString().c_str());
    }
    if (not _media) { return false; }
    libvlc_event_manager_t *em = libvlc_media_event_manager(_media);
    libvlc_event_attach(em, libvlc_MediaParsedChanged, event_filter, this);
    libvlc_media_parse_with_options(_media, libvlc_media_parse_local, -1);
    libvlc_media_player_set_media(_player, _media);
    return true;
}

bool LMediaPlayer::play()
{
    return libvlc_media_player_play(_player) == 0;
}

void LMediaPlayer::pause()
{
    libvlc_media_player_pause(_player);
}

void LMediaPlayer::set_position(float position)
{
    libvlc_media_player_set_position(_player, position);
}

void LMediaPlayer::set_volume(int volume)
{
    libvlc_audio_set_volume(_player, volume);
}

void LMediaPlayer::set_aspect_ratio(int width, int height)
{
    libvlc_video_set_aspect_ratio(_player, QString("%1:%2").arg(width).arg(height).toUtf8().constData());
}

void LMediaPlayer::set_aspect_ratio(const QString &ratio)
{
    libvlc_video_set_aspect_ratio(_player, ratio.toUtf8().constData());
}

void LMediaPlayer::set_playback_order_next()
{
    _playback_order = static_cast<PlaybackOrder>((_playback_order + 1) % PlaybackOrder::SIZE);
}

LMediaPlayer::PlaybackOrder LMediaPlayer::playback_order() const
{
    return _playback_order;
}

libvlc_media_t *LMediaPlayer::media() const
{
    return _media;
}

libvlc_media_player_t *LMediaPlayer::player() const
{
    return _player;
}

LMediaPlayer::PlaybackState LMediaPlayer::playback_state() const
{
    return static_cast<PlaybackState>(libvlc_media_player_get_state(_player));
}

bool LMediaPlayer::set_rate(float rate)
{
    return libvlc_media_player_set_rate(_player, rate) == 0;
}
