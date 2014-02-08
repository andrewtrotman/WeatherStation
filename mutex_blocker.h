/*
	MUTEX_BLOCKER.H
	---------------
	Copyright (c) 2014 Andrew Trotman
	Licensed BSD
*/
#ifndef MUTEX_BLOCKER_H_
#define MUTEX_BLOCKER_H_

#include "mutex.h"

/*
	class MUTEX_BLOCKER
	-------------------
*/
class mutex_blocker
{
private:
	mutex *mine;

public:
	mutex_blocker(mutex *device) {mine = device; mine->enter();}
	~mutex_blocker() {mine->leave();}
};

#endif /* MUTEX_BLOCKER_H_ */
