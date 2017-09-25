#ifndef SERVER_H
#define SERVER_H

#include <QObject>
//#include"protocol.h"
#include <QObject>
#include <QTimer>
#include <QtNetwork>
#include <QNetworkInterface>
#include "common.h"
#include"protocol.h"
#include "camera.h"
#include "serverinforeporter.h"
//#include "common.h"
//#include "camera.h"


class ClientSession:public QObject{
    Q_OBJECT
public:
    ClientSession(QTcpSocket *client_skt,CameraManager *p):skt(client_skt),p_manager(p){
        connect(skt,SIGNAL(readyRead()),this,SLOT(real_reply()));
        connect(skt,SIGNAL(disconnected()),this,SLOT(deleteLater()));

        udp_skt=new QUdpSocket();
        // QHostAddress a;
        // udp_skt->bind(a,12349);
        timer=new QTimer();
        //     connect(timer,SIGNAL(timeout()),this,SLOT(send_rst_to_client()));
        timer->start(1000);
        client_addr=skt->peerAddress();

    }
public slots:
    void send_rst_to_client()
    {
        udp_skt->writeDatagram("1231",client_addr,12341);
    }

    void simple_reply()
    {
        QByteArray client_buf=skt->readAll();


        QByteArray block;
        block.append(client_buf[0]+1);
        //        QString str("1234567890");
        //        block.append(str);
        skt->write(block);
        //  skt->disconnectFromHost();
    }
    void welcom_reply(){

        QByteArray block;
        QString str("welcom client ");
        block.append(str);
        skt->write(block);

    }

    void real_reply(){
        // CameraManager *pa=(CameraManager *)pt;
        //   skt->waitForReadyRead();
        int writes_num=0;

        QByteArray client_buf=skt->readAll();
        //       int len=client_buf.length();
        int ret=0;
        int cmd=Protocol::get_operation(client_buf.data());
        int pkg_len=Protocol::get_length(client_buf.data());
        int cam_index=Protocol::get_index(client_buf.data());
        memset(buf,0,BUF_MAX_LEN);
        QByteArray bta;
        switch (cmd) {
        case Protocol::ADD_CAMERA:
            prt(info,"protocol :adding   cam");

            bta.clear();
            bta.append((char *)client_buf.data()+Protocol::HEAD_LENGTH,pkg_len);
            p_manager->add_camera(bta);
            writes_num=skt->write(buf,ret+Protocol::HEAD_LENGTH);

            //     p_manager->add_camera();
            break;
        case Protocol::GET_CONFIG:
            prt(info,"protocol :send config");
#if 1
            ret= p_manager->get_config(buf+Protocol::HEAD_LENGTH);
            Protocol::encode_configuration_reply(buf,ret,Protocol::RET_SUCCESS);
            writes_num=skt->write(buf,ret+Protocol::HEAD_LENGTH);
#else
            ret= p_manager->get_config(buf);
            skt->write(buf,ret);
#endif
            break;
        case Protocol::DEL_CAMERA:
            prt(info,"protocol :deling    cam %d ",cam_index);
            p_manager->del_camera(cam_index);
            writes_num=skt->write(buf,ret+Protocol::HEAD_LENGTH);

            break;
        default:
            break;
        }

    }

    void displayError(QAbstractSocket::SocketError socketError)
    {
        switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
            break;
        case QAbstractSocket::HostNotFoundError:
            prt(info,"err");
            break;
        case QAbstractSocket::ConnectionRefusedError:
            prt(info,"err");
            break;
        default:break;

        }


    }
signals :
    int get_server_config(char *buf);

private:
    char buf[BUF_MAX_LEN];
    QTcpSocket *skt;
    CameraManager *p_manager;
    QUdpSocket *udp_skt; QTimer *timer;
    QHostAddress client_addr;
};
/*
    server provide
        tcpserver:handle clients session
        clients:hold all ClientSession
        systeminforeporter:recive udp request,and respond with ip ,netmask ,etc...
        camera manager:holds configuration , runing camera,manage add,delete and other operation

*/
class Server : public QObject
{
    Q_OBJECT
public:
    explicit Server(QObject *parent=0 ):QObject(parent){
        cam_manager=new CameraManager("/root/repo-github/pedestrian-v6/server/config.json");
        reporter=new ServerInfoReporter();
        bool ret=false;
        server=new QTcpServer();
        ret=server->listen(QHostAddress::Any,Protocol::SERVER_PORT);
        if(ret){
            prt(info,"ok to listen on port %d",Protocol::SERVER_PORT);
        }else
        {
            prt(info,"err in listen on port %d",Protocol::SERVER_PORT);
            exit(1);
        }
        connect(server, &QTcpServer::newConnection, this, &Server::handle_incomimg_client);
    }
    ~Server(){
        delete reporter;
        delete cam_manager;
        delete server;
    }
    void print_server_info()
    {
  //     prt(info,"server started ");
    }

signals:
public slots:
    void handle_incomimg_client()
    {
        QTcpSocket *skt = server->nextPendingConnection();
        connect(skt, SIGNAL(disconnected()),skt, SLOT(deleteLater()));
        QString str(skt->peerAddress().toIPv4Address()>>28);
        prt(info,"client %s:%d",str.data(),skt->peerPort());
        ClientSession *client=new ClientSession(skt,this->cam_manager);
        clients.append(client);
        connect(client,SIGNAL(get_server_config(char *)),cam_manager,SLOT(get_config(char *)));
    }
    void client_connected()
    {
        QTcpSocket *skt = server->nextPendingConnection();
        connect(skt, SIGNAL(disconnected()), skt, SLOT(deleteLater()));
    }
private:
    CameraManager *cam_manager;//manage all cameras
    ServerInfoReporter *reporter;//repy query for system info
    QTcpServer *server;//server for reply all clients request ,execute client cmds,like add cam,del cam, reconfigure cam,etc..
    QList <ClientSession *> clients;//client that connected to server
};

#endif // SERVER_H
