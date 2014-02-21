#include <cassert>

#include "TaskQueueWin32.h"

// Must be included last!
#include "debug.h"

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

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	SetThreadpoolThreadMinimum(m_pool, 1);
	SetThreadpoolThreadMaximum(m_pool, sysinfo.dwNumberOfProcessors);

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

MutexWin32::MutexWin32()
{
	m_mutex = CreateMutex(NULL, false, NULL);
}

MutexWin32::~MutexWin32()
{
	CloseHandle(m_mutex);
}

void MutexWin32::Lock()
{
	WaitForSingleObject(m_mutex, INFINITE);
}

void MutexWin32::Unlock()
{
	ReleaseMutex(m_mutex);
}
