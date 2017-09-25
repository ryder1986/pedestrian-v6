#ifndef YUVRENDER_H
#define YUVRENDER_H


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <QImage>
#include "config.h"
using namespace cv;
using namespace std;
#include <QGLWidget>
#include "common.h"
class YuvRender : public QGLWidget
{
    Q_OBJECT
public:

    // explicit YuvRender(QWidget *parent = 0);
    YuvRender(QWidget *parent=0) :
        QGLWidget(parent),video_width(VIDEO_WIDTH),video_height(VIDEO_HEIGHT),pressed_x(0),pressed_y(0)
    {
        tick=0;
    }


//    void  set_buf(char *buffer)
//    {
//        memcpy(yuv_buf,buffer,VIDEO_WIDTH*VIDEO_HEIGHT*3/2);
//    }

    //    void  initializeGL()
    //    {

    //    }

    //    void  resizeGL(int w, int h)
    //    {

    //    }





    static unsigned char  CONVERT_ADJUST(double tmp)
    {
        return (unsigned char)((tmp >= 0 && tmp <= 255)?tmp:(tmp < 0 ? 0 : 255));
    }
    //YUV420P to RGB24
    static  void   CONVERT_YUV420PtoRGB24(unsigned char* yuv_src,unsigned char* rgb_dst,int nWidth,int nHeight)
    {
        unsigned char *tmpbuf=(unsigned char *)malloc(nWidth*nHeight*3);
        unsigned char Y,U,V,R,G,B;
        unsigned char* y_planar,*u_planar,*v_planar;
        int rgb_width , u_width;
        rgb_width = nWidth * 3;
        u_width = (nWidth >> 1);
        int ypSize = nWidth * nHeight;
        int upSize = (ypSize>>2);
        int offSet = 0;

        y_planar = yuv_src;
        u_planar = yuv_src + ypSize;
        v_planar = u_planar + upSize;

        for(int i = 0; i < nHeight; i++)
        {
            for(int j = 0; j < nWidth; j ++)
            {
                // Get the Y value from the y planar
                Y = *(y_planar + nWidth * i + j);
                // Get the V value from the u planar
                offSet = (i>>1) * (u_width) + (j>>1);
                V = *(u_planar + offSet);
                // Get the U value from the v planar
                U = *(v_planar + offSet);

                // Cacular the R,G,B values
                // Method 1
                R = CONVERT_ADJUST((Y + (1.4075 * (V - 128))));
                G = CONVERT_ADJUST((Y - (0.3455 * (U - 128) - 0.7169 * (V - 128))));
                B = CONVERT_ADJUST((Y + (1.7790 * (U - 128))));
                /*
                  // The following formulas are from MicroSoft' MSDN
                  int C,D,E;
                  // Method 2
                  C = Y - 16;
                  D = U - 128;
                  E = V - 128;
                  R = CONVERT_ADJUST(( 298 * C + 409 * E + 128) >> 8);
                  G = CONVERT_ADJUST(( 298 * C - 100 * D - 208 * E + 128) >> 8);
                  B = CONVERT_ADJUST(( 298 * C + 516 * D + 128) >> 8);
                  R = ((R - 128) * .6 + 128 )>255?255:(R - 128) * .6 + 128;
                  G = ((G - 128) * .6 + 128 )>255?255:(G - 128) * .6 + 128;
                  B = ((B - 128) * .6 + 128 )>255?255:(B - 128) * .6 + 128;
                  */
                offSet = rgb_width * i + j * 3;

                rgb_dst[offSet] = B;
                rgb_dst[offSet + 1] = G;
                rgb_dst[offSet + 2] = R;
            }
        }
        free(tmpbuf);
    }



    void paint_layout1(QPainter &painter){
#if 0
        painter.beginNativePainting();
        makeCurrent();
        CONVERT_YUV420PtoRGB24((unsigned char *)yuv_buf,(unsigned char *)rgb_buf,video_width,video_height);
        glPixelZoom(1.0f*this->width()/video_width, 1.0f*this->height()/video_height);
#if 0
        glDrawPixels(video_width, video_height,GL_RGB, GL_UNSIGNED_BYTE, rgb_buf);
#else
        QImage img((uchar *)rgb_buf,video_width,video_height,QImage::Format_RGB888);
        //  QImage img((uchar *)rgb_buf,video_width,video_height,QImage::Format_RGB888);
        //QImage  img = QImage((const uchar*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_Indexed8);
        //   QImage  img = QImage((const uchar*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_Grayscale8);

        painter.drawImage(QRect(0,0,this->width(),this->height()),img);
#endif
        painter.endNativePainting();
#else
        painter.beginNativePainting();
        makeCurrent();
        //   QImage  img = QImage((const uchar*)\
        (frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),\
                QImage::Format_Indexed8);
        //   QImage  img = QImage((const uchar*)\
        (frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),\
                QImage::Format_RGB888);
      //  char t=*frame.data;

        Mat rgb_frame=frame;
        // cvtColor(frame,rgb_frame,CV_YUV2BGR);
        Mat yuv_frame;
     //     cvtColor(rgb_frame,yuv_frame,CV_RGB2GRAY);
       cvtColor(rgb_frame,yuv_frame,CV_BGR2RGB);
       QImage  img = QImage((const uchar*)(yuv_frame.data),
                             yuv_frame.cols,yuv_frame.rows,
                             yuv_frame.cols*yuv_frame.channels(),
                             QImage::Format_RGB888);
        painter.drawImage(QRect(0,0,this->width(),this->height()),img);
        painter.endNativePainting();

#endif
    }
    void paint_layout2(QPainter &painter){
#if 0
        QBrush red_brush_trans(QColor(0,0,200,100));
        painter.setBrush(red_brush_trans);
        // pressed_x=0;
        // pressed_y=0;
        //  if(flg++>10)


        painter.drawEllipse(pressed_x+tick%300,pressed_y,100,100);
        QPen testpen(red_brush_trans,10);
        painter.setPen(testpen);

        for(int i=0;i<8;i++){
            painter.drawLine(pt[i*2],pt[i*2+1]);
        }


        for(int i=0;i<8;i++){
            painter.drawLine(pt[i*2],pt[i*2+1]);
        }

        for(int i=0;i<8;i++){
            painter.drawLine(pt_rear[i*2],pt_rear[i*2+1]);
        }
        painter.drawEllipse(p1,10,10);
        painter.drawEllipse(p2,10,10);
        painter.drawEllipse(p3,10,10);
#endif
    }

    void paintEvent(QPaintEvent *)
    {
        int len=frame.cols;
        if(frame.cols>0){
             tick++;
             //prt(info,"painting");
            tick+=10;
            QPainter painter(this);
            paint_layout1(painter);
        }
        //  paint_layout2(painter);
    }
    //protected:
    //    void paintEvent(QPaintEvent *)
    //    {
    //        qDebug()<<"paint "<<tick++;
    //        tick+=10;
    //        QPainter painter(this);



    //        painter.beginNativePainting();
    //        makeCurrent();
    //        CONVERT_YUV420PtoRGB24((unsigned char *)yuv_buf,(unsigned char *)rgb_buf,video_width,video_height);
    //        glPixelZoom(1.0f*this->width()/video_width, 1.0f*this->height()/video_height);
    //#if 0
    //        glDrawPixels(video_width, video_height,GL_RGB, GL_UNSIGNED_BYTE, rgb_buf);
    //#else
    //        QImage img((uchar *)rgb_buf,video_width,video_height,QImage::Format_RGB888);
    //        //  QImage img((uchar *)rgb_buf,video_width,video_height,QImage::Format_RGB888);
    //        //QImage  img = QImage((const uchar*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_Indexed8);
    //        //   QImage  img = QImage((const uchar*)(frame.data),frame.cols,frame.rows,frame.cols*frame.channels(),QImage::Format_Grayscale8);

    //        painter.drawImage(QRect(0,0,this->width(),this->height()),img);
    //#endif
    //        painter.endNativePainting();



    //        QBrush red_brush_trans(QColor(0,0,200,100));
    //        painter.setBrush(red_brush_trans);
    //        // pressed_x=0;
    //        // pressed_y=0;
    //        //  if(flg++>10)


    //        painter.drawEllipse(pressed_x+tick%300,pressed_y,100,100);
    //        QPen testpen(red_brush_trans,10);
    //        painter.setPen(testpen);

    //        for(int i=0;i<8;i++){
    //            painter.drawLine(pt[i*2],pt[i*2+1]);
    //        }


    //        for(int i=0;i<8;i++){
    //            painter.drawLine(pt[i*2],pt[i*2+1]);
    //        }

    //        for(int i=0;i<8;i++){
    //            painter.drawLine(pt_rear[i*2],pt_rear[i*2+1]);
    //        }
    //        painter.drawEllipse(p1,10,10);
    //        painter.drawEllipse(p2,10,10);
    //        painter.drawEllipse(p3,10,10);
    //    }
    void  initializeGL()
    {

    }

signals:

public slots:
    int render_set_mat(Mat frame_mat)
    {
        int size=frame_mat.total();
      //  prt(info,"%d",size);
 // char tmp2=*f.data;
 // prt(info,"tick %d",tickmat++);
//        if(tickmat >590&&tickmat<610)
//            return 1;
//   prt(info,"render set frame  ");
        if(size>0)
        {
       //     prt(info,"render set frame ok");
            frame=frame_mat;
     //       prt(info,"%d",sizeof(f.data));
            //char tmp1=*f.data;
            //char tmp=*frame.data;
        }else
        {

            prt(info,"render set frame fail");
            frame.resize(640*480*3);
            frame=Mat(640,480,CV_8UC3);

            memset(frame.data,255,640*480*3);
        //    char tmp1=*f.data;
         //   char tmp=*frame.data;

        }
        this->update();
    //    prt(info,"update pic menually");
        return 1;
    }

private:
    int tmp_tick;
    char yuv_buf[640*480*3/2];
    char rgb_buf[640*480*3];
    char rgb_buf_tmp[640*480*3];
    int video_width;
    int video_height;
    int pressed_x,pressed_y,pressed;
    int tick;
    int tickmat;
    Mat frame;


    QPoint pt[16];
    QPoint pt_rear[16];
    QPoint p1;
    QPoint p2;
    QPoint p3;
protected:
    //  void  initializeGL();
    //  void  resizeGL(int w, int h);
    //  void  paintEvent(QPaintEvent *);


};

#endif // YUVRENDER_H
