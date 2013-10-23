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
