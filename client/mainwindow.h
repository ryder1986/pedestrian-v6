#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "client.h"
#include "protocol.h"
#include "config.h"
//#include "camera.h"
//#include "client.h"
#include "yuvrender.h"
#include "camera.h"
#include "videosrc.h"

class ClientCameraManager:public CameraManager{

public:
    ClientCameraManager() :CameraManager("/root/repo-github/pedestrian-v6/client/config.json")
    {
        //     p_cfg=new Config(":/config.json");
    }
    ~ClientCameraManager()
    {

    }

    void reconfig_camera(QGridLayout *g)
    {
        QList <Camera *> &c=get_cam();
        use_camera_config();
        int size=get_size();
        for(int i=0;i<size;i++){
            QWidget *render=(c[i])->get_render();
            g->addWidget(render,(i)/4,(i)%4);
        }

    }


private:

};

namespace Ui {
class MainWindow;
}
/*
   client:manage network connection with server. provide cmd like add,del,modify camera through server
   camera manager: manage cfg file,manage camera widget layout and rendering
*/
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit  MainWindow(QWidget *parent=0) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
    {
        ui->setupUi(this);
        ui->groupBox->setFixedWidth(300);
        ui->centralWidget->setLayout(ui->horizontalLayout);
        ui->groupBox->setLayout(ui->gridLayout);
        ui->widget->setLayout(ui->gridLayout_2);

        cam_manager=new ClientCameraManager();
        client=new Client();
        cam_manager->reconfig_camera(ui->gridLayout_2);
#if 0
        QPushButton *b1=new QPushButton("1");
        QPushButton *b2=new QPushButton("2");
        QPushButton *b3=new QPushButton("3");
        ui->gridLayout_2->addWidget(b1,1,1);
        ui->gridLayout_2->addWidget(b2,2,1);
        ui->gridLayout_2->addWidget(b3,2,2);
#endif
    }

    ~MainWindow()
    {
        delete cam_manager;
        delete ui;
    }


private slots:
    void test(int t)
    {
        qDebug()<<"get pic "<<t;
    }


    void on_pushButton_search_device_clicked();

    void on_pushButton_connect_server_clicked();



    void on_pushButton_get_config_clicked();


    void on_pushButton_add_clicked();


    void on_pushButton_del_clicked();

private:
    //    client *  clt;
    Ui::MainWindow *ui;
    ClientCameraManager *cam_manager;
    Client *client;
    char buf[BUF_MAX_LEN];
    //   YuvRender *render;
    //  QList <YuvRender *> renders;
};

#endif // MAINWINDOW_H



