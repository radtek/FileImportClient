/*
 * Task.cpp
 *
 *  Created on: 2018年7月21日
 *      Author: luzxiang
 */

#include "Task.h"

/*******************************************************
 * Function     : Task::Task
 * Description  : TODO
 * Input        :
 * Return       :
 * Author       : luzxiang
 * Notes        : --
 ******************************************************/
Task::Task() {
	// TODO Auto-generated constructor stub
}

/*******************************************************
 * Function     : Task::~Task
 * Description  : TODO
 * Input        :
 * Return       :
 * Author       : luzxiang
 * Notes        : --
 ******************************************************/
Task::~Task()
{

}

/*******************************************************
 * Function     : Task::Task
 * Description  : init task argument
 * Input        :
 * Return       :
 * Author       : luzxiang
 * Notes        : --
 ******************************************************/
Task::Task(const TaskInfo_St& tskinfo,string &srcpath,string &temppath,string &sendpath)
{
	memcpy(&this->Info,&tskinfo,sizeof(TaskInfo_St));
	this->Src.WorkPath = srcpath;
	this->Temp.WorkPath = temppath;
	this->Send.WorkPath = sendpath;

//	std::cout<<this->Info.SrcPath<<endl;
//	std::cout<<this->Info.TaskName<<endl;
//	std::cout<<this->Info.SyncPeriod<<endl;
//	std::cout<<this->Info.MaxSizeOfScan<<endl;
//	std::cout<<this->Info.LastSyncTime<<endl;
//
//	std::cout<<this->Src.WorkPath<<endl;
//	std::cout<<this->Temp.WorkPath<<endl;
//	std::cout<<this->Send.WorkPath<<endl;
}

/*******************************************************************************
 * Function     : UpdateTaskInfo
 * Description  : TODO
 * Input        :
 * Return       :
 * Author       : luzx
 * Notes        : -1 error
 * 				: >0 success
 *******************************************************************************/
int Task::UpdateTaskInfo(void)
{

	return 0;
}
/*******************************************************
 * Function     : Task::ScanSrcPath
 * Description  : TODO
 * Input        :
 * Return       :
 * Author       : luzxiang
 * Notes        : --
 ******************************************************/
int Task::ScanSrcPath()
{
	std::cout<<__FUNCTION__<<endl;
	std::cout<<this->Src.WorkPath<<endl;
	return 0;
}

/*******************************************************
 * Function     : Task::ScanTempPath
 * Description  : TODO
 * Input        :
 * Return       :
 * Author       : luzxiang
 * Notes        : --
 ******************************************************/
int Task::ScanTempPath()
{
	std::cout<<__FUNCTION__<<endl;
	std::cout<<this->Temp.WorkPath<<endl;
	return 0;
}

/*******************************************************
 * Function     : Task::MoveFileToTempPath
 * Description  : TODO
 * Input        :
 * Return       :
 * Author       : luzxiang
 * Notes        : --
 ******************************************************/
int Task::MoveFileToTempPath()
{
	std::cout<<__FUNCTION__<<endl;
	return 0;
}

/*******************************************************
 * Function     : Task::SendFileToSocket
 * Description  : TODO
 * Input        :
 * Return       :
 * Author       : luzxiang
 * Notes        : --
 ******************************************************/
int Task::SendFileToSocket(void)
{
	std::cout<<__FUNCTION__<<endl;
	std::cout<<this->Send.WorkPath<<endl;
	return 0;
}

/*******************************************************
 * Function     : Task::DelFilesOfSended
 * Description  : TODO
 * Input        :
 * Return       :
 * Author       : luzxiang
 * Notes        : --
 ******************************************************/
int Task::DelFilesOfSended(void)
{
	std::cout<<__FUNCTION__<<endl;
	std::cout<<this->Send.WorkPath<<endl;
	return 0;
}

/*******************************************************
 * Function     : Task::LoadFileToSendBuf
 * Description  : TODO
 * Input        :
 * Return       :
 * Author       : luzxiang
 * Notes        : --
 ******************************************************/
int Task::LoadFileToSendBuf(void)
{
	std::cout<<__FUNCTION__<<endl;
	return 0;
}














