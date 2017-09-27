#ifndef PROTOCOL_H
#define PROTOCOL_H
#define BUF_MAX_LEN 1000
//#define SERVER_PORT_MAIN 12345
//#define SERVER_PORT_BROADCAST_CHECK 12346
//#define CLIENT_PORT_BROADCAST_CHECK 12347
//#define SERVER_PORT_ALG_RST 12348
/*

head defination:

    /    head  /   /  body   /
    -- -- -- --    ---.......
    byte1-2:data body length,byte 3-4:protocol version, byte 5-6:operation,byte 7-8:reply rst,9->:body

*/


//#define GET_CONFIG 0
//#define ADD_CAMERA 1//add camera at tail
//#define DEL_CAMERA 2//del camera
//#define SET_ENV 3//del camera
//#define GET_ENV 4//del camera

//#define RET_SUCCESS 0
//#include "QByteArray"
class Protocol{
public :
//    #define VERSION 1
    enum VER{
        VERSION=1
    };
//    static int head_length=8;
  //  #define HEAD_LENGTH 8
    enum LEN{
        HEAD_LENGTH=8
    };

    enum AABB{
        SS=0,
        PP=1
    };
    enum CMD{
        GET_CONFIG,
        ADD_CAMERA,
        DEL_CAMERA
    };
    enum RET{
        RET_SUCCESS,
        RET_FAIL,
        RET_REFRESH
     };
    enum PORTS{
        SERVER_PORT=12345,
        SERVER_DATA_OUTPUT_PORT=12346,
        CLIENT_REPORTER_PORT=12347,
        SERVER_REPORTER_PORT=12348
    };

    static void pkg_set_len(char *c,int len)
    {
        char *dst=c;
        quint16 *p_len=( quint16 *)dst;
        *p_len=len;
    }
    static int pkg_get_len(char *c)
    {
        char *dst=c;
        quint16 *p_len=( quint16 *)dst;
        return *p_len;
    }
    static int pkg_get_ret(char *c)
    {
        char *dst=c;
        quint16 *p_ret=( quint16 *)dst;
        return *p_ret;
    }
    static void pkg_set_version(char *c,int version)
    {
        char *dst=c;
        quint16 *p_version=( quint16 *)dst;
        *p_version=version;
    }
    static void pkg_set_op(char *c,int op)
    {
        char *dst=c;
        quint16 *p_op=( quint16 *)dst;
        *p_op=op;
    }
    static int pkg_get_op(char *c)
    {
        char *dst=c;
        quint16 *p_op=( quint16 *)dst;
        return *p_op;
    }
    static void pkg_set_ret(char *c,int ret)
    {
        char *dst=c;
        quint16 *p_ret=( quint16 *)dst;
        *p_ret=ret;
    }
//    static int pkg_get_len(QByteArray &ba)
//    {
//              return 0;
//    }
//    static void pkg_set_version(QByteArray &ba)
//    {

//    }
//    static int pkg_get_version(QByteArray &ba)
//    {
//              return ba;
//    }
//    static void pkg_set_op(QByteArray &ba)
//    {

//    }
//    static int pkg_get_get(QByteArray &ba)
//    {
//              return ba;
//    }
    static int encode_configuration_request(char *buf){
     //   pkg_set_len(ba);
        memset(buf,0,BUF_MAX_LEN);
        pkg_set_len(buf,0);
        pkg_set_version(buf+2,VERSION);
        pkg_set_op(buf+2+2,GET_CONFIG);
        pkg_set_ret(buf+2+2+2,RET_SUCCESS);
        return HEAD_LENGTH;
    }
    static int encode_configuration_reply(char *buf,int len,int ret){
     //   pkg_set_len(ba);
     //   memset(buf,0,BUF_MAX_LEN);
        pkg_set_len(buf,len);
        pkg_set_version(buf+2,VERSION);
        pkg_set_op(buf+2+2,GET_CONFIG);
        pkg_set_ret(buf+2+2+2,ret);
        return HEAD_LENGTH;
    }
    static int encode_addcam_request(char *buf,int len){
     //   pkg_set_len(ba);
        memset(buf,0,BUF_MAX_LEN);
        pkg_set_len(buf,len);
        pkg_set_version(buf+2,VERSION);
        pkg_set_op(buf+2+2,ADD_CAMERA);
        pkg_set_ret(buf+2+2+2,RET_SUCCESS);
        return HEAD_LENGTH+len;
    }
    static int encode_delcam_request(char *buf,int index){
     //   pkg_set_len(ba);
        memset(buf,0,BUF_MAX_LEN);
        pkg_set_len(buf,0);
        pkg_set_version(buf+2,VERSION);
        pkg_set_op(buf+2+2,DEL_CAMERA);
        pkg_set_ret(buf+2+2+2,index);
        return HEAD_LENGTH;
    }
    static int get_operation(char *buf){
        return pkg_get_op(buf+2+2);
    }
    static int get_length(char *buf){
        return pkg_get_len(buf);
    }
    static int get_index(char *buf){
        return pkg_get_ret(buf+2+2+2);
    }
//    static char* encode_configuration_reply(char *buf){
//     //   pkg_set_len(ba);
//        pkg_set_len(buf,0);
//        pkg_set_version(buf+2,VERSION);
//        pkg_set_op(buf+2+2,GET_CONFIG);
//        pkg_set_ret(buf+2+2+2,0);
//        return c;
//    }
//    static char* decode_request(char *buf){
//        return buf+8;
//    }
};

#endif // PROTOCOL_H

