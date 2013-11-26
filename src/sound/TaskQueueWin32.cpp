#include "TaskQueueWin32.h"

static VOID CALLBACK TaskCallback(PTP_CALLBACK_INSTANCE instance, PVOID parameter, PTP_WORK work)
{
	const std::function<void()> *task = (const std::function<void()> *)parameter;

	(*task)();

	delete task;
}

TaskQueueWin32::TaskQueueWin32()
{
	m_pool = CreateThreadpool(NULL);
	SetThreadpoolThreadMinimum(m_pool, 1);
	// FIXME: Don't hardcode this.
	SetThreadpoolThreadMaximum(m_pool, 4);

	InitializeThreadpoolEnvironment(&m_environment);
	SetThreadpoolCallbackPool(&m_environment, m_pool);
}

TaskQueueWin32::~TaskQueueWin32()
{
	DestroyThreadpoolEnvironment(&m_environment);
	CloseThreadpool(m_pool);
}

void TaskQueueWin32::EnqueueTask(const std::function<void()> &task)
{
	// Copy the task so it won't get freed while we use it.
	const std::function<void()> *data = new std::function<void()>(task);

	PTP_WORK work = CreateThreadpoolWork(TaskCallback, (PVOID)data, &m_environment);
	SubmitThreadpoolWork(work);
	CloseThreadpoolWork(work);
}

void TaskQueueWin32::WaitForTasks()
{
}
