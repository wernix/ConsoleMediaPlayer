#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QDebug>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QObject>
#include <QDir>
#include <iostream>
#include <QFileInfoList>
#include <QDirIterator>

// Media Player Controls
enum PlayerControl {
    PlayerPlay = 1,
    PlayerStop,
    PlayerPause,
    PlayerPrevious,
    PlayerNext,
    PlayerSelectMedia
};

class MediaPlayer : public QObject
{
    Q_OBJECT

public:
    MediaPlayer(QObject *parent = 0);
    ~MediaPlayer();
    bool createPlaylist(QString);
    QMediaPlayer *player;
    QString getStatus();
    QString getPlaybackStatus();

    bool selectMediaFolder();
    void run();

public slots:
    void play();
    void pause();
    void stop();
    void previous();
    void next();
    void showMenu();


private:
    QMediaPlaylist *playlist;
    void header();
    bool mainMenu();
    void showPlaylist();


};

#endif // MEDIAPLAYER_H
