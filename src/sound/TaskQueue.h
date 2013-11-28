#ifndef SOUND_TASKQUEUE_H
#define SOUND_TASKQUEUE_H

#include <functional>

class TaskQueue
{
public:
	virtual ~TaskQueue() {};

	virtual void EnqueueTask(const std::function<void()> &task) = 0;

	virtual void WaitForTasks(bool cancelPendingTasks) = 0;
};

#endif
