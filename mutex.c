/*
	MUTEX.C
	-------
	Copyright (c) 2014 Andrew Trotman
	Licensed BSD
*/

#include "mutex.h"
/*
	MUTEX::MUTEX()
	--------------
*/
mutex::mutex(char *name)
{

#ifdef _MSC_VER
	SECURITY_ATTRIBUTES attributes = {sizeof(attributes), NULL, false};
	mutex_id = CreateMutex(&attributes, false, name);
#else
	sem_open(name, O_CREAT, 777);
#endif
}

/*
	MUTEX::~MUTEX()
	---------------
*/
mutex::~mutex()
{
#ifdef _MSC_VER
	CloseHandle(mutex_id);
#else
	sem_close(mutex_id);
#endif
}


/*
	MUTEX::ENTER()
	--------------
*/
void mutex::enter(void)
{
#ifdef _MSC_VER
	WaitForSingleObject(mutex_id, INFINITE);
#else
	sem_wait(mutex_id);
#endif
}

/*
	MUTEX::LEAVE()
	--------------
*/
void mutex::leave(void)
{
#ifdef _MSC_VER
	ReleaseMutex(mutex_id);
#else
	sem_post(mutex_id);
#endif
}
