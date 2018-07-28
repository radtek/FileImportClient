/*
 * ITask.h
 *
 *  Created on: 2018年7月21日
 *      Author: luzxiang
 */

#ifndef ITASK_H_
#define ITASK_H_

class ITask {
public:

	ITask();
	virtual ~ITask();

	virtual int ScanSrcPath(void) = 0;

	virtual int MoveFileToTempPath(void) = 0;

	virtual int ScanTempPath(void) = 0;

	virtual int LoadFileToSendBuf(void) = 0;

	virtual int SendFileToSocket(void) = 0;

	virtual int DelFilesOfSended(void) = 0;

};

#endif /* ITASK_H_ */
