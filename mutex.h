/*
	MUTEX.H
	-------
	Copyright (c) 2014 Andrew Trotman
	Licensed BSD
*/
#ifndef MUTEX_H_
#define MUTEX_H_

#ifdef _MSC_VER
	#include <windows.h>
#else
	#include <fcntl.h>
#endif

/*
	class MUTEX
	-----------
*/
class mutex
{
public:
	#ifdef _MSC_VER
		HANDLE mutex_id;
	#else
		sem_t mutex_id;
	#endif
public:
	mutex(char *name);
	virtual ~mutex();

	void enter(void);
	void leave(void);
};

#endif /* MUTEX_H_ */
