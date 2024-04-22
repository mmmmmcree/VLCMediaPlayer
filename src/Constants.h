#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <QFile>

namespace vlc {
    static constexpr auto plugins_env_path = "VLC_PLUGIN_PATH=D:/Desktop/cpp/Qt/VLCMediaPlayer/vlc-3.0.20/plugins";
    struct Warning {
        static constexpr auto create_instance_failure = "Failed to create VLC instance";
        static constexpr auto create_player_failure = "Failed to create VLC media player";
    };
}

struct MainWindow {
    static constexpr auto title = "Mango Player";
    static constexpr auto background_image = ":/imgs/background.png";
};

struct Sql {
    static constexpr auto database_name = "playlist_data";
    static constexpr auto database_driver = "QSQLITE";
    static constexpr auto table_name = "playlist";
    static constexpr auto create_playlist_table = "CREATE TABLE IF NOT EXISTS playlist (name TEXT, url TEXT)";
    enum Columns {
        name,
        url
    };
};

struct JavaScript
{
    static constexpr auto get_search_results = R"(
        var items = document.getElementsByClassName('stui-vodlist__thumb lazyload');
        var results = [];
        for (var i = 0; i < items.length; i++) { results.push({href: items[i].href, title: items[i].title}); }
        results;
    )";
    static constexpr auto get_episode_links = R"(
        var items = document.querySelector("body > div:nth-child(3) > div.row > div > div > div > div:nth-child(2) > ul").querySelectorAll("li");
        var results = [];
        for (var i = 0; i < items.length; i++) { results.push(items[i].querySelector('a').href); }
        results;
    )";
    static constexpr auto get_media_url = R"(document.querySelector('#playleft > iframe').contentWindow.document.querySelector('video').src)";
};

struct SqlError {
    static constexpr auto open_database_failed = "Failed to open database";
};

struct IconPath {
    static constexpr auto pause = ":/imgs/pause.svg";
    static constexpr auto play = ":/imgs/play.svg";
    static constexpr auto stop = ":/imgs/stop.svg";
    static constexpr auto full_screen = ":/imgs/fullscreen.svg";
    static constexpr auto normal_screen = ":/imgs/normalscreen.svg";
    static constexpr auto volume = ":/imgs/volume.svg";
    static constexpr auto add = ":/imgs/add.svg";
    static constexpr auto minus = ":/imgs/minus.svg";
    static constexpr auto sequential = ":/imgs/sequential.svg";
    static constexpr auto loop = ":/imgs/loop.svg";
    static constexpr auto single_loop = ":/imgs/singleloop.svg";
    static constexpr auto random = ":/imgs/random.svg";
    static constexpr auto mute = ":/imgs/mute.svg";
    static constexpr auto flash = ":/imgs/flash.svg";
    static constexpr auto search = ":/imgs/search.svg";
    static constexpr auto download = ":/imgs/download.svg";
};

struct StyleSheet {
private:
    static auto get_stylesheet(const char *path) {
        QFile file(path);
        file.open(QFile::ReadOnly | QFile::Text);
        return file.readAll();
    }
public:
    static auto tab_widget() { return get_stylesheet(":/stylesheets/tabwidget.qss"); }
};

struct Network {
    static constexpr auto user_agent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:104.0) Gecko/20100101 Firefox/104.0";
};

struct Filter {
    static constexpr auto media_filter = "Media files (*.aac,*.flac *.mp3 *.wav *.ogg *.mp4 *.avi *.mkv *.mov, *.flv)";
};

struct Dialog {
    static constexpr auto choose_media = "Choose media files";
};
#endif