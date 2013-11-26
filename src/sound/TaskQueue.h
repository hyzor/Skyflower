#ifndef SOUND_TASKQUEUE_H
#define SOUND_TASKQUEUE_H

#include <functional>

class TaskQueue
{
public:
	virtual ~TaskQueue() {};

	virtual void EnqueueTask(const std::function<void()> &task) = 0;

	virtual void WaitForTasks() = 0;
};

#endif
