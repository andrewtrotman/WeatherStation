/*
	FUNDAMENTAL_TYPES.H
	-------------------
	Copyright (c) 2012-2013 Andrew Trotman
	Licensed BSD
*/
#ifndef FUNDAMENTAL_TYPES_H_
#define FUNDAMENTAL_TYPES_H_

/*
	Standard types
	Its necessary to declare these here because they are not provided by Visual C/C++ 9  (Visual Studio 2008)
*/
#ifdef _MSC_VER
	typedef signed char int8_t;
	typedef unsigned char uint8_t;
	typedef unsigned short uint16_t;
	typedef unsigned long uint32_t;
	typedef signed long int32_t;
	typedef unsigned long long uint64_t;
	typedef signed long long int64_t;
#else
	#include <stdint.h>
#endif



#endif /* FUNDAMENTAL_TYPES_H_ */
