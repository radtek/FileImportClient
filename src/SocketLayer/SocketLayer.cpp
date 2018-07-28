#include <sys/stat.h>
#include <sys/types.h>

#include "../SocketLayer/SocketLayer.h"

/*******************************************************
 * Function		: RevSktDataProc
 * Description	: 接收来自安全导入服务器TCP socket数据线程
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
void *RevSktDataProc(void *p)
{
    SocketLayer *pSocketLayer = (SocketLayer*) p;
    if (NULL == pSocketLayer)
        return NULL;
    pSocketLayer->bThreadStart = true;
    while (pSocketLayer->bThreadStart)
    {
        pSocketLayer->CheckData();
        if (!pSocketLayer->m_bConnected)
        {
            usleep(300 * 1000);
        }
    }
    return NULL;
}

/*******************************************************
 * Function		: SocketLayer
 * Description	: 与安全导入服务器通信类的构造函数
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
SocketLayer::SocketLayer(char* ip,int port)
{
    sprintf(svrIP,ip,sizeof(svrIP));
    svrPort = port;                         //端口

	thRevSktPid = 0;
	nEncrypt = 0;
	nServerState = 0;
	bNeedConnect = 0;
    recbuflen = 0;
    recbuf = (char*) malloc(SOCKET_RECBUF_LEN);
    nSocketId = -1;                    //关联的socket句柄
    SocketState = 0;				   //Socket状态，0为空，1为需要连接，2为正在连接，3为已连接
    recbuflen = 0;					   //<当前数据缓冲区内的数据长度
    ConnectTime = 0;                   //<连接时间，用于RVU运行率数据库记录
    tLastUpdate = 0;				   //<最后一次更新时间
    tLastRevData = time(NULL);         //<最后一次接收到数据的时间
    thRevSktPid = 0;
    bThreadStart = false;
    m_bConnected = false;
//    m_pSsl = NULL;
//    m_pMeth = NULL;
//    m_pCtx = NULL;
}

/*******************************************************
 * Function		: ~SocketLayer
 * Description	: 与安全导入服务器通信类的析构函数
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
SocketLayer::~SocketLayer()
{
    Release();
}

/*******************************************************
 * Function		: Send
 * Description	: 发送数据封装函数
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
int SocketLayer::Send(int fd, const char *cmd, const size_t len)
{
    if (!m_bConnected || cmd == NULL)
        return -1;
    fd_set writefd;
    size_t offset = 0;

    int nSendLen = 0;
    while (offset < len)
    {
        FD_ZERO(&writefd);
        FD_SET(fd, &writefd);
        int nRet = select(fd + 1, 0, &writefd, 0, NULL);//&timeout);
        if(nRet == -1)
        {
        	S_LOG_ERROR("select error: %s", strerror(errno));
        	return -1;
        }

        if (nEncrypt == 0 || nEncrypt == 2) //数据不加密
        {
            if (fd > 0)
        		nSendLen = send(fd, cmd + offset, len - offset, 0);
        }
        if(nSendLen == -1)
        {
        	S_LOG_ERROR("send error: %s", strerror(errno));
        	return -1;
        }
        offset += nSendLen;
    }
    return len;
}

/*******************************************************
 * Function		: Release
 * Description	: 释放资源(socket、线程、内存等)
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
void SocketLayer::Release()
{
    if (bThreadStart && thRevSktPid != 0)
    {
        bThreadStart = false;
        pthread_join(thRevSktPid, NULL);	//结束线程
        thRevSktPid = 0;
    }

    close(nSocketId);

    nSocketId = -1;
    if (recbuf)
    {
        free(recbuf);
        recbuf = NULL;
    }
}

/*******************************************************
 * Function		: CreateConnect
 * Description	: 创建TCP连接资源
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
void SocketLayer::CreateConnect()
{
    nSocketId = -1;
    if (inet_pton(AF_INET, this->svrIP, &svrAddr.sin_addr) > 0)
        nSocketId = socket(AF_INET, SOCK_STREAM, 0);
//    if (inet_pton(AF_INET6, this->svrIP, &svrAddr6.sin6_addr) > 0)                 // IPv6
//        nSocketId = socket(AF_INET6, SOCK_STREAM, 0);

    if (nSocketId <= 0)
    {
        S_LOG_ERROR("create socket errno：%d, errerInfo：%s", errno, strerror(errno));
        return;
    }
    else
    {
        S_LOG_INFO("socket created sucessfully");
    }
    int b_on = 0;
    ioctl(nSocketId, FIONBIO, &b_on);
    sched_param param;
    pthread_attr_t attr;
    int newprio = 90;
    pthread_attr_init(&attr);
    pthread_attr_getschedparam(&attr, &param);
    param.sched_priority = newprio;
    pthread_attr_setschedparam(&attr, &param);
    bThreadStart = true;
    pthread_create(&thRevSktPid, &attr, RevSktDataProc, (void*) this);
}

/*******************************************************
 * Function		: CheckData
 * Description	: 接收及连接SOCKET
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
void SocketLayer::CheckData()
{
    if (m_bConnected)
    {
        HandlePacket();
    }
    else
    {
        nEncrypt = 0;
        S_LOG_WARN("SocketLayer unConnected");
        ConnectServer();
    }
}

/*******************************************************
 * Function		: ConnectServer
 * Description	: 连接安全导入服务器
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        : -1 error
 * 				: >0 success
 ********************************************************/
void SocketLayer::ConnectServer()
{
    int nRet = -1;
    if (inet_pton(AF_INET, this->svrIP, &svrAddr.sin_addr) > 0)
        nRet = connect(nSocketId, (struct sockaddr*) &svrAddr, sizeof(svrAddr));

    if (nRet != 0)
    {
        m_bConnected = false;
        return;
    }
    else
    {
        m_bConnected = true;
    }

}

/*******************************************************
 * Function		: CloseSocket
 * Description	: 关闭套接字资源
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        : -1 error
 * 				: >0 success
 ********************************************************/
int SocketLayer::CloseSocket()
{
    m_bConnected = false;
    close(nSocketId);
    usleep(5 * 1000);
    nSocketId = -1;

    return 0;
}

/*******************************************************
 * Function		: HandlePacket
 * Description	: 接收数据处理
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
void SocketLayer::HandlePacket()
{
    char* buffer = (char*) malloc(1024 * 64);
    int nLen = 0;
    timeval timeout;
    timeout.tv_sec = 2;
    timeout.tv_usec = 0;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(nSocketId, &fdset);
    if (bThreadStart == false)
        return;
    if (select(nSocketId + 1, &fdset, 0, 0, &timeout) > 0)
    {
        if (nEncrypt == 0)
            nLen = recv(nSocketId, buffer, sizeof(buffer), 0);
//        else
//            nLen = SSL_read(m_pSsl, buffer, sizeof(buffer));

        if (nLen > 0)
        {
            tLastRevData = time(NULL);
            OnReceive(buffer, nLen);
        }
        else //SOCKE 异常（已退出）
        {
            m_bConnected = false;
            sleep(1 * 1000);
        }
    }
    if ((time(NULL) - tLastRevData) >= 30)
    {
        SendTest();
    }
}

/*******************************************************
 * Function		: OnReceive
 * Description	: 模块接收到socket数据处理
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
void SocketLayer::OnReceive(char *buf, int len)
{
    if (!m_bConnected) return;								//未连接不处理

    tLastRevData = time(NULL);   //收到数据

    if ((recbuflen + len) >= SOCKET_RECBUF_LEN)   //缓冲区内数据太多，直接将这部分数据丢掉
    {
    	recbuflen = 0;
    }
    memcpy(recbuf + recbuflen, buf, len);
    recbuflen += len;

    if((unsigned int)recbuflen >= sizeof(PACK_HEADER))
    {
    	AnalyseReceiveData();
    }
}

int SocketLayer::InsertDataIntoCachePool(char *pDataBuf, int nLen)
{
	string buf(pDataBuf,nLen);
	this->CachePool.push_back(buf);
	return 0;
}

/*******************************************************
 * Function		: AnalyseReceiveData
 * Description	: 协议解析(对收到的数据进行协议解析)
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
void SocketLayer::AnalyseReceiveData()
{
    PACK_HEADER *cmd_Header = NULL;
    unsigned int Offset = 0;				//记录tempbuf的偏移量

    while (1)
    {
        if (Offset >= (unsigned int) recbuflen)                             //数据已拷贝完成
        {
        	recbuflen = 0;
        	return;
        }

        if((recbuflen - Offset) < sizeof(PACK_HEADER))
        	break;

        cmd_Header = (PACK_HEADER*) (recbuf + Offset);
        if ((strncmp(cmd_Header->HeadFlag, "####", 4) != 0 &&
        		strncmp(cmd_Header->HeadFlag, "@@@@", 4) != 0 )||
        		cmd_Header->Version != CMD_VERS_ONE)
        {
            Offset++;
            continue;
        }

        if (cmd_Header->TotalLength > (unsigned int) (recbuflen - Offset))	//剩余数据包不全
            break;

        InsertDataIntoCachePool(recbuf + Offset,cmd_Header->TotalLength);
        Offset += cmd_Header->TotalLength;
    }

    recbuflen -= Offset;
    memmove(recbuf, recbuf + Offset, recbuflen);
    return;
}


/*******************************************************
 * Function		: GetHeadPackValue
 * Description	: 获取包头信息（自动填充）
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
bool SocketLayer::GetHeadPackValue(unsigned char nCmdType, unsigned char nCmdCode, unsigned int nDataLength,
        PACK_HEADER &PackHead)
{
    strcpy(PackHead.HeadFlag, "####");
    PackHead.Version = CMD_VERS_ONE;
    PackHead.HeadLen = sizeof(PACK_HEADER);
    PackHead.ModuleType = MODTYPE_IMPORT_CLIENT;
    PackHead.CommandType = nCmdType;
    PackHead.CommandCode = nCmdCode;
    PackHead.TotalLength = sizeof(PACK_HEADER) + nDataLength;
    return true;
}

/*******************************************************
 * Function		: SendTest
 * Description	: 发送测试（心跳）包
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        :
 ********************************************************/
int SocketLayer::SendTest()
{
    PACK_HEADER head;
    char SendBuf[sizeof(PACK_HEADER) + 20];
    GetHeadPackValue(CMD_TYPE_SYST, CMD_SYST_TEST, 0, head);
    memcpy(SendBuf, &head, sizeof(PACK_HEADER));
    return Send(nSocketId, SendBuf, head.TotalLength);
}







