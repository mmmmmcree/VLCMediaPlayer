#include "widget.h"
#include "./ui_widget.h"
#include "Constants.h"
#include "TemplateView.h"
#include <QRandomGenerator>
#include <QTimer>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>


Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);

    media_player = new LMediaPlayer(this);
    media_player->set_hwnd(ui->videoWidget->winId());

    timer = new QTimer(this);
    timer->setInterval(2500);
    timer->start();

    netease_music_searcher = new NetEaseMusicSearcher(this);

    playlist_database = new PlaylistDatabase(this);

    ui->playlistView->setModel(playlist_database->model());
    ui->searchlistView->setModel(netease_music_searcher->model());

    for (auto child : this->findChildren<QWidget*>()) {
        child->installEventFilter(this);
    }

    connect(timer, QTimer::timeout, this, hide_controls);
    connect(ui->aspectEdit->lineEdit(), QLineEdit::textChanged, this, set_aspect_ratio);
    connect(ui->aspectEdit->pushButton(), QPushButton::clicked, this, [p = ui->aspectEdit->lineEdit()](){ p->setText("00:00"); });
    connect(ui->playBtn, QPushButton::clicked, this, do_playBtn_clicked);
    connect(ui->popSlider, QPushButton::clicked, this, [p = ui->popSlider](){ p->slider()->setValue(0); });

    connect(ui->nextBtn, QPushButton::clicked, this, [p = ui->playlistWidget](){ p->selectNextItem(); });
    connect(ui->preBtn, QPushButton::clicked, this, [p = ui->playlistWidget](){ p->selectPreviousItem(); });

    connect(ui->orderBtn, QPushButton::clicked, this, do_orderBtn_clicked);
    connect(ui->fullscreenBtn, QPushButton::clicked, this, do_fullscreenBtn_clicked);

    connect(ui->playlistWidget, DataDisplayWidget::addItem, this, do_addPlaylistItem);
    connect(ui->playlistWidget, DataDisplayWidget::deleteItem, this, [p = playlist_database->model()]() { p->select(); });
    connect(ui->playlistView, QAbstractItemView::doubleClicked, this, do_playlistView_doubleClicked);
    connect(ui->searchlistView, QAbstractItemView::doubleClicked, this, do_searchlistView_doubleClicked);


    connect(ui->seekSlider, QSlider::valueChanged, this, do_seekSlider_valueChanged);
    connect(ui->seekSlider, QSlider::sliderReleased, this, do_seekSliderReleased);
    connect(ui->popSlider->slider(), QSlider::valueChanged, this, do_volumeSlider_valueChanged);
    connect(media_player, LMediaPlayer::positionChanged, this, do_mediaPlayer_positionChanged);
    connect(media_player, LMediaPlayer::playbackStateChanged, this, do_playbackStateChanged);
    connect(media_player, LMediaPlayer::mediaTypeChanged, this, do_mediatypeChanged);
    connect(media_player, LMediaPlayer::durationChanged, this, do_durationChanged);
    connect(ui->searchEdit->pushButton(), QPushButton::clicked, this, do_searchBtn_clicked);


    init_ui();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::init_ui()
{
    this->setWindowTitle(MainWindow::title);
    this->setMouseTracking(true);
    ui->videoWidget->setMouseTracking(true);
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->setSpacing(0);
    ui->playlistWidget->setStreamerOrientation(Qt::Vertical);
    ui->playlistWidget->setStreamerSpeed(5);
    ui->searchlistWidget->setStreamerOrientation(Qt::Vertical);
    ui->searchlistWidget->setStreamerSpeed(5);

    ui->playlistView->setColumnHidden(Sql::url, true);
    ui->playlistWidget->setView(ui->playlistView);
    ui->playlistView->setHeaderHidden(true);
    ui->searchlistWidget->setView(ui->searchlistView);
    
    HoverFillPushButton *add_btn = new HoverFillPushButton(ui->playlistWidget);
    HoverFillPushButton *delete_btn= new HoverFillPushButton(ui->playlistWidget);
    add_btn->setIcon(QIcon(IconPath::add));
    delete_btn->setIcon(QIcon(IconPath::minus));

    ui->playlistWidget->topLayout()->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    ui->playlistWidget->topLayout()->addWidget(add_btn);
    ui->playlistWidget->topLayout()->addWidget(delete_btn);
    ui->playlistWidget->connect(add_btn, QPushButton::clicked, ui->playlistWidget, DataDisplayWidget::addItem);
    ui->playlistWidget->connect(delete_btn, QPushButton::clicked, ui->playlistWidget, DataDisplayWidget::deleteAction_triggered);

    ui->searchEdit->setFixedHeight(40);
    ui->searchEdit->pushButton()->setIcon(QIcon(IconPath::search));
    ui->searchlistWidget->topLayout()->addWidget(ui->searchEdit);
    
    ui->popSlider->slider()->setValue(50);
    ui->aspectEdit->tipLabel()->setText("aspect ratio");
    ui->aspectEdit->lineEdit()->setInputMask("99:99;_");
    ui->aspectEdit->lineEdit()->setAlignment(Qt::AlignCenter);
    ui->aspectEdit->lineEdit()->setText("00:00");
    ui->aspectEdit->pushButton()->setIcon(QIcon(IconPath::flash));
    ui->aspectEdit->setTipLabelColor(QColor("#707070"));
    ui->tabWidget->setCurrentWidget(ui->playlistTab);
}

void Widget::hide_controls()
{
    if (not ui->popSlider->dialog_visible() and not ui->aspectEdit->lineEdit()->hasFocus()) { ui->bottomBar->hide(); }
    if (not ui->searchEdit->lineEdit()->hasFocus() and
        not ui->searchlistWidget->isVisible() and
        not ui->playlistView->isVisible())
    { ui->tabWidget->hide(); }
    if (ui->bottomBar->isHidden() and ui->playlistWidget->isHidden()) {
        this->setCursor(Qt::BlankCursor);
    }
}

void Widget::show_controls()
{
    ui->bottomBar->show();
    ui->tabWidget->show();
    this->setCursor(Qt::ArrowCursor);
    timer->start();
}

void Widget::play(const QString &url)
{
    media_player->set_source(url, url.startsWith("http"));
    bool successful = media_player->play();
    if (not successful) {
        QMessageBox::warning(this, "play status", "failed");
    } else {
        set_aspect_ratio(ui->aspectEdit->lineEdit()->text());
    }
}

void Widget::set_aspect_ratio(const QString &ratio)
{
    auto aspect_ratio = ratio.split(':');
    if (aspect_ratio.size() != 2) { return; }
    int width = aspect_ratio[0].toInt(), height = aspect_ratio[1].toInt();
    if (width and height) {
        media_player->set_aspect_ratio(width, height);
    } else {
        media_player->set_aspect_ratio(this->width(), this->height());
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    show_controls();
}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
        case QEvent::MouseMove: {
            this->mouseMoveEvent(static_cast<QMouseEvent*>(event));
        } break;
        case QEvent::KeyPress: {
            if (obj == ui->aspectEdit or
                obj == ui->aspectEdit->lineEdit() or
                obj == ui->searchEdit or
                obj == ui->searchEdit->lineEdit()) { break; }
            this->keyPressEvent(static_cast<QKeyEvent*>(event));
            return true;
        } break;
    }
    return obj->eventFilter(obj, event);
}

void Widget::resizeEvent(QResizeEvent *event)
{
    set_aspect_ratio(ui->aspectEdit->lineEdit()->text());
    QWidget::resizeEvent(event);
}

void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QWidget::mouseDoubleClickEvent(event);
    do_fullscreenBtn_clicked();
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    show_controls();
}

void Widget::keyPressEvent(QKeyEvent * event)
{ 
    switch(event->key()) {
        case Qt::Key_Escape: { do_fullscreenBtn_clicked(); } break;
        case Qt::Key_Space: { do_playBtn_clicked(); } break;
        case Qt::Key_Right: { ui->seekSlider->setValue(ui->seekSlider->value() + 5000); } break;
        case Qt::Key_Left: { ui->seekSlider->setValue(ui->seekSlider->value() - 5000); } break;
        case Qt::Key_Up: { ui->popSlider->slider()->setValue(ui->popSlider->slider()->value() + 5); } break;
        case Qt::Key_Down: { ui->popSlider->slider()->setValue(ui->popSlider->slider()->value() - 5); } break;
    }
}

void Widget::do_playBtn_clicked()
{
    auto state = media_player->playback_state();
    if (state == LMediaPlayer::PlayingState) {
        media_player->pause();
    } else if (state == LMediaPlayer::PausedState) {
        media_player->play();
    }
}

void Widget::do_orderBtn_clicked()
{
    media_player->set_playback_order_next();
    switch(media_player->playback_order()) {
        case LMediaPlayer::SequentialOrder: {
            ui->orderBtn->setIcon(QIcon(IconPath::sequential));
        } break;
        case LMediaPlayer::SingleLoopOrder: {
            ui->orderBtn->setIcon(QIcon(IconPath::single_loop));
        } break;
        case LMediaPlayer::LoopingOrder: {
            ui->orderBtn->setIcon(QIcon(IconPath::loop));
        } break;
        case LMediaPlayer::RandomOrder: {
            ui->orderBtn->setIcon(QIcon(IconPath::random));
        } break;
    }
}

void Widget::do_fullscreenBtn_clicked()
{
    if (not this->isFullScreen()) {
        this->showFullScreen();
        ui->fullscreenBtn->setIcon(QIcon(IconPath::normal_screen));
        media_player->set_aspect_ratio(this->width(), this->height());
    } else {
        this->showNormal();
        ui->fullscreenBtn->setIcon(QIcon(IconPath::full_screen));
    }
}

void Widget::do_searchBtn_clicked()
{
    ui->searchlistView->model()->removeRows(0, ui->searchlistView->model()->rowCount());
    netease_music_searcher->searchMusic(ui->searchEdit->lineEdit()->text());
}

void Widget::do_addPlaylistItem()
{
    auto file_paths = QFileDialog::getOpenFileNames(this, Dialog::choose_media, QDir::homePath(), Filter::media_filter);
    for (const auto &file_path : file_paths) {
        QFileInfo file_info(file_path);
        if (not file_info.exists()) { continue; }
        playlist_database->insertData(file_info.fileName(), QDir::toNativeSeparators(file_path));
    }
}

void Widget::do_playlistView_doubleClicked(const QModelIndex &index)
{
    QModelIndex url_index = playlist_database->model()->index(index.row(), Sql::url);
    auto url = playlist_database->model()->data(url_index).toString();
    play(url);
}

void Widget::do_searchlistView_doubleClicked(const QModelIndex &index)
{
    auto url =netease_music_searcher->model()->data(index, Qt::UserRole).toString();
    play(url);
}

void Widget::do_durationChanged(int duration)
{
    ui->durationLabel->setText(QTime::fromMSecsSinceStartOfDay(duration).toString("hh:mm:ss"));
    ui->seekSlider->setMaximum(duration);
}

void Widget::do_seekSlider_valueChanged(int value)
{
    if (ui->seekSlider->isSliderDown()) { return; }
    media_player->set_position(static_cast<float>(value) / ui->seekSlider->maximum());
}

void Widget::do_seekSliderReleased()
{
    media_player->set_position(static_cast<float>(ui->seekSlider->value()) / ui->seekSlider->maximum());
}

void Widget::do_mediaPlayer_positionChanged(float position)
{
    if (ui->seekSlider->isSliderDown()) { return; }
    int time = position * ui->seekSlider->maximum();
    ui->seekSlider->blockSignals(true);
    ui->seekSlider->setValue(time);
    ui->seekSlider->blockSignals(false);
    ui->timeEdit->setTime(QTime::fromMSecsSinceStartOfDay(time));
}

void Widget::do_playbackStateChanged(LMediaPlayer::PlaybackState state)
{
    switch(state) {
        case LMediaPlayer::PlayingState: {
            ui->playBtn->setIcon(QIcon(IconPath::pause));
        } break;
        case LMediaPlayer::PausedState: {
            ui->playBtn->setIcon(QIcon(IconPath::play));
        } break;
        case LMediaPlayer::StoppedState: {
            ui->playBtn->setIcon(QIcon(IconPath::stop));
        } break;
        case LMediaPlayer::EndedState: {
            ui->playBtn->setIcon(QIcon(IconPath::stop));
            int row_count = playlist_database->model()->rowCount();
            switch(media_player->playback_order()) {
                case LMediaPlayer::SequentialOrder: {
                    if (ui->playlistView->currentIndex().row() != row_count - 1)
                    { ui->playlistWidget->selectNextItem(); }
                } break;
                case LMediaPlayer::SingleLoopOrder: {
                    ui->playlistWidget->selectNextItem(0);
                } break;
                case LMediaPlayer::LoopingOrder: {
                    ui->playlistWidget->selectNextItem();
                } break;
                case LMediaPlayer::RandomOrder: {
                    ui->playlistWidget->selectNextItem(QRandomGenerator::global()->bounded(0, row_count - 1));
                } break;
            }
        } break;
    }
}

void Widget::do_mediatypeChanged(LMediaPlayer::MediaType type)
{
    switch(type) {
        case LMediaPlayer::Audio: {
            media_player->restore_hwnd_event_filter();
        } break;
        case LMediaPlayer::Video: {
            media_player->block_hwnd_event_filter();
        } break;
    }
}

void Widget::do_volumeSlider_valueChanged(int value)
{
    media_player->set_volume(value);
    if (value == 0) {
        ui->popSlider->setIcon(QIcon(IconPath::mute));
    } else {
        ui->popSlider->setIcon(QIcon(IconPath::volume));
    }
}