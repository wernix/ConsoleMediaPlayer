#include <mediaplayer.h>
#include <remote.h>
#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    //Remote server;
    MediaPlayer player;
    //server.setControl(&player);

    // Check that argument is full path to folder
    if(QDir::isAbsolutePath(argv[1])) {
        // Check and add content medias to playlist
        if(player.createPlaylist(argv[1])) {
            qDebug()<<"[Main] Playlist created.";
            player.play();
        }else
            qDebug()<<"[Main] Playlist error, not created.";
    }

    player.run();
    return a.exec();
}
