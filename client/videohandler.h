#ifndef VIDEOHANDLER_H
#define VIDEOHANDLER_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <QObject>
#include "yuvrender.h"
using namespace cv;
class VideoHandler : public QObject
{
    Q_OBJECT
public:
    explicit VideoHandler(QObject *parent = 0){
        p_render=new YuvRender();
      //  p_render->show();
    }
    ~VideoHandler()
    {

        delete p_render;
    }

    void set_frame(Mat *frm){
        p_render->render_set_mat(*frm);
    }
    void work(char *)
    {

    }
    QWidget *get_render()
    {
       return p_render;
    }

signals:

public slots:
private:
    YuvRender *p_render;
};

#endif // VIDEOHANDLER_H
