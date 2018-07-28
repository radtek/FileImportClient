/*
 * Task.h
 *
 *  Created on: 2018年7月21日
 *      Author: luzxiang
 */

#ifndef TASK_H_
#define TASK_H_

#include "ITask.h"
#include "com/swgap_comm.h"
#include <deque>
#include <iostream>
#include <string.h>
using namespace std;

struct FILEINFO_St
{
	int SecretLevel;             	//涉密级别
	char FilePath[1024];     		//文件相对工作目录的路径
	unsigned long nFileSize;		//文件大小
	unsigned int nsessionId;		//文件ID号
	FILEINFO_St()
	{
		SecretLevel = 0;
		nFileSize = 0;
		nsessionId = 0;
		memset(FilePath,0,sizeof(FilePath));
	}
} ;
struct FileList_St
{
	std::deque<FILEINFO_St> FileList;   //扫描到准备发送的文件队列
	unsigned long long TotalFileSize;	//当前已扫描到可以发送的文件大小总量
	unsigned int Flag;       			//发送标志 0不需要发送 1等待发送
	string WorkPath;				//工作目录
	FileList_St()
	{
		Flag = 0;
		TotalFileSize = 0;
	}
} ;

struct TaskInfo_St
{
	FILESRC_CONFIGURE Conf;
	FILESYNC_RULE Rule;
	time_t LastSyncTime;

	TaskInfo_St()
	{
		memset(&Conf,0,sizeof(Conf));
		memset(&Rule,0,sizeof(Rule));
		LastSyncTime = 0;
	}
};

class Task: public ITask {
public:
	TaskInfo_St Info;
	FileList_St Src;
	FileList_St Temp;
	FileList_St Send;

	virtual ~Task();
	Task(void);
	Task(const TaskInfo_St& tskinfo,string &srcpath,string &temppath,string &sendpath);

	int UpdateTaskInfo(void);

	int ScanSrcPath(void);

	int MoveFileToTempPath(void);

	int ScanTempPath(void);

	int LoadFileToSendBuf(void) ;

	int SendFileToSocket(void) ;

	int DelFilesOfSended(void);

	int RecvTaskReqFromSocket(void);

	int RecvFileFromSocket(void);
};

#endif /* TASK_H_ */











