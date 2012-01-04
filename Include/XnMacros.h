/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
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
	void operator=(const TypeName&)

/** Disables the "local variable is initialized but not referenced" warning (if you need
    to use this variable in an assert */
#define XN_REFERENCE_VARIABLE(x)						\
	((void)x)

#endif //_XN_MACROS_H_
