#include "mediaplayer.h"

// Function initialize and setup head
// configuration CMP
MediaPlayer::MediaPlayer(QObject *parent) : QObject(parent)
{
    qDebug()<<"[MediaPlayer] Inicjacja Odtwarzacza...";
    // Init media player settings
    player = new QMediaPlayer();
    playlist = new QMediaPlaylist();
    player->setVolume(70);
    player->setPlaylist(playlist);

    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(showMenu()));
}

MediaPlayer::~MediaPlayer()
{
    player->stop();
    delete player;
    delete playlist;
}

// Function start media player
void MediaPlayer::run()
{
    bool set;
    do {
        set = mainMenu();
    }while(set);
}

// Header, status etc
void MediaPlayer::header()
{
#if defined WIN32
#define CLEAR "cls"
#else
#define CLEAR "clear"
    system( CLEAR );

#endif
    qDebug() << ".----------------------.";
    qDebug() << "| Console Media Player |";
    qDebug() << "`----------------------'";
    qDebug() << "| " << getStatus();
    qDebug() << "`----------------------";
    qDebug() << "| " << getPlaybackStatus();
    qDebug() << "`----------------------";

    if(playlist->mediaCount()) {
        showPlaylist();
        qDebug() << "'----------------------";
    }
}

void MediaPlayer::showMenu()
{
    mainMenu();
}

// Function show files in playlist
void MediaPlayer::showPlaylist()
{
    // Get count files in playlist
    int mediaCount = playlist->mediaCount();
    //Get current media
    QString curMedia = playlist->currentMedia().canonicalUrl().fileName();

    // Show all media in playlist
    for(int i = 0; i < mediaCount; i++) {
        // Get media filename
        QString media = playlist->media(i).canonicalUrl().fileName();
        // If media is the same as current media show >>
        if(media == curMedia)
            qDebug() << "|" << ">>" << i << ") " <<  media;
        else
            qDebug() << "|" << i << ") " <<  media;
    }
}

// Function show mainScreen with maincontrol menu
bool MediaPlayer::mainMenu()
{
    // Load header
    header();

    qDebug() << "[1] Graj," << "[2] Stop," << "[3] Pauza," << "\n"
             << "[4] Poprzedni" << "[5] Następny," << "\n"
             << "[6] Załaduje media";
    qDebug() << "[Co chcesz zrobić ?]";

    QTextStream get(stdin);
    QString ch;
    get >> ch;
    int choose = ch.toInt();

    qDebug() << "[Wybrałeś " << QString::number(choose) << "]";

    switch(choose)
    {
    case PlayerPlay:
        play();
        break;
    case PlayerStop:
        stop();
        break;
    case PlayerPause:
        pause();
        break;
    case PlayerPrevious:
        previous();
        break;
    case PlayerNext:
        next();
        break;
    case PlayerSelectMedia:
        selectMediaFolder();
        break;
    default:
        qDebug() << "[Nie ma takiej opcji]";
        //return false;
    }

    return true;
}

// Function show screen when user can be select/input
// folder. Function return true when playlist using selected path
// be success created.
bool MediaPlayer::selectMediaFolder()
{
    // Load header title, status etc
    header();


    qDebug() << "[SelectMedia] Odtwarzanie z:";
    qDebug() << "1) Jakiś folder ";
    #ifdef linux
        qDebug() << "2) USB";
    #endif

    QTextStream input(stdin);
    QString ch;

    // Get int from input
    input >> ch;
    int choose = ch.toInt();

    QDir path;

    if(choose == 1) {
        QTextStream input(stdin);
        QString url;
        qDebug() << "[Playlist] Podaj folder do mediów:";
        // Get url from user
        input >> url;
        // Set user path
        path.setPath(url);
    }

    if(choose == 2) {
        QStringList list;
#ifdef WIN32
        QString mediaPath = "C:/Users/";
#endif
#ifdef linux
        QString mediaPath = "/home/mateusz/Pobrane";
#endif
        QDirIterator dirs(mediaPath, QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

        list << dirs.next();

        while(dirs.hasNext())
            list << dirs.next();
        // Get folder count
        int count = list.length();

        if(count) {
            qDebug() << "[SelectMedia] Wybierz ścieżkę:";
            // Loop show any folder as been found
            for(int i = 0; i < count; i++)
                qDebug() << i << ") " << list.value(i);

            qDebug() << "[Co chcesz zrobić ?]";
            QTextStream input(stdin);
            QString ch;
            input >> ch;
            int choose = ch.toInt();

            // Set selected user path if is available from list
            if(!list.value(choose).isNull())
                path.setPath(list.value(choose));
        }
    }

    // Playlist is creating used selected path
    if(createPlaylist(path.path())) {
        qDebug()<<"[SelectMedia] Playlista stworzona.";
        return true;
    }else
        qDebug()<<"[SelectMedia] Błąd podczas tworzenia playlisty.";

    return false;
}

//Function return current media player status
QString MediaPlayer::getStatus()
{
    // Information about song - filename
    QString song = playlist->currentMedia().canonicalUrl().fileName();

    // Geting state from player
    int state = player->state();

    switch(state)
    {
    case QMediaPlayer::StoppedState:
        if(!playlist->mediaCount())
            return "Zatrzymany. Pusta playlista";
        break;
    case QMediaPlayer::PlayingState:
        return QString("Gram - " + song);
    case QMediaPlayer::PausedState:
        return QString("Wstrzymano - " + song);
    }

//    if(state == QMediaPlayer::StoppedState) {
//        if(!playlist->mediaCount())
//            return "Zatrzymany. Pusta playlista";
//        return "Zatrzymany";
//    }

//    if(state == QMediaPlayer::PlayingState)
//        return QString("Gram - " + song);

//    if(state == QMediaPlayer::PausedState)
//        return QString("Wstrzymano - " + song);

    // If state is unknown
    return "Unknown state";
}

// Function return current Playback status
QString MediaPlayer::getPlaybackStatus()
{
    // Geting playback state
    int state = playlist->playbackMode();

    switch(state)
    {
    case QMediaPlaylist::CurrentItemOnce:
        return "Playback - CurrentItemOnce";
    case QMediaPlaylist::CurrentItemInLoop:
        return "Playback - CurrentItemInLoop";
    case QMediaPlaylist::Sequential:
        return "Playback - Sequential";
    case QMediaPlaylist::Loop:
        return "Playback - Loop";
    case QMediaPlaylist::Random:
        return "Playback - Random";
    }

    // If state is unknown
    return "error";
}

bool MediaPlayer::createPlaylist(QString path)
{
    QDir dir;

    // Settings folder list
    dir.setPath(path);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Name);

    // Files supported
    dir.setNameFilters(QStringList("*.mp3"));

    // Check user dir is available to use
    if(dir.exists()) {
        qDebug() << "------------";

        // If playlist has medias must be cleared.
        if(playlist->mediaCount())
            playlist->clear();

        QFileInfoList list = dir.entryInfoList();

        // If path dir is one or more medias add all
        // If media files = 0 return func.
        if(list.size()) {
            qDebug() << "[Playlista] Lista dostępnych mediów:";
            for(int i = 0; i < list.size(); ++i)
            {
                QFileInfo file = list.at(i);

                bool added = playlist->addMedia(QUrl::fromLocalFile(path + "/" + file.fileName()));

                if(added)
                    qDebug() << "[Playlista] " + file.fileName() + " - added.";
                else
                    qDebug() << "[Playlista] " + file.fileName() + " - error.";
            }
            qDebug() << "[Playlista] Dodano " + QString::number(playlist->mediaCount()) + " plików muzycznych.";
            playlist->setCurrentIndex(0);
            play();
        }else {
            qDebug() << "[Playlista] Brak plików muzycznych.";
            return false;
        }
        qDebug() << "------------";
    }else {
        qDebug() << "[Playlist] Nieprawidłowa lokalizacja.";
        return false;
    }

    return true;
}

void MediaPlayer::play()
{
    qDebug()<<"[MediaPlayer] Gra...";
    player->play();
}

void MediaPlayer::pause()
{
    qDebug()<<"[MediaPlayer] Pauza.";
    player->pause();
}

void MediaPlayer::stop()
{
    qDebug()<<"[MediaPlayer] Zatrzymany.";
    player->stop();
}

void MediaPlayer::next()
{
    qDebug() << "[MediaPlayer] Następny...";
    player->playlist()->next();
}

void MediaPlayer::previous()
{
    qDebug() << "[MediaPlayer] Poprzedni ...";
    player->playlist()->previous();
}
