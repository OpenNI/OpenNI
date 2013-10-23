/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#ifndef _XN_MACROS_H_
#define _XN_MACROS_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStatusCodes.h"

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------

/** Returns the (x)th power of 2. */
#define XN_SETBITMASK(x) (1 << x-1)

/** Returns Y if X is NULL. */ 
#define XN_RET_IF_NULL(x, y)	\
		if (x == NULL)			\
		{						\
			return (y);			\
		}

/** Returns Y if X isn't XN_STATUS_OK. */ 
#define XN_IS_STATUS_OK_RET(x, y)	\
		if (x != XN_STATUS_OK)		\
		{							\
			return (y);				\
		}

/** Jumps to the ErrorCleanUp label if X isn't XN_STATUS_OK and return Y. */ 
#define XN_IS_STATUS_OK_GOCLEANUP_RET(x, y)	\
		if (x != XN_STATUS_OK)				\
		{									\
			x = y;							\
			goto ErrorCleanUp;				\
		}

/** Returns X if X isn't XN_STATUS_OK. */ 
#define XN_IS_STATUS_OK(x)			\
		XN_IS_STATUS_OK_RET(x, x)

#define XN_IS_STATUS_OK_ASSERT(x)	\
	if (x != XN_STATUS_OK)		\
	{							\
		XN_ASSERT(FALSE);		\
		return (x);				\
	}
		

/** Jumps to the ErrorCleanUp label if X isn't XN_STATUS_OK. */ 
#define XN_IS_STATUS_OK_GOCLEANUP(x)	\
		if (x != XN_STATUS_OK)			\
		{								\
			goto ErrorCleanUp;			\
		}

/** Returns Y if X isn't XN_STATUS_OK. */ 
#define XN_IS_BOOL_OK_RET(x, y)		\
		if (x != TRUE)				\
		{							\
			return (y);				\
		}

/** Jumps to the ErrorCleanUp label if X isn't XN_STATUS_OK and returns Y. */ 
#define XN_IS_BOOL_OK_GOCLEANUP_RET(x, y)	\
		if (x != TRUE)						\
		{									\
			goto ErrorCleanUp;				\
		}

/** Jumps to the ErrorCleanUp label if X isn't XN_STATUS_OK and returns Y. */ 
#define XN_GOCLEANUP_RET(x, y)	\
		x = y;					\
		goto ErrorCleanUp;				

/** Returns Y if X is null. */
#define XN_VALIDATE_PTR(x,y)	\
		if (x == NULL)			\
		{						\
			return (y);			\
		}

/** Returns an output overflow error if x is beyond y */
#define XN_CHECK_INPUT_OVERFLOW(x, y)					\
		if (x > y)										\
		{												\
			return (XN_STATUS_INPUT_BUFFER_OVERFLOW);	\
		}

/** Returns an input overflow error if x is beyond y */
#define XN_CHECK_OUTPUT_OVERFLOW(x, y)					\
		if (x > y)										\
		{												\
			return (XN_STATUS_OUTPUT_BUFFER_OVERFLOW);	\
		}

/** Disables Copy ctor and assignment operator. Should be placed under "private:" section. */
#define XN_DISABLE_COPY_AND_ASSIGN(TypeName)			\
	TypeName(const TypeName&);							\
	void operator=(const TypeName&);

/** Disables the "local variable is initialized but not referenced" warning (if you need
    to use this variable in an assert */
#define XN_REFERENCE_VARIABLE(x)						\
	((void)x)

#endif //_XN_MACROS_H_
