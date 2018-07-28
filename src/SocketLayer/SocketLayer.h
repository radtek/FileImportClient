#ifndef BD_SOCKETLAYER_H
#define BD_SOCKETLAYER_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
//#ifdef OS_LINUX
#include <resolv.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#endif
#include <iostream>
#include  <list>
#include  <deque>
using namespace std;

#include "../com/swgap_comm.h"
#include "../log/log.h"

#define PACK_HEADER 		GAPPACK_HEADER
#define S_LOG_ERROR			LOG_ERROR
#define S_LOG_WARN 			LOG_WARN
#define S_LOG_INFO 			LOG_INFO
#define SOCKET_RECBUF_LEN	(192*1024)	//接收缓冲区长度
#define SOCKET_IP_LEN		(32)		//接收缓冲区长度
class SocketLayer{public://与安全导入服务器通信类的构造函数
    SocketLayer();
    SocketLayer(char*,int);

    //与安全导入服务器通信类的析构函数
    virtual ~SocketLayer();
    //创建TCP连接资源
    void CreateConnect();


    std::deque<string> CachePool;
    int nServerState;
    int ConnectTime;                    //<连接时间，用于RVU运行率数据库记录
    bool m_bConnected;                  //<连接状态
    int SocketState;					//Socket状态，0为空，1为需要连接，2为正在连接，3为已连接
    char svrIP[SOCKET_IP_LEN];
    unsigned int svrPort;
    bool bThreadStart;
    pthread_t thRevSktPid;              //接收数据线程ID
    //接收及连接SOCKET
    void CheckData();
    //接收数据处理
    void HandlePacket();
    //获取包头信息（自动填充）
    bool GetHeadPackValue(unsigned char nCmdType, unsigned char nCmdCode, unsigned int nDataLength, PACK_HEADER &PackHead);

private:
    int nSocketId;                      //关联的socket句柄
    struct sockaddr_in svrAddr;      //IPV4socket 地址信息
    struct sockaddr_in6 svrAddr6;    //IPV6socket 地址信息

    char *recbuf;						//<Socket接收数据缓冲区
    unsigned int recbuflen;						//<当前数据缓冲区内的数据长度
    int tLastUpdate;					//<最后一次更新时间
    int nEncrypt ;
    int tLastRevData;                   //<最后一次接收到数据的时间
    bool bNeedConnect;                  //是否需要连接
//    SSL *m_pSsl;                        //SSL 对象指针
//    SSL_METHOD *m_pMeth;                //SSL方法指针
//    SSL_CTX *m_pCtx;                    //SSL ctx指针
    //释放资源(socket、线程、内存等)
    void Release();

    //发送数据封装函数
    int Send(int fd, const char *cmd, const size_t len);

    //连接安全导入服务器
    void ConnectServer();

    //关闭套接字资源
    int CloseSocket();

    //模块接收到socket数据处理
    void OnReceive(char *pDataBuf, int nLen);

    //协议解析(对收到的数据进行协议解析)
    void AnalyseReceiveData(void);

    int InsertDataIntoCachePool(char *pDataBuf, int nLen);

    //发送测试（心跳）包
    int SendTest();
};

#endif // SOCKETLAYER_H