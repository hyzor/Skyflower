#include "TaskQueue.h"
#include "TaskQueueWin32.h"

// Must be included last!
#include "debug.h"

TaskQueue *TaskQueue::Create()
{
	TaskQueue *taskQueue = NULL;

#ifdef _WIN32
	taskQueue = new TaskQueueWin32();
#else
	#error Unsupported platform!
#endif

	return taskQueue;
}

void TaskQueue::Destroy(TaskQueue *taskQueue)
{
	delete taskQueue;
}

Mutex *Mutex::Create()
{
	Mutex *mutex = NULL;

#ifdef _WIN32
	mutex = new MutexWin32();
#else
	#error Unsupported platform!
#endif

	return mutex;
}

void Mutex::Destroy(Mutex *mutex)
{
	delete mutex;
}
