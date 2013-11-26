#ifndef SOUND_UTIL_H
#define SOUND_UTIL_H

#include <cstdint>

// FIXME: Replace with shared_ptr
template<class T>
class RefCounted
{
public:
	RefCounted(T value)
	{
		m_value = value;
		m_refCount = 1;
	}
	~RefCounted()
	{
	}
	T GetValue()
	{
		return m_value;
	}
	void IncRefCount()
	{
		m_refCount++;
	}
	int DecRefCount()
	{
		assert(m_refCount > 0);

		return --m_refCount;
	}
private:
	T m_value;
	unsigned int m_refCount;
};

// FIXME: Remove?
class Mutex
{
public:
	Mutex()
	{
		m_mutex = CreateMutex(NULL, false, NULL);
	}
	virtual ~Mutex()
	{
		CloseHandle(m_mutex);
	}
	void Lock()
	{
		WaitForSingleObject(m_mutex, INFINITE);
	}
	void Unlock()
	{
		ReleaseMutex(m_mutex);
	}

private:
	HANDLE m_mutex;
};

#endif
