#include "remote.h"
#include "mediaplayer.h"

Remote::Remote(QObject *parent) : QObject(parent)
{
    qDebug()<<"[Remote] Inicjacja...";

    // Create client to capture commands from
    // users connected to MediaPlayer
    connect(&server, SIGNAL(newConnection()),
      this, SLOT(acceptConnection()));

    // Set listening parametrs IP and PORT
    server.listen(QHostAddress::LocalHost, 9999);

    if(server.isListening())
        qDebug()<<"[Remote] Uruchomiono na porcie 9999.";
    else
        qDebug()<<"[Remote] Błąd. Nie nasłuchuje.";
}

Remote::~Remote()
{
    server.close();
}


void Remote::acceptConnection()
{
  client = server.nextPendingConnection();

  connect(client, SIGNAL(readyRead()),
    this, SLOT(startRead()));

  // Slot sends to remote app current status
  connect(player->player, SIGNAL(stateChanged(QMediaPlayer::State)),
          this, SLOT(changeCurStatus()));
}

void Remote::changeCurStatus()
{
    control("check_status");
}

// Function reading data/commands from users connected
// to CMP
void Remote::startRead()
{
  char buffer[1024] = {0};
  client->read(buffer, client->bytesAvailable());

  QString qBuffer = buffer;

  qDebug()<< "[Remote] RemoteApp wysyła komendę " + qBuffer + ".";

  control(qBuffer);

}

void Remote::setControl(MediaPlayer *mediaPlayer)
{
    player = mediaPlayer;
}

void Remote::control(QString command)
{
    if(command == "play") {
        player->play();
        return;
    }

    if(command == "pause") {
        player->pause();
        return;
    }

    if(command == "stop") {
        player->stop();
        return;
    }

    if(command == "check_status") {
        QString status = player->getStatus();

        client->write(status.toStdString().c_str());

        if(client->waitForBytesWritten(1000))
            qDebug() << "[RemoteApp] Status wysłany do aplikacji. /" << status << endl;

        return;
    }

    qDebug()<<"[Remote] ?? Nieznane polecenie.";
}
