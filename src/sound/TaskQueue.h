#ifndef SOUND_TASKQUEUE_H
#define SOUND_TASKQUEUE_H

#include <functional>

class TaskQueue
{
public:
	static TaskQueue *Create();
	static void Destroy(TaskQueue *taskQueue);

public:
	virtual ~TaskQueue() {};

	virtual void EnqueueTask(const std::function<void()> &task) = 0;
	virtual void WaitForTasks(bool cancelPendingTasks) = 0;
};

class Mutex
{
public:
	static Mutex *Create();
	static void Destroy(Mutex *mutex);

public:
	virtual ~Mutex() {}
	virtual void Lock() = 0;
	virtual void Unlock() = 0;
};

#endif
