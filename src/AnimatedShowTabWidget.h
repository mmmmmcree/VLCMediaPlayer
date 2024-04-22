#ifndef ANIMATEDSHOWTABWIDGET_H
#define ANIMATEDSHOWTABWIDGET_H
#include "AnimatedShow.h"
#include "LeaveHide.h"
#include <QTabWidget>
using AnimatedShowTabWidget = LeaveHide<AnimatedShow<QTabWidget>>;
#endif // ANIMATEDSHOWTABWIDGET_H