#ifndef _XN_HASH_T_H_
#define _XN_HASH_T_H_ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <XnListT.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
typedef XnUInt8 XnHashCode;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
template<class _TKey, class _TValue>
struct XnKeyValuePair
{
	typedef _TKey TKey;
	typedef _TValue TValue;

	XnKeyValuePair() : key(TKey()), value(TValue()) {}
	XnKeyValuePair(TKey key, TValue value) : key(key), value(value) {}
	XnKeyValuePair(const XnKeyValuePair& other) : key(other.key), value(other.value) {}

public:
	TKey const& Key() const { return key; }
	TValue const& Value() const { return value; }
	TValue& Value() { return value; }

private:
	TKey key;
	TValue value;
};

template<class TKey>
class XnDefaultKeyManagerT
{
public:
	static XnHashCode Hash(TKey const& key)
	{
		return (((XnSizeT)key) & 0xff);
	}

	static XnInt32 Compare(TKey const& key1, TKey const& key2)
	{
		return XnInt32(XnSizeT(key1)-XnSizeT(key2));
	}
};

template<class TKey, 
		class TValue, 
		class TKeyManager = XnDefaultKeyManagerT<TKey>, 
		class TAlloc = XnLinkedNodeDefaultAllocatorT<XnKeyValuePair<TKey, TValue> > >
class XnHashT
{
public:
	typedef XnKeyValuePair<TKey, TValue> TPair;
	typedef XnListT<TPair, TAlloc> TPairList;

	enum 
	{ 
		LAST_BIN = (1 << (sizeof(XnHashCode)*8)),
		NUM_BINS = LAST_BIN + 1,
	};

	class ConstIterator
	{
	public:
		ConstIterator() : m_ppBins(NULL), m_nCurrBin(0)
		{}

		ConstIterator(TPairList* const* apBins, XnUInt32 nCurrBin, typename TPairList::ConstIterator currIt)
			: m_ppBins(apBins), m_nCurrBin(nCurrBin), m_currIt(currIt)
		{
			if (nCurrBin != LAST_BIN && m_currIt == m_ppBins[m_nCurrBin]->End())
			{
				// this does not point to an actual entry. run to next one.
				++*this;
			}
		}

		ConstIterator(const ConstIterator& other)
			: m_ppBins(other.m_ppBins), m_nCurrBin(other.m_nCurrBin), m_currIt(other.m_currIt)
		{}

		/**
		* Support ++iterator, go to the next object in the list
		*/
		ConstIterator& operator++()
		{
			XN_ASSERT(m_nCurrBin != LAST_BIN);

			// increment internal bin iterator
			if (m_currIt != m_ppBins[m_nCurrBin]->End())
			{
				++m_currIt;
			}

			// check if we need to move to next bin
			if (m_currIt == m_ppBins[m_nCurrBin]->End())
			{
				// go forward through bins, until we either reach the end or a non-empty bin
				do 
				{
					++m_nCurrBin;
				} while (m_nCurrBin < LAST_BIN && 
						(m_ppBins[m_nCurrBin] == NULL || m_ppBins[m_nCurrBin]->IsEmpty()));

				m_currIt = m_ppBins[m_nCurrBin]->Begin();
			}

			return *this;
		}

		/**
		* Support iterator++, go to the next object in the list, returning the old value
		*/
		ConstIterator operator++(int)
		{
			ConstIterator retVal(*this);
			++*this;
			return retVal;
		}

		/**
		* Support --iterator, go to the previous object in the hash
		*/
		ConstIterator& operator--()
		{
			XN_ASSERT(m_nCurrBin != LAST_BIN);

			// decrement internal bin iterator
			if (m_currIt != m_ppBins[m_nCurrBin]->ReverseEnd())
			{
				--m_currIt;
			}

			// check if we need to move to previous bin
			if (m_currIt == m_ppBins[m_nCurrBin]->ReverseEnd())
			{
				// go backwards through bins, until we either reach the end or a non-empty bin
				do 
				{
					if (m_nCurrBin == 0)
					{
						m_nCurrBin = LAST_BIN;
						break;
					}
					else
					{
						--m_nCurrBin;
					}
				} while (m_ppBins[m_nCurrBin] == NULL || m_ppBins[m_nCurrBin]->IsEmpty());

				m_currIt = m_ppBins[m_nCurrBin]->Begin();
			}

			return *this;
		}

		/**
		* Support iterator--, go to the previous object in the hash, returning the old value
		*/
		ConstIterator operator--(int)
		{
			ConstIterator retVal(*this);
			--*this;
			return retVal;
		}

		/**
		* Operator to check if two iterators point to the same object
		* 
		* @param	other	[in]	instance to compare with
		*/
		inline XnBool operator==(const ConstIterator& other) const
		{
			return m_currIt == other.m_currIt;
		}

		/**
		* Operator to check if two iterators point to different objects
		* 
		* @param	other	[in]	instance to compare with
		*/
		inline XnBool operator!=(const ConstIterator& other) const
		{
			return m_currIt != other.m_currIt;
		}

		/**
		* Get the value of the current object (const version)
		*/
		inline TPair const& operator*() const
		{
			return *m_currIt;
		}

		/**
		* Get a pointer to the value of the current object (const version)
		*/
		inline TPair const* operator->() const
		{
			return m_currIt.operator->();
		}

	protected:
		friend class XnHashT;

		TPairList* const* m_ppBins;
		XnUInt32 m_nCurrBin;
		typename TPairList::ConstIterator m_currIt;
	};

	class Iterator : public ConstIterator
	{
	public:
		Iterator() : ConstIterator()
		{}

		Iterator(TPairList** apBins, XnUInt32 nCurrBin, typename TPairList::Iterator currIt)
			: ConstIterator(apBins, nCurrBin, currIt)
		{}

		Iterator(const Iterator& other) : ConstIterator(other)
		{}

		/**
		* Support ++iterator, go to the next object in the list
		*/
		Iterator& operator++()
		{
			++(*(ConstIterator*)this);
			return (*this);
		}

		/**
		* Support iterator++, go to the next object in the list, returning the old value
		*/
		inline Iterator operator++(int) 
		{ 
			Iterator retVal(*this);
			++*this;
			return (retVal);
		}
		
		/**
		* Support --iterator, go to the next object in the list
		*/
		inline Iterator& operator--() 
		{ 
			--(*(ConstIterator*)this); 
			return (*this);
		}

		/**
		* Support iterator--, go to the next object in the list, returning the old value
		*/
		inline Iterator operator--(int)
		{ 
			Iterator retVal(*this);
			--*this;
			return (retVal);
		}

		/**
		* Get the value of the current object
		*/
		inline TPair& operator*() const 
		{
			return const_cast<TPair&>(*this->m_currIt);
		}

		/**
		* Get a pointer to the value of the current object
		*/
		inline TPair* operator->() const
		{
			return const_cast<TPair*>(this->m_currIt.operator->());
		}
	};

	XnHashT()
	{
		Init();
	}

	XnHashT(const XnHashT& other)
	{
		Init();
		*this = other;
	}

	XnHashT& operator=(const XnHashT& other)
	{
		Clear();

		XnStatus nRetVal = XN_STATUS_OK;

		for (ConstIterator it = other.Begin(); it != other.End(); ++it)
		{
			nRetVal = Set(it->Key(), it->Value());
			XN_ASSERT(nRetVal == XN_STATUS_OK);
		}

		return *this;
	}

	~XnHashT()
	{
		// NOTE: we don't want to delete LAST_BIN (it points to the m_lastBin member)
		for (XnUInt32 i = 0; i < LAST_BIN; ++i)
		{
			if (m_apBins[i] != NULL)
			{
				XN_DELETE(m_apBins[i]);
			}
		}
	}

	/**
	* An iterator to the first entry of the list (non-const version)
	*/
	Iterator Begin()
	{
		return Iterator(m_apBins, m_nMinBin, m_apBins[m_nMinBin]->Begin());
	}

	/**
	* An iterator to the first entry of the list (const version)
	*/
	ConstIterator Begin() const
	{
		return ConstIterator(m_apBins, m_nMinBin, m_apBins[m_nMinBin]->Begin());
	}

	/**
	* An iterator 1to the end of the list (non-const version). The position is invalid.
	*/
	Iterator End()
	{
		return Iterator(m_apBins, LAST_BIN, m_apBins[LAST_BIN]->Begin());
	}

	/**
	* An iterator to the end of the list (const version). The position is invalid.
	*/
	ConstIterator End() const
	{
		return ConstIterator(m_apBins, LAST_BIN, m_apBins[LAST_BIN]->Begin());
	}

	/**
	* Set a new key-value entry. If key exists, will replace value.
	* 
	* @param	key		[in]	The key to which to associate the value
	* @param	value	[in]	The value to add to the XnHash
	*/
	XnStatus Set(const TKey& key, const TValue& value)
	{
		XnHashCode nHash = TKeyManager::Hash(key);

		// check if bin exists
		if (m_apBins[nHash] == NULL)
		{
			// create it
			XN_VALIDATE_NEW(m_apBins[nHash], TPairList);

			if (nHash < m_nMinBin)
			{
				m_nMinBin = nHash;
			}
		}

		// now check if key is already in the bin
		for (typename TPairList::Iterator it = m_apBins[nHash]->Begin(); it != m_apBins[nHash]->End(); ++it)
		{
			if (TKeyManager::Compare(it->Key(), key) == 0)
			{
				// replace it
				it->Value() = value;
				return (XN_STATUS_OK);
			}
		}

		// if we got here, key is not in bin. Add it.
		return m_apBins[nHash]->AddLast(TPair(key, value));
	}

	/**
	* Get an iterator pointing to the pair in the hash (const version).
	* 
	* @param	key		[in]	The searched key 
	*
	* @return	End()	if value doesn't exist
	*/
	ConstIterator Find(TKey const& key) const
	{
		XnUInt32 nBin = LAST_BIN;
		typename TPairList::ConstIterator it;
		if (TRUE == Find(key, nBin, it))
		{
			return ConstIterator(m_apBins, nBin, it);
		}
		else
		{
			return End();
		}
	}

	/**
	* Get an iterator pointing to the pair in the hash.
	* 
	* @param	key		[in]	The searched key 
	*
	* @return	End()	if value doesn't exist
	*/
	Iterator Find(TKey const& key)
	{
		XnUInt32 nBin = LAST_BIN;
		typename TPairList::Iterator it;
		if (TRUE == Find(key, nBin, it))
		{
			return Iterator(m_apBins, nBin, it);
		}
		else
		{
			return End();
		}
	}

	/**
	* Get an iterator pointing to the pair in the hash (const version).
	* 
	* @param	key		[in]	The searched key 
	* @param	it		[out]	An iterator to the entry in the hash.
	*
	* @return	XN_STATUS_NO_MATCH	if value doesn't exist
	*/
	XnStatus Find(TKey const& key, ConstIterator& it) const
	{
		it = Find(key);
		return (it == End() ? XN_STATUS_NO_MATCH : XN_STATUS_OK);
	}

	/**
	* Get an iterator pointing to the pair in the hash (const version).
	* 
	* @param	key		[in]	The searched key 
	* @param	it		[out]	An iterator to the entry in the hash.
	*
	* @return	XN_STATUS_NO_MATCH	if value doesn't exist
	*/
	XnStatus Find(TKey const& key, Iterator& it)
	{
		it = Find(key);
		return (it == End() ? XN_STATUS_NO_MATCH : XN_STATUS_OK);
	}

	/**
	* Get the value associated with the supplied key
	* 
	* @param	key		[in]	The key of the entry
	* @param	value	[out]	The retrieved value
	*
	* @return	XN_STATUS_NO_MATCH	if no such key exists
	*/
	XnStatus Get(TKey const& key, TValue& value) const
	{
		ConstIterator it = Find(key);
		if (it == End())
		{
			return XN_STATUS_NO_MATCH;
		}
		else
		{
			value = it->Value();
			return XN_STATUS_OK;
		}
	}

	/**
	* Get a pointer to the value associated with the supplied key
	* 
	* @param	key		[in]	The key of the entry
	* @param	pValue	[out]	A const pointer to the value that is stored in the hash.
	*
	* @return	XN_STATUS_NO_MATCH	if no such key exists
	*/
	XnStatus Get(TKey const& key, TValue const*& pValue) const
	{
		ConstIterator it = Find(key);
		if (it == End())
		{
			return XN_STATUS_NO_MATCH;
		}
		else
		{
			pValue = &it->Value();
			return XN_STATUS_OK;
		}
	}

	/**
	* Get the value associated with the supplied key
	* 
	* @param	key		[in]	The key of the entry
	* @param	value	[out]	The retrieved value
	*
	* @return	XN_STATUS_NO_MATCH	if no such key exists
	*/
	XnStatus Get(TKey const& key, TValue& value)
	{
		Iterator it = Find(key);
		if (it == End())
		{
			return XN_STATUS_NO_MATCH;
		}
		else
		{
			value = it->Value();
			return XN_STATUS_OK;
		}
	}

	/**
	* Get a pointer to the value associated with the supplied key
	* 
	* @param	key		[in]	The key of the entry
	* @param	pValue	[out]	A pointer to the value that is stored in the hash
	*
	* @return	XN_STATUS_NO_MATCH	if no such key exists
	*/
	XnStatus Get(TKey const& key, TValue*& pValue)
	{
		Iterator it = Find(key);
		if (it == End())
		{
			return XN_STATUS_NO_MATCH;
		}
		else
		{
			pValue = &it->Value();
			return XN_STATUS_OK;
		}
	}

	/**
	* Gets a reference to the value of a specific key. If this key is not in the hash, it will be added.
	*
	* @param	key				[in]	The key of the entry.
	*/
	TValue& operator[](TKey const& key)
	{
		XnStatus nRetVal = XN_STATUS_OK;
		Iterator it = Find(key);
		if (it == End())
		{
			nRetVal = Set(key, TValue());
			XN_ASSERT(nRetVal == XN_STATUS_OK);

			it = Find(key);
			XN_ASSERT(it != End());
		}

		return it->Value();
	}

	XnStatus Remove(ConstIterator it)
	{
		// Verify iterator is valid
		if (it == End())
		{
			XN_ASSERT(FALSE);
			return XN_STATUS_ILLEGAL_POSITION;
		}

		XN_ASSERT(m_apBins == it.m_ppBins);
		XN_ASSERT(m_apBins[it.m_nCurrBin] != NULL);

		return m_apBins[it.m_nCurrBin]->Remove(it.m_currIt);
	}

	XnStatus Remove(TKey const& key)
	{
		ConstIterator it = Find(key);
		if (it != End())
		{
			return Remove(it);
		}
		else
		{
			return XN_STATUS_NO_MATCH;
		}
	}

	/**
	* Remove all entries from the XnHash.
	*/
	XnStatus Clear()
	{
		while (Begin() != End())
			Remove(Begin());

		return XN_STATUS_OK;
	}

	/**
	* Checks if hash is empty.
	*/
	XnBool IsEmpty() const
	{
		return (Begin() == End());
	}

	/**
	* Gets the number of entries in the hash.
	*/
	XnUInt32 Size() const
	{
		XnUInt32 nSize = 0;
		for (ConstIterator iter = Begin(); iter != End(); ++iter, ++nSize)
			;

		return nSize;
	}

private:
	XnBool Find(TKey const& key, XnUInt32& nBin, typename TPairList::ConstIterator& currIt) const
	{
		XnHashCode nHash = TKeyManager::Hash(key);

		if (m_apBins[nHash] != NULL)
		{
			// look for value in bin
			for (typename TPairList::ConstIterator it = m_apBins[nHash]->Begin(); it != m_apBins[nHash]->End(); ++it)
			{
				if (TKeyManager::Compare(it->Key(), key) == 0)
				{
					nBin = nHash;
					currIt = it;
					return TRUE;
				}
			}
		}

		// if we got here, key wasn't found
		return FALSE;
	}

	void Init()
	{
		xnOSMemSet(m_apBins, 0, sizeof(m_apBins));
		m_apBins[LAST_BIN] = &m_lastBin;
		m_nMinBin = LAST_BIN;
	}

	TPairList* m_apBins[NUM_BINS];
	TPairList m_lastBin;
	XnUInt32 m_nMinBin;
};



#endif // _XN_HASH_T_H_