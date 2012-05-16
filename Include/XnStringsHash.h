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
#ifndef _XN_STRINGS_HASH_H
#define _XN_STRINGS_HASH_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnHash.h"
#include <XnOS.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnStringsKeyManager
{
public:
	static XnHashValue Hash(const XnChar* const& key)
	{
		XnUInt32 nCRC = 0;
		xnOSStrCRC32(key, &nCRC);

		// convert from UINT32 to XnHashValue
		return nCRC % (1 << (sizeof(XnHashValue)*8));
	}

	static XnInt32 Compare(const XnChar* const& key1, const XnChar* const& key2)
	{
		return strcmp(key1, key2);
	}
};

class XnStringsKeyTranslator
{
public:
	static XnValue CreateValueCopy(const XnChar* const& orig)
	{
		// we should copy string, so we can keep the key
		XnUInt32 nLen = xnOSStrLen(orig) + 1; // with null termination
		XnChar* pcKey = (XnChar*)xnOSMalloc(nLen);
		xnOSStrCopy(pcKey, orig, nLen);
		return (pcKey);
	}

	static void FreeValue(XnValue& Value)
	{
		XnChar* pcKey = (XnChar*)Value;
		xnOSFree(pcKey);
	}

	static XnValue GetAsValue(const XnChar* const& orig)
	{
		return (XnValue)orig;
	}

	static const XnChar* const& GetFromValue(const XnValue& Value)
	{
		return (const XnChar* const&)Value;
	}

	static const XnChar*& GetFromValue(XnValue& Value)
	{
		return (const XnChar*&)Value;
	}
};

/**
* Declares a hash table from strings to @a ValueType that's named @a ClassName and uses @a ValueTranslator
* to translate values. It is declared using the @a decl declspec.
*/
#define XN_DECLARE_STRINGS_HASH_WITH_TRANSLATOR_DECL(decl, ValueType, ClassName, ValueTranslator)								\
	XN_DECLARE_HASH_DECL(decl, const XnChar*, ValueType, ClassName, XnStringsKeyTranslator, ValueTranslator, XnStringsKeyManager)	\

/**
* Declares a hash table from strings to @a ValueType that's named @a ClassName and uses @a ValueTranslator
* to translate values.
*/
#define XN_DECLARE_STRINGS_HASH_WITH_TRANSLATOR(ValueType, ClassName, ValueTranslator)								\
	XN_DECLARE_STRINGS_HASH_WITH_TRANSLATOR_DECL(, ValueType, ClassName, ValueTranslator)

/**
* Declares a hash table from strings to @a ValueType that's named @a ClassName and uses default translator
* to translate values. It is declared using the @a decl declspec.
*/
#define XN_DECLARE_STRINGS_HASH_DECL(decl, ValueType, ClassName)														\
	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR_DECL(decl, ValueType, XN_DEFAULT_TRANSLATOR_NAME(ClassName))					\
	XN_DECLARE_STRINGS_HASH_WITH_TRANSLATOR_DECL(decl, ValueType, ClassName, XN_DEFAULT_TRANSLATOR_NAME(ClassName))	\

/**
* Declares a hash table from strings to @a ValueType that's named @a ClassName and uses default translator
* to translate values.
*/
#define XN_DECLARE_STRINGS_HASH(ValueType, ClassName)		\
	XN_DECLARE_STRINGS_HASH_DECL(, ValueType, ClassName)

XN_DECLARE_STRINGS_HASH(XnValue, XnStringsHash)


#endif //_XN_STRINGS_HASH_H
