#ifndef SOUND_TASKQUEUEWIN32_H
#define SOUND_TASKQUEUEWIN32_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include "TaskQueue.h"

class TaskQueueWin32 : public TaskQueue
{
public:
	TaskQueueWin32();
	virtual ~TaskQueueWin32();

	void EnqueueTask(const std::function<void()> &task);

	void WaitForTasks(bool cancelPendingTasks);

private:
	TP_POOL *m_pool;
	TP_CLEANUP_GROUP *m_cleanupGroup;
	TP_CALLBACK_ENVIRON m_environment;
};

class MutexWin32 : public Mutex
{
public:
	MutexWin32();
	virtual ~MutexWin32();

	void Lock();
	void Unlock();

private:
	HANDLE m_mutex;
};

#endif
