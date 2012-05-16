#ifndef _XN_STRINGS_HASH_T_H_
#define _XN_STRINGS_HASH_T_H_ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnHashT.h"

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
class XnStringsHashKeyManager
{
public:
	static XnHashCode Hash(const XnChar* const& key)
	{
		XnUInt32 nCRC = 0;
		xnOSStrCRC32(key, &nCRC);

		// convert from UINT32 to XnHashValue
		return nCRC % (1 << (sizeof(XnHashCode)*8));
	}

	static XnInt32 Compare(const XnChar* const& key1, const XnChar* const& key2)
	{
		return strcmp(key1, key2);
	}
};

template<class TValue>
class XnStringsNodeAllocator
{
public:
	typedef XnKeyValuePair<const XnChar*, TValue> TPair;
	typedef XnLinkedNodeT<TPair> TLinkedNode;

	static TLinkedNode* Allocate(TPair const& pair)
	{
		XnChar* pKeyCopy = xnOSStrDup(pair.Key());
		if (pKeyCopy == NULL)
		{
			return NULL;
		}

		return XN_NEW(TLinkedNode, TPair(pKeyCopy, pair.Value()));
	}

	static void Deallocate(TLinkedNode* pNode)
	{
		XN_ASSERT(pNode != NULL);
		XN_ASSERT(pNode->value.Key() != NULL);

		xnOSFree(pNode->value.Key());
		XN_DELETE(pNode);
	}
};

template<class TValue>
class XnStringsHashT : public XnHashT<const XnChar*, TValue, XnStringsHashKeyManager, XnStringsNodeAllocator<TValue> >
{
	typedef XnHashT<const XnChar*, TValue, XnStringsHashKeyManager, XnStringsNodeAllocator<TValue> > Base;

public:
	XnStringsHashT() : Base() {}

	XnStringsHashT(const XnStringsHashT& other) : Base()
	{
		*this = other;
	}

	XnStringsHashT& operator=(const XnStringsHashT& other)
	{
		Base::operator=(other);
		// no other members
		return *this;
	}
};

class XnStringsSet : public XnStringsHashT<void*>
{
	typedef XnStringsHashT<void*> Base;

public:
	XnStatus Set(const XnChar* key)
	{
		return Base::Set(key, NULL);
	}
};

#endif // _XN_STRINGS_HASH_T_H_