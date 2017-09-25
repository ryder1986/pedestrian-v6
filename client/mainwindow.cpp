#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "client.h"
//#include "camera.h"
//MainWindow::MainWindow(QWidget *parent) :
//    QMainWindow(parent),
//    ui(new Ui::MainWindow)
//{
//    ui->setupUi(this);
//    ui->groupBox->setFixedWidth(100);
//    ui->centralWidget->setLayout(ui->horizontalLayout);
//    ui->groupBox->setLayout(ui->gridLayout);
//    ui->widget->setLayout(ui->gridLayout_2);

//    cam_manager=new CameraManager();

////    clt=new client();
////    //CameraManager *cam=new CameraManager();
////    render=new YuvRender();
////    clt->get_client_setting();
////    Config *cfg=new Config;
////    Camera *cam=new Camera(cfg->data.camera[0]);
////    connect(&cam->src,SIGNAL(frame_update(Mat)),render,SLOT(set_mat(Mat) ));
//#if 0
//    QPushButton *b1=new QPushButton("1");
//    QPushButton *b2=new QPushButton("2");
//    QPushButton *b3=new QPushButton("3");
//    ui->gridLayout_2->addWidget(b1,1,1);
//    ui->gridLayout_2->addWidget(b2,2,1);
//    ui->gridLayout_2->addWidget(b3,2,2);
//#endif
// }

//MainWindow::~MainWindow()
//{
//          delete cam_manager;
//    delete ui;
//}

void MainWindow::on_pushButton_search_device_clicked()
{
    client->search_device();
    ui->lineEdit_search->setText(client->wait_server_info_reply());
}

void MainWindow::on_pushButton_connect_server_clicked()
{
  //  client->server_ip=ui->lineEdit_connect->text();
    client->server_ip=ui->lineEdit_search->text();
    ui->lineEdit_connect->setText( client->server_ip);
    client->connect_to_server();
}
#include "protocol.h"

void MainWindow::on_pushButton_get_config_clicked()
{
    int request_length=Protocol::encode_configuration_request(buf);
    QByteArray rst=client->call_server(buf,request_length);
    rst=rst.remove(0,Protocol::HEAD_LENGTH);//TODO:get the ret value

    cam_manager->save_config(rst);
    // cam_manager->cfg->set_ba(rst);
    cam_manager->reconfig_camera(ui->gridLayout_2);
    //  cam_manager->cfg->save();
    // YuvRender *r=new YuvRender();
    //  connect(cam_manager->cams,SIGNAL())

    ui->lineEdit_get->setText( cam_manager->get_config());
}




void MainWindow::on_pushButton_add_clicked()
{
    QString ip=ui->lineEdit_add->text();
    cam_manager->add_camera(ip);
    QByteArray setting=cam_manager->get_config(1);
    int len=Protocol::encode_addcam_request(buf,setting.length());
    memcpy(buf+Protocol::HEAD_LENGTH,setting.data(),setting.length());
    QByteArray rst=client->call_server(buf,len);
    cam_manager->reconfig_camera(ui->gridLayout_2);
}



void MainWindow::on_pushButton_del_clicked()
{
    QString index=ui->lineEdit_del->text();
    if(index.toInt()<=cam_manager->get_size()&&index.toInt()>0)
    {
        cam_manager->del_camera(index.toInt());
        Protocol::encode_delcam_request(buf,index.toInt());
        client->call_server(buf,Protocol::HEAD_LENGTH);
    }
     cam_manager->reconfig_camera(ui->gridLayout_2);
}
