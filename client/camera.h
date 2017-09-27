#ifndef CAMERA_H
#define CAMERA_H
/*
    well,there are two policies,one is emit buffer when VideoSrc avilable, one is timer emit fetching from VideoSrc per xx msecond.

*/
#include <QTimer>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <QObject>
#include <QMutex>
#include <QThread>
#include "config.h"
#include "videosrc.h"
#include "videohandler.h"
#include <QMutex>
using namespace cv;
using namespace std;
//class CameraManager;
/*
    get mat from src  every XX msec , give mat to video handler
    if video src fail to create  or fail to fetch mat, signal will send to this camera ,
    frequency will turn down, and try restart video src per XXX sencond
*/

class Camera : public QThread
{
    Q_OBJECT
public:
    bool quit_work;
    explicit Camera(camera_data_t dat) : data(dat),p_video_src(NULL)
    {
        quit_work=false;
        tick_last=tick=0;
        tick_work=0;
        start_video_src();
        //      connected=false;
   //     create_video_src();
        //      emit restart_source();
    }
    ~Camera(){
        if(quit_work==false){
            prt(info,"waitin for thread quit");
            quit_work=true;
            //     QThread::msleep(1000);
        }
        //   this->quit();
        //      this->exit();
        this->wait();//wait run done
        delete p_video_src;
        p_video_src=NULL;
    }

    void start_video_src()
    {
        timer=new QTimer();
        connect(timer,SIGNAL(timeout()),this,SLOT(tick_check_frame_rate()));
        timer->start(1000);
        //prt(info,"1");
        work_lock.lock();// lock need because we use videosrc(function "run" use it too)
        p_video_src=new VideoSrc(data.ip);
//        connect(this,SIGNAL(restart_source()),this,SLOT(restart_video()),Qt::BlockingQueuedConnection);
        connect(this,SIGNAL(restart_source()),this,SLOT(restart_video()));
        quit_work=false;
        this->start(NormalPriority);
        //    connect(this,SIGNAL(restart_source()),this,SLOT(restart_video()));
        work_lock.unlock();
    }
    void close_video_src()
    {
        disconnect(timer,SIGNAL(timeout()),this,SLOT(tick_check_frame_rate()));
        delete timer;

        quit_work=true;
       work_lock.lock();//
        disconnect(this,SIGNAL(restart_source()),this,SLOT(restart_video()));
     //   this->exit();
          this->quit();//tell run to quit, TODO, why i need quit while manully?
       this->wait();// wait run  quit
        delete p_video_src;
         work_lock.unlock();
    }

    void create_video_src()
    {
        work_lock.lock();//dont work when creating video source

        prt(info,"restart %s done",data.ip.toStdString().data());
        if(p_video_src!=NULL)
        {
            //            disconnect(p_video_src,SIGNAL(video_disconnected()),this,SLOT(source_disconnected()));
            //            disconnect(p_video_src,SIGNAL(video_connected()),this,SLOT(source_connected()));
            disconnect(this,SIGNAL(restart_source()),this,SLOT(source_disconnected()));
            disconnect(p_video_src,SIGNAL(video_disconnected()),this,SLOT(source_disconnected()));

            delete p_video_src;
            p_video_src=NULL;
        }
        p_video_src=new VideoSrc(data.ip);
        //        connect(p_video_src,SIGNAL(video_disconnected()),this,SLOT(source_disconnected()));
        //        connect(p_video_src,SIGNAL(video_connected()),this,SLOT(source_connected()));

        connect(this,SIGNAL(restart_source()),this,SLOT(source_disconnected()),Qt::BlockingQueuedConnection);
        connect(p_video_src,SIGNAL(video_disconnected()),this,SLOT(source_disconnected()));
        //   connect(this,SIGNAL(restart_source()),this,SLOT(source_connected()));

        //        if(p_video_src->video_connected_flag==true)
        //            source_connected();
        //        else
        //            source_disconnected();
        prt(info,"restart %s done",data.ip.toStdString().data());
        work_lock.unlock();
    }

    void restart(camera_data_t dat)
    {
        data=dat;
    }
#ifdef CLIENT
    QWidget *get_render()
    {
        return video_handler.get_render();
    }
#endif
protected:
    virtual void run()
    {
        while(quit_work==false)
        {
            //         prt(info,"runing %s",data.ip.toStdString().data());
            if(work()!=true){

                //    create_video_src();
                    //prt(info,"restarting  %s",data.ip.toStdString().data());
                emit restart_source();
             //    prt(info,"restart  %s",data.ip.toStdString().data());
                QThread::msleep(100);
                //      break;
            }
            QThread::msleep(1);
        }
    }

signals:
    void restart_source();
    void output(QByteArray ba);
public slots:
    void tick_check_frame_rate()
    {
        prt(info,"frame rate %d",tick-tick_last);
        tick_last=tick;
    }

    //    void source_connected()
    //    {
    //        prt(info,"video connected");
    //        connected=true;
    //    }
    void restart_video()
    {
    //    prt(info,"1restarting   %s",data.ip.toStdString().data());
        close_video_src();
       // prt(info,"2restarting   %s",data.ip.toStdString().data());

        start_video_src();

     //   prt(info,"3restarting   %s",data.ip.toStdString().data());

    }

    void source_disconnected()
    {//   lock.lock();
        //  if(connected==true){//avoid multiple call
        prt(info,"video disconnected");
        //       connected=false;
        create_video_src();
        // }
        // lock.unlock();
    }
    /*
    video src -> fetch mat
    video handler -> handle mat
    timer (always alive) work per xx mseconds
    */
    bool work()
    {
        work_lock.lock();
        QByteArray ba;
        ba.append(p_video_src->get_url());
        bool ret=true;
        //      if(connected==true){
        if(p_video_src!=NULL){

            //            if(tick==200){
            //                // prt(info,"restart video");
            //                //   std::this_thread::sleep_for(chrono::milliseconds(1000));
            //                //            delete p_video_src;
            //                //            //       std::this_thread::sleep_for(chrono::milliseconds(1000));
            //                //            //     p_video_src=new VideoSrc(data.ip);
            //                //            p_video_src=create_video_src();
            //                create_video_src();
            //                tick=0;
            //            }
            //    IplImage *f=p_video_src->fetch_frame();
            Mat *f=p_video_src->get_frame();
            if(1){
                if(f==NULL){
                    prt(info,"get null frame, need to restart video");
                    //    source_disconnected();
                    ret=false;
                }else
                {
                    video_handler.set_frame(f);
                    video_handler.work(ba);
              //      prt(info,"get   video");
                }
            }else{
                //      prt(info,"sleep start");
                //     std::this_thread::sleep_for(chrono::milliseconds(2000));
                //    prt(info,"sleep end");
            }
        }else{
            prt(info,"%s , work ignored cuz unconnected",data.ip.toStdString().data());
            //    source_disconnected();
            ret=false;
        }
        if(ret==true){
               emit  output(ba);
               tick++;
             //  prt(info,"tick now %d  %d" ,  tick,tick_last);
        }
        work_lock.unlock();
          //prt(info,"ret %d",ret);
        return ret;
    }
private:
    QTimer *timer;
    camera_data_t data;//data that camera need
    //    QTimer *timer;//do work per xx micro seconds
    VideoSrc*p_video_src;//camera frame source
    VideoHandler video_handler;//camera frame handler
    int tick;
    int tick_last;
    int tick_work;
    //   bool connected=false;
    QList <IplImage> frame_list;
    QMutex work_lock;
    Mat mt;
    //   QThread fetch_thread;
};


class CameraManager:public QObject{
    Q_OBJECT
public:

    //    CameraManager(){
    //        p_cfg=new Config("/root/repo-github/pedestrian-v1/server/config.json");
    //        //     p_cfg=new Config();
    //        for(int i=0;i<p_cfg->data.camera_amount;i++){
    //            Camera *c=new Camera(p_cfg->data.camera[i]);
    //            cams.append(c);
    //        }
    //    }
    CameraManager(char * url){

        p_cfg=new Config(url);
        use_camera_config();
    }
    ~CameraManager(){

        for(int i=0;i<p_cfg->data.camera_amount;i++){
            // delete cams[i];
            del_camera_internal(i);
        }
    }

    void use_camera_config()
    {
        foreach (Camera *c, cams) {
            delete c;
        }
        int num;
        cams.clear();
        for(int i=0;i<p_cfg->data.camera_amount;i++){
            //            Camera *c=new Camera(p_cfg->data.camera[i]);
            //            cams.append(c);
            add_camera_internal(i);
            //  if(i==0)
            //    connect(c->p_src,SIGNAL(frame_update(Mat)),&c->render,SLOT(set_mat(Mat)));
            //   if(i==0)
            //   layout->addWidget(&c->render,i,i);
        }
        num=cams.size();
    }
    QList <Camera *>  &get_cam()
    {
        return cams;
    }
    void save_config(QByteArray buf)
    {
        p_cfg->set_ba((buf));
    }

public slots:
    void camera_output(QByteArray ba)
    {
        emit output_2_client(ba);
    }

    void add_camera(QByteArray buf)
    {
        p_cfg->set_ba((buf));
        //        Camera *c=new Camera(p_cfg->data.camera[p_cfg->data.camera_amount-1]);
        //        cams.append(c);
        add_camera_internal();
    }
    void add_camera_internal()
    {
        //        Camera *c=new Camera(p_cfg->data.camera[p_cfg->data.camera_amount-1]);
        //        cams.append(c);
        add_camera_internal(p_cfg->data.camera_amount-1);

    }


    void add_camera_internal(int index)
    {
        Camera *c=new Camera(p_cfg->data.camera[index]);
        connect(c,SIGNAL(output(QByteArray)),this,SLOT(camera_output(QByteArray)));
        cams.append(c);
     //   c->start();
        prt(info,"cam %d append",index);
    }
    void del_camera_internal(int index)
    {
        disconnect(cams[index],SIGNAL(output(QByteArray)),this,SLOT(camera_output(QByteArray)));
        delete cams[index];
        cams.removeAt(index);
        prt(info,"cam %d deleted",index);
    }


    void add_camera(QString ip)
    {
        //         Camera *c=new Camera(cfg.data.camera[i]);

        //        camera_data_t ca;
        //        ca.ip=ip;
        //        ca.port=554;
        //        p_cfg->data.camera.append(ca);
        //        p_cfg->data.camera_amount++;

        p_cfg->append_camera(ip,554);
        add_camera_internal();
        //        Camera *c=new Camera(p_cfg->data.camera[p_cfg->data.camera_amount-1]);
        //        cams.append(c);
        //  p_cfg->save();
        //  if(i==0)
        //    connect(c->p_src,SIGNAL(frame_update(Mat)),&c->render,SLOT(set_mat(Mat)));
        //   if(i==0)
        //    layout->addWidget(&c->render,1,cams.length()-1);
    }
    void del_camera(int cam_no)
    {
        if(cam_no<=p_cfg->data.camera_amount&&cam_no>0){
            //            p_cfg->data.camera.removeAt(index-1);
            //            p_cfg->data.camera_amount--;
            //            p_cfg->save_config_to_file();
            p_cfg->del_camera(cam_no);
            del_camera_internal(cam_no-1);
            //            delete cams[index-1];
            //            cams.removeAt(index-1);
        }
    }


    void modify_camera(int index)
    {
        cams[index]->restart(p_cfg->data.camera[index-1]);
    }
    int get_config(char *c)
    {
        QByteArray b(p_cfg->get_ba());
        int len=b.length();
        memcpy(c,b.data(),len);
        return len;
    }
    QString get_config()
    {
        QByteArray b(p_cfg->get_ba());
        QString str(b);
        return str;
    }
    QByteArray get_config(int i)
    {
        QByteArray b(p_cfg->get_ba());

        return b;
    }

    int get_size()
    {
        return cams.size();
    }
signals:
    void output_2_client(QByteArray ba);
private:

    QList <Camera *> cams;//cameras that opened, all cameras is working,or trying to work
    Config *p_cfg;//all the setting on this server
};


#endif // CAMERA_H
