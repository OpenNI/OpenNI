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
#ifndef _XN_DATA_TYPES_H_
#define _XN_DATA_TYPES_H_

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnOS.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/**
 * The value type for all general data structures
 */
typedef void* XnValue;

/** 
 * Declares a translator from @a Type to XnValue, with a name @a ClassName. The class is declared
 * with the @a decl declspec.
 */
#define XN_DECLARE_DEFAULT_VALUE_TRANSLATOR_DECL(decl, Type, ClassName)	\
	class decl ClassName												\
	{																	\
	public:																\
		XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_CONDITION_IS_CONST_WARNING_ID)	\
																		\
		static XnValue CreateValueCopy(Type const& orig)				\
		{																\
			if (sizeof(Type) > sizeof(XnValue))							\
			{															\
				Type* pNew = XN_NEW(Type, orig);						\
				return (XnValue)pNew;									\
			}															\
			else														\
			{															\
				XnValue result = 0;										\
				xnOSMemCopy(&result, &orig, sizeof(Type));				\
				return result;											\
			}															\
		}																\
		static void FreeValue(XnValue& Value)							\
		{																\
				if (sizeof(Type) > sizeof(XnValue))						\
				{														\
					Type* p = (Type*)Value;								\
					XN_DELETE(p);										\
				}														\
		}																\
		static XnValue GetAsValue(Type const& orig)						\
		{																\
			if (sizeof(Type) > sizeof(XnValue))							\
			{															\
				return (XnValue)&orig;									\
			}															\
			else														\
			{															\
				XnValue result = 0;										\
				xnOSMemCopy(&result, &orig, sizeof(Type));				\
				return result;											\
			}															\
		}																\
		static Type const& GetFromValue(const XnValue& Value)			\
		{																\
			if (sizeof(Type) > sizeof(XnValue))							\
			{															\
				Type const* p = (Type const*)Value;						\
				return *p;												\
			}															\
			else														\
			{															\
				Type const* p = (Type const*)&Value;					\
				return *p;												\
			}															\
		}																\
		static Type& GetFromValue(XnValue& Value)						\
		{																\
			if (sizeof(Type) > sizeof(XnValue))							\
			{															\
				Type* p = (Type*)Value;									\
				return *p;												\
			}															\
			else														\
			{															\
				Type* p = (Type*)&Value;								\
				return *p;												\
			}															\
		}																\
		XN_PRAGMA_STOP_DISABLED_WARNING_SECTION							\
	};

/** 
 * Declares a translator from @a Type to XnValue, with a name @a ClassName.
 */
#define XN_DECLARE_DEFAULT_VALUE_TRANSLATOR(Type, ClassName)	\
	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR_DECL(, Type, ClassName)

#define XN_DEFAULT_TRANSLATOR_NAME(ClassName)	ClassName ## Translator

#endif // _XN_DATA_TYPES_H_
