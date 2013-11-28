#include <cassert>

#include "TaskQueueWin32.h"

static void CALLBACK TaskCallback(PTP_CALLBACK_INSTANCE instance, void *context, PTP_WORK work)
{
	const std::function<void()> *task = (const std::function<void()> *)context;

	(*task)();

	delete task;
}

TaskQueueWin32::TaskQueueWin32()
{
	m_pool = CreateThreadpool(NULL);
	m_cleanupGroup = CreateThreadpoolCleanupGroup();

	assert(m_pool);
	assert(m_cleanupGroup);

	SetThreadpoolThreadMinimum(m_pool, 1);
	// FIXME: Don't hardcode this.
	SetThreadpoolThreadMaximum(m_pool, 4);

	InitializeThreadpoolEnvironment(&m_environment);
	SetThreadpoolCallbackPool(&m_environment, m_pool);
	SetThreadpoolCallbackCleanupGroup(&m_environment, m_cleanupGroup, NULL);
}

TaskQueueWin32::~TaskQueueWin32()
{
	DestroyThreadpoolEnvironment(&m_environment);
	CloseThreadpool(m_pool);
	CloseThreadpoolCleanupGroup(m_cleanupGroup);
}

void TaskQueueWin32::EnqueueTask(const std::function<void()> &task)
{
	// Copy the task so it won't get freed while we use it.
	const std::function<void()> *data = new std::function<void()>(task);
	TP_WORK *work = CreateThreadpoolWork(TaskCallback, (void *)data, &m_environment);

	assert(work);

	SubmitThreadpoolWork(work);
	CloseThreadpoolWork(work);
}

void TaskQueueWin32::WaitForTasks(bool cancelPendingTasks)
{
	CloseThreadpoolCleanupGroupMembers(m_cleanupGroup, cancelPendingTasks, NULL);
}
