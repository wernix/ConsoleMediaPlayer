#ifndef REMOTE_H
#define REMOTE_H

#include "mediaplayer.h"

#include <QtNetwork>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QObject>


class Remote : public QObject
{
Q_OBJECT
public:
    Remote(QObject *parent = 0);
    ~Remote();
    MediaPlayer *player;
    void setControl(MediaPlayer*);

public slots:
  void acceptConnection();
  void startRead();
  void changeCurStatus();

private:
    QTcpServer server;
    QTcpSocket *client;

    void control(QString);

};

#endif // REMOTE_H
