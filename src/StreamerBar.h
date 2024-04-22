#ifndef STREAMERBAR_H
#define STREAMERBAR_H
#include <QWidget>
#include "AnimatedShow.h"
#include "Streamer.h"
using StreamerBar = Streamer<AnimatedShow<QWidget>>;
#endif // STREAMERBAR_H