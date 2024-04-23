#include "widget.h"
#include "./ui_widget.h"
#include "Constants.h"
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
    timer->setInterval(3000);
    timer->start();

    netease_music_searcher = new NetEaseMusicSearcher(this);
    kugou_music_searcher = new KugouMusicSearcher(this);
    libvio_searcher = new LibvioSearcher(this);

    playlist_database = new PlaylistDatabase(this);
    file_downloader = new FileDownloader(this);

    ui->playlistView->setModel(playlist_database->model());
    ui->searchlistView->setModel(netease_music_searcher->model());
    ui->TVlistView->setModel(libvio_searcher->model());

    for (auto child : this->findChildren<QWidget*>()) {
        child->installEventFilter(this);
    }
    for (auto line_edit : this->findChildren<LineEditWidget*>()) {
        line_edit->removeEventFilter(this);
        for (auto child : line_edit->findChildren<QWidget*>())
        { child->removeEventFilter(this); }
    }

    connect(timer, &QTimer::timeout, this, &Widget::hide_controls);
    connect(ui->aspectEdit->lineEdit(), &QLineEdit::textChanged, this, &Widget::set_aspect_ratio);
    connect(ui->aspectEdit->pushButton(), &QPushButton::clicked, this, [p = ui->aspectEdit->lineEdit()](){ p->setText("00:00"); });
    connect(ui->rateEdit->lineEdit(), &QLineEdit::textChanged, media_player, [this](const QString &rate) {
        if (rate.toFloat() > 4) { ui->rateEdit->lineEdit()->setText("4.0"); }
        else if (rate.toFloat() < 0.3) { ui->rateEdit->lineEdit()->setText("0.3"); }
        else { media_player->set_rate(rate.toFloat()); }
    });
    connect(ui->rateEdit->pushButton(), &QPushButton::clicked, this, [p = ui->rateEdit->lineEdit()](){ p->setText("1.0"); });
    connect(ui->urlEdit->pushButton(), &QPushButton::clicked, this, [this]() { play(ui->urlEdit->lineEdit()->text()); });
    connect(ui->playBtn, &QPushButton::clicked, this, &Widget::do_playBtn_clicked);
    connect(ui->popSlider, &QPushButton::clicked, this, &Widget::do_popSlider_clicked);
    connect(ui->nextBtn, &QPushButton::clicked, this, [this](){ play_current_list(1); });
    connect(ui->preBtn, &QPushButton::clicked, this, [this](){ play_current_list(-1); });

    connect(ui->orderBtn, &QPushButton::clicked, this, &Widget::do_orderBtn_clicked);
    connect(ui->fullscreenBtn, &QPushButton::clicked, this, &Widget::do_fullscreenBtn_clicked);

    connect(ui->playlistWidget, &DataDisplayWidget::addItem, this, &Widget::do_addPlaylistItem);
    connect(ui->playlistWidget, &DataDisplayWidget::deleteItem, this, [p = playlist_database->model()]() { p->select(); });
    connect(ui->playlistView, &QAbstractItemView::doubleClicked, this, &Widget::play_playlist);
    connect(ui->searchlistWidget->onClickedMenu()->addAction("download"), &QAction::triggered, this, &Widget::searchlist_downloadAction_triggered);
    connect(ui->searchlistView, &QAbstractItemView::doubleClicked, this, &Widget::play_searchlist);
    connect(ui->TVlistView, &QTreeView::doubleClicked, libvio_searcher, &LibvioSearcher::processModelIndex);
    connect(ui->TVlistView->model(), &QAbstractItemModel::rowsInserted, ui->TVlistView, &QTreeView::expand);

    connect(libvio_searcher, &LibvioSearcher::mediaUrlReady, this, [this](const QString &url) { play(url); });

    connect(ui->seekSlider, &QSlider::valueChanged, this, &Widget::do_seekSlider_valueChanged);
    connect(ui->seekSlider, &QSlider::sliderReleased, this, &Widget::do_seekSliderReleased);
    connect(ui->popSlider->slider(), &QSlider::valueChanged, this, &Widget::do_volumeSlider_valueChanged);
    connect(media_player, &LMediaPlayer::positionChanged, this, &Widget::do_mediaPlayer_positionChanged);
    connect(media_player, &LMediaPlayer::playbackStateChanged, this, &Widget::do_playbackStateChanged);
    connect(media_player, &LMediaPlayer::mediaTypeChanged, this, &Widget::do_mediatypeChanged);
    connect(media_player, &LMediaPlayer::durationChanged, this, &Widget::do_durationChanged);
    connect(ui->searchEdit->pushButton(), &QPushButton::clicked, this, &Widget::do_searchBtn_clicked);
    connect(file_downloader, &FileDownloader::downloadFinished, this, [p = playlist_database](const QString &file_path) {
        p->insertData(QFileInfo(file_path).completeBaseName(), QDir::toNativeSeparators(file_path));
    });
    connect(ui->TVsearchEdit->pushButton(), &QPushButton::clicked, libvio_searcher, [this]() {
        static_cast<QStandardItemModel*>(ui->TVlistView->model())->clear();
        libvio_searcher->search(ui->TVsearchEdit->lineEdit()->text());
    });

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
    QPixmap pixmap(MainWindow::background_image);
    ui->videoWidget->setPixmap(pixmap);
    ui->videoWidget->setScaledContents(true);
    this->layout()->setContentsMargins(0, 0, 0, 0);
    this->layout()->setSpacing(0);

    ui->playlistWidget->setStreamerOrientation(Qt::Vertical);
    ui->playlistWidget->setStreamerSpeed(5);
    ui->searchlistWidget->setStreamerOrientation(Qt::Vertical);
    ui->searchlistWidget->setStreamerSpeed(5);
    ui->TVlistWidget->setStreamerOrientation(Qt::Vertical);
    ui->TVlistWidget->setStreamerSpeed(5);

    ui->splitter->setHandleWidth(0);
    ui->playlistView->setColumnHidden(Sql::url, true);
    ui->playlistWidget->setView(ui->playlistView);
    ui->playlistView->setHeaderHidden(true);
    ui->searchlistWidget->setView(ui->searchlistView);
    ui->searchlistView->setAutoScroll(true);
    ui->TVlistWidget->setView(ui->TVlistView);
    ui->TVlistView->setHeaderHidden(true);

    QPushButton *downloadBtn = new HoverFill<QPushButton>(ui->searchlistWidget);
    downloadBtn->setFlat(true);
    downloadBtn->setIcon(QIcon(IconPath::download));
    ui->searchlistWidget->topLayout()->addWidget(downloadBtn);
    connect(downloadBtn, &QPushButton::clicked, this, &Widget::searchlist_downloadAction_triggered);
   
    HoverFillPushButton *add_btn = new HoverFillPushButton(ui->playlistWidget);
    HoverFillPushButton *delete_btn= new HoverFillPushButton(ui->playlistWidget);
    add_btn->setIcon(QIcon(IconPath::add));
    delete_btn->setIcon(QIcon(IconPath::minus));

    ui->tabWidget->setShowType(ShowType::RightToLeft);
    ui->topBar->setShowType(ShowType::TopToBottom);

    ui->playlistWidget->topLayout()->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
    ui->playlistWidget->topLayout()->addWidget(add_btn);
    ui->playlistWidget->topLayout()->addWidget(delete_btn);
    ui->playlistWidget->connect(add_btn, &QPushButton::clicked, ui->playlistWidget, &DataDisplayWidget::addItem);
    ui->playlistWidget->connect(delete_btn, &QPushButton::clicked, ui->playlistWidget, &DataDisplayWidget::deleteAction_triggered);

    ui->urlEdit->setFixedHeight(40);
    ui->urlEdit->pushButton()->setIcon(QIcon(IconPath::play));
    ui->urlEdit->tipLabel()->setText("url");
    ui->urlEdit->setTipLabelColor(QColor("#707070"));

    ui->searchEdit->setFixedHeight(40);
    ui->searchEdit->pushButton()->setIcon(QIcon(IconPath::search));
    ui->searchEdit->tipLabel()->setText("search");
    ui->searchEdit->setTipLabelColor(QColor("#707070"));
    ui->searchlistWidget->topLayout()->addWidget(ui->searchEdit);

    ui->TVsearchEdit->setFixedHeight(40);
    ui->TVsearchEdit->pushButton()->setIcon(QIcon(IconPath::search));
    ui->TVsearchEdit->tipLabel()->setText("search TV");
    ui->TVsearchEdit->setTipLabelColor(QColor("#707070"));
    ui->TVlistWidget->topLayout()->addWidget(ui->TVsearchEdit);
    
    ui->popSlider->slider()->setValue(50);
    ui->popSlider->setProperty("mute", false);
    
    ui->aspectEdit->tipLabel()->setText("aspect ratio");
    ui->aspectEdit->lineEdit()->setInputMask("99:99;_");
    ui->aspectEdit->lineEdit()->setAlignment(Qt::AlignCenter);
    ui->aspectEdit->lineEdit()->setText("00:00");
    ui->aspectEdit->pushButton()->setIcon(QIcon(IconPath::flash));
    ui->aspectEdit->setTipLabelColor(QColor("#707070"));

    ui->rateEdit->tipLabel()->setText("play rate");
    ui->rateEdit->lineEdit()->setInputMask("9.0;_");
    ui->rateEdit->lineEdit()->setAlignment(Qt::AlignCenter);
    ui->rateEdit->lineEdit()->setText("1.0");
    ui->rateEdit->pushButton()->setIcon(QIcon(IconPath::flash));
    ui->rateEdit->setTipLabelColor(QColor("#707070"));
    
    ui->tabWidget->setCurrentWidget(ui->playlistTab);
}

bool splitter_handle_under_mouse(QSplitter *splitter)
{
    for (int i = 0, n = splitter->count(); i < n; ++i) {
        if (splitter->handle(i)->underMouse()) { return true; }
    }
    return false;
}

void Widget::hide_controls()
{
    if (not ui->popSlider->dialog_visible() and not ui->aspectEdit->lineEdit()->hasFocus()) { ui->bottomBar->hide(); }
    if (not ui->urlEdit->lineEdit()->hasFocus()) { ui->topBar->hide(); }
    if (not ui->searchEdit->lineEdit()->hasFocus() and not splitter_handle_under_mouse(ui->splitter)
        and not ui->playlistWidget->menuVisible() and not ui->searchlistWidget->menuVisible()
    ) {  ui->tabWidget->hide(); }
    if (ui->bottomBar->isHidden() and ui->tabWidget->isHidden()) {
        this->setCursor(Qt::BlankCursor);
    }
}

void Widget::show_controls()
{
    timer->stop();
    ui->topBar->show();
    ui->bottomBar->show();
    ui->tabWidget->show();
    ui->playlistWidget->show();
    ui->searchlistWidget->show();
    this->setCursor(Qt::CustomCursor);
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

void Widget::play_playlist(const QModelIndex &index)
{
    if (not index.isValid()) { return; }
    QModelIndex url_index = playlist_database->model()->index(index.row(), Sql::url);
    QString url = playlist_database->model()->data(url_index).toString();
    play(url);
}

void Widget::play_searchlist(const QModelIndex &index)
{
    if (not index.isValid()) { return; }
    auto item = netease_music_searcher->item(index);
    QString url = item->data(Qt::UserRole).toString(), name = item->text();
    play(url);
}

DataDisplayWidget *Widget::current_list_widget() const
{
    int list_index = ui->tabWidget->currentIndex();
    auto list_tab = ui->tabWidget->widget(list_index);
    return list_tab->findChild<DataDisplayWidget*>();
}

void Widget::play_current_list(int step)
{
    auto list_widget = current_list_widget();
    QModelIndex play_index = list_widget->selectNextModelIndex(step);
    if (list_widget == ui->playlistWidget) {
        play_playlist(play_index);
    } else if (list_widget == ui->searchlistWidget) {
        play_searchlist(play_index);
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    this->setCursor(Qt::ArrowCursor);
}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
        case QEvent::MouseMove: {
            this->mouseMoveEvent(static_cast<QMouseEvent*>(event));
        } break;
        case QEvent::KeyPress: {
            this->keyPressEvent(static_cast<QKeyEvent*>(event));
            return true;
        } break;
    }
    return QObject::eventFilter(obj, event);
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

void Widget::wheelEvent(QWheelEvent *event)
{
    ui->popSlider->slider()->setValue(ui->popSlider->slider()->value() + event->angleDelta().y() / 60);
    QWidget::wheelEvent(event);
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

void Widget::do_popSlider_clicked()
{
    if (ui->popSlider->property("mute").toBool()) {
        ui->popSlider->setProperty("mute", false);
        ui->popSlider->setIcon(QIcon(IconPath::volume));
        media_player->set_volume(ui->popSlider->slider()->value());
    } else {
        ui->popSlider->setProperty("mute", true);
        ui->popSlider->setIcon(QIcon(IconPath::mute));
        media_player->set_volume(0);
    }
}

void Widget::do_searchBtn_clicked()
{
    static_cast<QStandardItemModel*>(ui->searchlistView->model())->clear();
    netease_music_searcher->search(ui->searchEdit->lineEdit()->text());
    kugou_music_searcher->search(ui->searchEdit->lineEdit()->text());
}

void Widget::do_addPlaylistItem()
{
    auto file_paths = QFileDialog::getOpenFileNames(this, Dialog::choose_media, QDir::homePath(), Filter::media_filter);
    for (const auto &file_path : file_paths) {
        QFileInfo file_info(file_path);
        if (not file_info.exists()) { continue; }
        playlist_database->insertData(file_info.completeBaseName(), QDir::toNativeSeparators(file_path));
    }
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

            auto list_widget = current_list_widget();
            int row_count = list_widget->rowCount();
            switch(media_player->playback_order()) {
                case LMediaPlayer::SequentialOrder: {
                    if (list_widget->currentRow() != row_count - 1)
                    { play_current_list(1); }
                } break;
                case LMediaPlayer::SingleLoopOrder: {
                    play_current_list(0);
                } break;
                case LMediaPlayer::LoopingOrder: {
                    play_current_list(1);
                } break;
                case LMediaPlayer::RandomOrder: {
                    play_current_list(QRandomGenerator::global()->bounded(0, row_count - 1));
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

void Widget::searchlist_downloadAction_triggered()
{
    auto index = ui->searchlistView->currentIndex();
    if (not index.isValid()) { return; }
    auto item = netease_music_searcher->item(index);
    QString url = item->data(Qt::UserRole).toString(), name = item->text();
    file_downloader->download(url, name);
}
