//============================================================================
// Name        : client.cpp
// Author      : luzxiang
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
using namespace std;

#include "Task.h"
#include "SocketLayer/SocketLayer.h"
#include "log/log.h"

int main(int argv, char*argvs[])
{
	std::vector <Task> g_Task;
	char temp[5]={0};
	char ip[] = "127.0.0.1";//"66.154.108.47";
	SocketLayer admin(ip,4141);
	admin.CreateConnect();

	for(int i = 0; i < 10; i++)
	{
		sprintf(temp,"%d",i);
		string srcpath("/home/src/");
		srcpath.append(temp);
		string temppath("/home/temp/");
		temppath.append(temp);
		string sendpath("/home/send/");
		sendpath.append(temp);

		TaskInfo_St taskinfo;
		taskinfo.Rule.nTaskID = 5;
		taskinfo.Rule.status = 1;
		taskinfo.Rule.nLimitSpeed = 3;
		taskinfo.Rule.nTransmod = 1;
		taskinfo.Rule.nTaskID = 1000 + i;
		taskinfo.LastSyncTime = time(NULL);

		memcpy(taskinfo.Conf.strSrcPath,srcpath.c_str(),sizeof(taskinfo.Conf.strSrcPath));
		snprintf(taskinfo.Conf.strSrcIP,sizeof(taskinfo.Conf.strSrcIP),"%s","10.10.10.1");
		snprintf(taskinfo.Rule.strTaskName,sizeof(taskinfo.Rule.strTaskName),"%s%d","taskname",i);

		Task task(taskinfo,srcpath,temppath,sendpath);
		g_Task.push_back(task);
	}
	while(1)
	{
		cout<<"test"<<endl;
		sleep(10);
	}
//	int delay = 50*1000;
//	for(auto tsk : g_Task)
//	{
//		tsk.ScanSrcPath();
//		usleep(delay);
//		tsk.MoveFileToTempPath();
//		usleep(delay);
//		tsk.ScanTempPath();
//		usleep(delay);
//		tsk.LoadFileToSendBuf();
//		usleep(delay);
//		tsk.SendFileToSocket();
//		usleep(delay);
//		tsk.DelFilesOfSended();
//		usleep(delay);
//		cout << "===============" << endl;
//	}
	return 0;
}










