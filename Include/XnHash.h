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
#ifndef _XN_HASH_H
#define _XN_HASH_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnList.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_HASH_LAST_BIN 256
#define XN_HASH_NUM_BINS (XN_HASH_LAST_BIN + 1)
//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/**
* The key to the hash
*/
typedef XnValue XnKey;

/**
* The hash value - the output of the hash function
*/
typedef XnUInt8 XnHashValue;

/**
* Default Hash function
*/
static XnHashValue XnDefaultHashFunction(const XnKey& key)
{
	return (XnSizeT(key) & 0xff);
}

/**
* Default Compare function
*/
static XnInt32 XnDefaultCompareFunction(const XnKey& key1, const XnKey& key2)
{
	return XnInt32(XnSizeT(key1)-XnSizeT(key2));
}

/**
 * The hash - associative array
 */
class XnHash
{
public:
	/**
	* Iterator to the XnHash
	*/
	class ConstIterator
	{
	public:
		friend class XnHash;

		/**
		* Copy constructor
		* 
		* @param	other	[in]	instance to copy from
		*/
		ConstIterator(const ConstIterator& other) :
			m_pHash(other.m_pHash), m_nCurrentBin(other.m_nCurrentBin), m_Iterator(other.m_Iterator) {}

		/**
		* Support ++iterator, go to the next object in the hash
		*/
		ConstIterator& operator++()
		{
			++m_Iterator;

			while (m_Iterator == m_pHash->m_Bins[m_nCurrentBin]->end() &&
				m_Iterator != m_pHash->m_Bins[XN_HASH_LAST_BIN]->end())
			{
				do
				{
					m_nCurrentBin++;
				} while (m_pHash->m_Bins[m_nCurrentBin] == NULL);
				m_Iterator = m_pHash->m_Bins[m_nCurrentBin]->begin();
			}
			return *this;
		}

		/**
		* Support iterator++, go to the next object in the hash, returning the old value
		*/
		ConstIterator operator++(int)
		{
			XnHash::ConstIterator other(*this);
			++*this;
			return other;
		}

		/**
		* Support --iterator, go to the previous object in the hash
		*/
		ConstIterator& operator--()
		{
			--m_Iterator;

			while (m_Iterator == m_pHash->m_Bins[m_nCurrentBin]->end() &&
				m_Iterator != m_pHash->m_Bins[XN_HASH_LAST_BIN]->end())
			{
				do 
				{
					if (m_nCurrentBin == 0)
					{
						m_nCurrentBin = XN_HASH_LAST_BIN;
						m_Iterator = m_pHash->m_Bins[XN_HASH_LAST_BIN]->end();
						return *this;
					}
					m_nCurrentBin--;
				} while (m_pHash->m_Bins[m_nCurrentBin] == NULL);
				m_Iterator = m_pHash->m_Bins[m_nCurrentBin]->rbegin();
			}
			return *this;
		}

		/**
		* Support iterator--, go to the previous object in the hash, returning the old value
		*/
		ConstIterator operator--(int)
		{
			ConstIterator other(*this);
			--*this;
			return other;
		}

		/**
		* Operator to check if 2 iterators point to the same object
		* 
		* @param	other	[in]	instance to compare with
		*/
		XnBool operator==(const ConstIterator& other) const
		{
			return m_Iterator == other.m_Iterator;
		}

		/**
		* Operator to check if 2 iterators point to different objects
		* 
		* @param	other	[in]	instance to compare with
		*/
		XnBool operator!=(const ConstIterator& other) const
		{
			return m_Iterator != other.m_Iterator;
		}

		/**
		* Get the key of the current object (const version)
		*/
		const XnKey& Key() const
		{
			return ((XnNode*)(*m_Iterator))->Data();
		}

		/**
		* Get the value of the current object (const version)
		*/
		const XnValue& Value() const
		{
			return ((XnNode*)(*m_Iterator))->Next()->Data();
		}

		/**
		* Get the entire current object (non-const version)
		*/
		XnNode* GetNode()
		{
			return m_Iterator.GetNode();
		}

		/**
		* Get the entire current object (const version)
		*/
		const XnNode* GetNode() const
		{
			return m_Iterator.GetNode();
		}

	protected:
		/**
		* constructor to be used from inside the XnHash
		* 
		* @param	pHash			[in]	The hash to which the iterator belongs
		* @param	nBin			[in]	The bin of the current object
		* @param	listIterator	[in]	Iterator on the bin (each bin is a XnList)
		*/
		ConstIterator(const XnHash* pHash, XnUInt16 nBin, XnList::Iterator listIterator) :
			 m_pHash(pHash), m_nCurrentBin(nBin), m_Iterator(listIterator)
			 {
				 // Find the first valid
				 while (m_Iterator == m_pHash->m_Bins[m_nCurrentBin]->end() &&
					 m_Iterator != m_pHash->m_Bins[XN_HASH_LAST_BIN]->end())
				 {
					 do
					 {
						 m_nCurrentBin++;
					 } while (m_pHash->m_Bins[m_nCurrentBin] == NULL);
					 m_Iterator = m_pHash->m_Bins[m_nCurrentBin]->begin();
				 }
			 }

		/**
		* constructor to be used from inside the XnHash. It points to the first value in the hash.
		* 
		* @param	pHash	[in]	The hash to which the iterator belongs
		*/
		ConstIterator(const XnHash* pHash) : 
			 m_pHash(pHash), m_nCurrentBin(0), m_Iterator(m_pHash->m_Bins[XN_HASH_LAST_BIN]->end()) {}

		/** The hash to which the iterator belongs */
		const XnHash* m_pHash;
		/** The bin of the current object */
		XnUInt16 m_nCurrentBin;
		/** Iterator for the specific bin */
		XnList::Iterator m_Iterator;
	};

	/**
	* Iterator to the XnHash
	*/
	class Iterator : public ConstIterator
	{
	public:
		friend class XnHash;

		/**
		* Copy constructor
		* 
		* @param	other	[in]	instance to copy from
		*/
		inline Iterator(const Iterator& other) : ConstIterator(other) {}

		/**
		* Support ++iterator, go to the next object in the list
		*/
		inline Iterator& operator++() 
		{ 
			++(*(ConstIterator*)this);
			return (*this);
		}
		/**
		* Support iterator++, go to the next object in the list, returning the old value
		*/
		inline Iterator operator++(int) 
		{ 
			Iterator result = *this;
			++*this;
			return (result);
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
			Iterator result = *this;
			--*this;
			return (result);
		}

		/**
		* Get the key of the current object (const version)
		*/
		XnKey& Key() const { return (XnKey&)ConstIterator::Key(); }

		/**
		* Get the value of the current object (const version)
		*/
		XnValue& Value() const { return (XnValue&)ConstIterator::Value(); }

	protected:
		/**
		* constructor to be used from inside the XnHash
		* 
		* @param	pHash			[in]	The hash to which the iterator belongs
		* @param	nBin			[in]	The bin of the current object
		* @param	listIterator	[in]	Iterator on the bin (each bin is a XnList)
		*/
		Iterator(const XnHash* pHash, XnUInt16 nBin, XnList::Iterator listIterator) :
			ConstIterator(pHash, nBin, listIterator)
		{}

		/**
		* constructor to be used from inside the XnHash. It points to the first value in the hash.
		* 
		* @param	pHash	[in]	The hash to which the iterator belongs
		*/
		Iterator(const XnHash* pHash) : ConstIterator(pHash) {}

		Iterator(const ConstIterator& other) : ConstIterator(other) {}
	};

	friend class ConstIterator;

public:
	/**
	 * Definition of a hash function - receive key, and outputs HashValue
	 */
	typedef XnHashValue (*XnHashFunction)(const XnKey& key);
	/**
	 * Definition of comparison function - receives 2 keys, returns 0 on equality
	 */
	typedef XnInt32 (*XnCompareFunction)(const XnKey& key1, const XnKey& key2);

	/**
	* Constructor. Initializes internal representations.
	*/
	XnHash()
	{
		m_nInitStatus = Init();
	}

	/**
	* Copy constructor
	*/
	XnHash(const XnHash& other)
	{
		m_nInitStatus = Init();
		if (m_nInitStatus == XN_STATUS_OK)
		{
			m_nMinBin = other.m_nMinBin;
			m_CompareFunction = other.m_CompareFunction;
			m_HashFunction = other.m_HashFunction;
			for (int i = 0; i < XN_HASH_NUM_BINS; i++)
			{
				if (other.m_Bins[i] != NULL)
				{
					m_Bins[i] = XN_NEW(XnList);
					if (m_Bins[i] == NULL)
					{
						m_nInitStatus = XN_STATUS_ALLOC_FAILED;
						return;
					}
					*(m_Bins[i]) = *(other.m_Bins[i]);
				}
			}
		}
	}

	/**
	* Destructor. Destroys internal representations.
	*/
	virtual ~XnHash()
	{
		if (m_Bins != NULL)
		{
			for (int i = 0; i < XN_HASH_NUM_BINS; ++i)
			{
				XN_DELETE(m_Bins[i]);
			}
			XN_DELETE_ARR(m_Bins);
		}
	}

	/**
	* Returns the status of the initialization of the hash.
	* @returns XN_STATUS_OK if the hash was initialized successfully, or an error code otherwise 
	* (e.g. if memory could not be allocated).
	*/
	XnStatus GetInitStatus() const
	{
		return m_nInitStatus;
	}

	/**
	* Set a new key-value entry. If key exists, will replace value.
	* 
	* @param	key		[in]	The key to which to associate the value
	* @param	value	[in]	The value to add to the XnHash
	*/
	XnStatus Set(const XnKey& key, const XnValue& value)
	{
		XnHashValue HashValue = (*m_HashFunction)(key);

		// Check if key already exists
		if (m_Bins[HashValue] != NULL)
		{
			Iterator hiter(this);
			if (Find(key, HashValue, hiter) == XN_STATUS_OK)
			{
				// Replace value
				hiter.Value() = value;
				return XN_STATUS_OK;
			}
		}
		else
		{
			// First time trying to access this bin, create it.
			m_Bins[HashValue] = XN_NEW(XnList);
			if (m_Bins[HashValue] == NULL)
			{
				return XN_STATUS_ALLOC_FAILED;
			}
			if (HashValue < m_nMinBin)
				m_nMinBin = HashValue;
		}

		// Get a new node for the key
		XnNode* pKeyNode = XnNode::Allocate();
		if (pKeyNode == NULL)
		{
			return XN_STATUS_ALLOC_FAILED;
		}
		pKeyNode->Data() = key;

		// Get a new node for the value
		XnNode* pValueNode = XnNode::Allocate();
		if (pValueNode == NULL)
		{
			XnNode::Deallocate(pKeyNode);
			return XN_STATUS_ALLOC_FAILED;
		}
		pValueNode->Data() = value;

		// Concatenate the value node to the key node
		pKeyNode->Next() = pValueNode;
		pValueNode->Next() = NULL;

		// Add the 2 nodes as the value to the key's list
		XnStatus ListStatus = m_Bins[HashValue]->AddLast(XnValue(pKeyNode));
		if (ListStatus != XN_STATUS_OK)
		{
			// Add failed. return the 2 nodes to the pool
			XnNode::Deallocate(pKeyNode);
			XnNode::Deallocate(pValueNode);
			return ListStatus;
		}

		return XN_STATUS_OK;
	}

	/**
	* Get the value associated with the supplied key
	* 
	* @param	key		[in]	The key of the entry
	* @param	value	[out]	The retrieved value
	*
	* @return	XN_STATUS_NO_MATCH	if no such key exists
	*/
	XnStatus Get(const XnKey& key, XnValue& value) const
	{
		// Check if key exists
		Iterator hiter(this);
		XnStatus FindStatus = Find(key, hiter);
		if (FindStatus != XN_STATUS_OK)
		{
			// Key doesn't exist!
			return FindStatus;
		}
		value = hiter.Value();

		return XN_STATUS_OK;
	}

	/**
	* Remove a key-value entry from the XnHash
	* 
	* @param	key		[in]	The key of the entry
	* @param	value	[out]	The value that was in the removed entry

	* @return	XN_STATUS_NO_MATCH	if no such key exists	
	*/
	XnStatus Remove(const XnKey& key, XnValue& value)
	{
		// find the entry to which the key belongs
		Iterator hiter(this);

		XnStatus FindStatus = Find(key, hiter);
		if (FindStatus != XN_STATUS_OK)
		{
			// no such entry!
			return FindStatus;
		}

		// Remove by iterator
		value = hiter.Value();
		return Remove(hiter);
	}

	/**
	* Remove an entry from the XnHash by iterator
	* 
	* @param	iter	[in]	Iterator pointing to an entry in the hash
	* @param	key		[out]	The key that was in the removed entry
	* @param	value	[out]	The value that was in the removed entry
	*
	* @return	XN_STATUS_ILLEGAL_POSITION	if iterator is invalid
	*/
	XnStatus Remove(ConstIterator iter, XnKey& key, XnValue& value)
	{
		if (iter == end())
		{
			//  Can't remove invalid node
			return XN_STATUS_ILLEGAL_POSITION;
		}

		// Get value and key, to return to the caller
		value = iter.Value();
		key = iter.Key();

		return Remove(iter);
	}

	/**
	* Remove an entry from the XnHash by iterator
	* 
	* @param	iter	[in]	Iterator pointing to an entry in the hash
	*
	* @return	XN_STATUS_ILLEGAL_POSITION	if iterator is invalid
	*/
	virtual XnStatus Remove(ConstIterator iter)
	{
		if (iter == end())
		{
			//  Can't remove invalid node
			return XN_STATUS_ILLEGAL_POSITION;
		}

		XnNode* pNode = iter.GetNode();

		XnNode* pKeyNode = (XnNode*)(pNode->Data());
		XnNode* pValueNode = pKeyNode->Next();

		// Return the nodes to the pool
		XnNode::Deallocate(pKeyNode);
		XnNode::Deallocate(pValueNode);

		pNode->Previous()->Next() = pNode->Next();
		pNode->Next()->Previous() = pNode->Previous();

		XnNode::Deallocate(pNode);

		return XN_STATUS_OK;
	}


	/**
	* Remove all entries from the XnHash.
	*/
	XnStatus Clear()
	{
		while (begin() != end())
			Remove(begin());

		return XN_STATUS_OK;
	}

	/**
	* Checks if hash is empty.
	*/
	XnBool IsEmpty() const
	{
		return (begin() == end());
	}

	/**
	* Gets the number of entries in the hash.
	*/
	XnUInt32 Size() const
	{
		XnUInt32 nSize = 0;
		for (Iterator iter = begin(); iter != end(); ++iter, ++nSize)
			;

		return nSize;
	}

	/**
	* Get an iterator pointing to the entry to which the key belongs
	* 
	* @param	key		[in]	The key of the entry
	* @param	hiter	[out]	Iterator to the entry described by key
	*
	* @return	XN_STATUS_NO_MATCH	No such key in the XnHash
	*/
	XnStatus Find(const XnKey& key, ConstIterator& hiter) const
	{
		return ConstFind(key, hiter);
	}

	/**
	* Get an iterator pointing to the entry to which the key belongs
	* 
	* @param	key		[in]	The key of the entry
	* @param	hiter	[out]	Iterator to the entry described by key
	*
	* @return	XN_STATUS_NO_MATCH	No such key in the XnHash
	*/
	XnStatus Find(const XnKey& key, Iterator& hiter)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		ConstIterator& it = hiter;
		nRetVal = ConstFind(key, it);
		XN_IS_STATUS_OK(nRetVal);

		return (XN_STATUS_OK);
	}

	/**
	* Get an iterator to the beginning of the XnHash (non-const version)
	*/
	Iterator begin()
	{
		return Iterator(this, m_nMinBin, m_Bins[m_nMinBin]->begin());
	}

	/**
	* Get an iterator to the beginning of the XnHash (const version)
	*/
	ConstIterator begin() const
	{
		return ConstIterator(this, m_nMinBin, m_Bins[m_nMinBin]->begin());
	}

	/**
	* Get an iterator to the end of the XnHash (position is invalid) (non-const version)
	*/
	Iterator end()
	{
		return Iterator(this, XN_HASH_LAST_BIN, m_Bins[XN_HASH_LAST_BIN]->end());
	}

	/**
	* Get an iterator to the end of the XnHash (position is invalid) (const version)
	*/
	ConstIterator end() const
	{
		return ConstIterator(this, XN_HASH_LAST_BIN, m_Bins[XN_HASH_LAST_BIN]->end());
	}

	/**
	* Change the hash function. The XnHash must be empty for this to succeed.
	* 
	* @param	hashFunction	[in]	The new hash function
	*
	* @return XN_STATUS_IS_NOT_EMPTY	if the XnHash isn't empty
	*/
	XnStatus SetHashFunction(XnHashFunction hashFunction)
	{
		if (begin() != end())
		{
			return XN_STATUS_IS_NOT_EMPTY;
		}
		m_HashFunction = hashFunction;
		return XN_STATUS_OK;
	}

	/**
	* Change the comparison function. The XnHash must be empty for this to succeed.
	* 
	* @param	compareFunction	[in]	The new hash function
	*
	* @return XN_STATUS_IS_NOT_EMPTY	if the XnHash isn't empty
	*/
	XnStatus SetCompareFunction(XnCompareFunction compareFunction)
	{
		if (begin() != end())
		{
			return XN_STATUS_IS_NOT_EMPTY;
		}
		m_CompareFunction = compareFunction;
		return XN_STATUS_OK;
	}

protected:

	XnStatus Init()
	{
		m_Bins = XN_NEW_ARR(XnList*, XN_HASH_NUM_BINS);
		XN_VALIDATE_ALLOC_PTR(m_Bins);

		for (int i = 0; i < XN_HASH_NUM_BINS; i++)
		{
			m_Bins[i] = NULL;
		}

		m_Bins[XN_HASH_LAST_BIN] = XN_NEW(XnList); // We need this for  an end() iterator
		m_nMinBin = XN_HASH_LAST_BIN;

		XN_VALIDATE_ALLOC_PTR(m_Bins[XN_HASH_LAST_BIN]);
		m_CompareFunction = &XnDefaultCompareFunction;
		m_HashFunction = &XnDefaultHashFunction;
		return XN_STATUS_OK;
	}

	/**
	* Get an iterator pointing to the entry to which the key belongs
	* 
	* @param	key			[in]	The key of the entry
	* @param	hashValue	[in]	The hash value of the key
	* @param	hiter		[out]	Iterator to the entry described by key
	*
	* @return	XN_STATUS_NO_MATCH	No such key in the XnHash
	*/
	XnStatus Find(const XnKey& key, XnHashValue hashValue, ConstIterator& hiter) const
	{
		if (m_Bins[hashValue] != NULL)
		{
			hiter = ConstIterator(this, hashValue, m_Bins[hashValue]->begin());
			for (XnList::ConstIterator iter = m_Bins[hashValue]->begin();
				iter != m_Bins[hashValue]->end(); ++iter, ++hiter)
			{
				if ((*m_CompareFunction)(key, hiter.Key()) == 0)
					return XN_STATUS_OK;
			}
		}

		return XN_STATUS_NO_MATCH;
	}


	/** Each bin is a XnList */
	XnList** m_Bins;

	XnUInt16 m_nMinBin;

	/* Status of initialization - could be an error if memory could not be allocated. */
	XnStatus m_nInitStatus;

	/** The current hash function */
	XnHashFunction m_HashFunction;
	/** The current comparison function */
	XnCompareFunction m_CompareFunction;

private:
	XnStatus ConstFind(const XnKey& key, ConstIterator& hiter) const
	{
		XnHashValue HashValue = (*m_HashFunction)(key);
		return Find(key, HashValue, hiter);
	}
};

/**
* Declares a default Key Manager for type @a KeyType. It is given the name @a ClassName.
* The manager uses the translator @a KeyTranslator, and is declared using the @a decl declspec.
*/
#define XN_DECLARE_DEFAULT_KEY_MANAGER_DECL(decl, KeyType, ClassName, KeyTranslator)	\
	class decl ClassName																\
	{																					\
	public:																				\
		inline static XnHashValue Hash(KeyType const& key)								\
		{																				\
			const XnKey _key = KeyTranslator::GetAsValue(key);							\
			return XnDefaultHashFunction(_key);											\
		}																				\
		inline static XnInt32 Compare(KeyType const& key1, KeyType const& key2)		\
		{																				\
			const XnKey _key1 = KeyTranslator::GetAsValue(key1);						\
			const XnKey _key2 = KeyTranslator::GetAsValue(key2);						\
			return XnDefaultCompareFunction(_key1, _key2);								\
		}																				\
	};

/**
* Declares a default Key Manager for type @a KeyType. It is given the name @a ClassName.
* The manager uses the translator @a KeyTranslator.
*/
#define XN_DECLARE_DEFAULT_KEY_MANAGER(KeyType, ClassName, KeyTranslator)	\
	XN_DECLARE_DEFAULT_KEY_MANAGER_DECL(, KeyType, ClassName, KeyTranslator)

/**
* Declares a hash table from @a KeyType to @a ValueType named @a ClassName. The hash table uses
* @a KeyTranslator and @a ValueTranslator for storing keys and values, and @a KeyManager for managing the keys.
* It is declared using the @a decl declspec.
*/
#define XN_DECLARE_HASH_DECL(decl, KeyType, ValueType, ClassName, KeyTranslator, ValueTranslator, KeyManager)	\
	class decl ClassName : public XnHash															\
	{																								\
	public:																							\
		class decl ConstIterator : public XnHash::ConstIterator										\
		{																							\
		public:																						\
			friend class ClassName;																	\
			inline ConstIterator(const ConstIterator& other) : XnHash::ConstIterator(other) {}		\
			inline ConstIterator& operator++()														\
			{																						\
				++(*(XnHash::ConstIterator*)this);													\
				return (*this);																		\
			}																						\
			inline ConstIterator operator++(int)													\
			{																						\
				ConstIterator result = *this;														\
				++*this;																			\
				return result;																		\
			}																						\
			inline ConstIterator& operator--()														\
			{																						\
				--(*(XnHash::ConstIterator*)this);													\
				return (*this);																		\
			}																						\
			inline ConstIterator operator--(int)													\
			{																						\
				ConstIterator result = *this;														\
				--*this;																			\
				return result;																		\
			}																						\
			inline KeyType const& Key() const														\
			{																						\
				return KeyTranslator::GetFromValue(XnHash::ConstIterator::Key());					\
			}																						\
			inline ValueType const& Value() const													\
			{																						\
				return ValueTranslator::GetFromValue(XnHash::ConstIterator::Value());				\
			}																						\
		protected:																					\
			inline ConstIterator(const XnHash::ConstIterator& other) :								\
				XnHash::ConstIterator(other) {}														\
		};																							\
		class decl Iterator : public ConstIterator													\
		{																							\
		public:																						\
			friend class ClassName;																	\
			inline Iterator(const Iterator& other) : ConstIterator(other) {}						\
			inline Iterator& operator++()															\
			{																						\
				++(*(ConstIterator*)this);															\
				return (*this);																		\
			}																						\
			inline Iterator operator++(int)															\
			{																						\
				Iterator result = *this;															\
				++*this;																			\
				return result;																		\
			}																						\
			inline Iterator& operator--()															\
			{																						\
				--(*(ConstIterator*)this);															\
				return (*this);																		\
			}																						\
			inline Iterator operator--(int)															\
			{																						\
				Iterator result = *this;															\
				--*this;																			\
				return result;																		\
			}																						\
			inline KeyType& Key() const																\
			{																						\
				return (KeyType&)ConstIterator::Key();												\
			}																						\
			inline ValueType& Value() const															\
			{																						\
				return (ValueType&)ConstIterator::Value();											\
			}																						\
		protected:																					\
			inline Iterator(const XnHash::Iterator& other) : ConstIterator(other) {}				\
		};																							\
	public:																							\
		ClassName()																					\
		{																							\
			SetHashFunction(Hash);																	\
			SetCompareFunction(Compare);															\
		}																							\
		ClassName(const ClassName& other)															\
		{																							\
			SetHashFunction(Hash);																	\
			SetCompareFunction(Compare);															\
			*this = other;																			\
		}																							\
		virtual ~ClassName()																		\
		{																							\
			while (!IsEmpty())																		\
				Remove(begin());																	\
		}																							\
		ClassName& operator=(const ClassName& other)												\
		{																							\
			Clear();																				\
			for (ConstIterator it = other.begin(); it != other.end(); it++)							\
			{																						\
				m_nInitStatus = Set(it.Key(), it.Value());											\
				if (m_nInitStatus != XN_STATUS_OK)													\
				{																					\
					return *this;																	\
				}																					\
			}																						\
			return *this;																			\
		}																							\
		XnStatus Set(KeyType const& key, ValueType const& value)									\
		{																							\
		    Iterator oldIt = begin();                                                               \
		    if (Find(key, oldIt) == XN_STATUS_OK)                                                   \
			{                                                                                       \
				oldIt.Value() = value;																\
			}																						\
			else																					\
			{																						\
				XnKey _key = KeyTranslator::CreateValueCopy(key);									\
				XnValue _value = ValueTranslator::CreateValueCopy(value);							\
				XnStatus nRetVal = XnHash::Set(_key, _value);										\
				if (nRetVal != XN_STATUS_OK)														\
				{																					\
					KeyTranslator::FreeValue(_key);													\
					ValueTranslator::FreeValue(_value);												\
					return (nRetVal);																\
				}																					\
			}																						\
			return XN_STATUS_OK;																	\
		}																							\
		XnStatus Get(KeyType const& key, ValueType& value) const									\
		{																							\
			XnKey _key = KeyTranslator::GetAsValue(key);											\
			XnValue _value;																			\
			XnStatus nRetVal = XnHash::Get(_key, _value);											\
			if (nRetVal != XN_STATUS_OK) return (nRetVal);											\
			value = ValueTranslator::GetFromValue(_value);											\
			return XN_STATUS_OK;																	\
		}																							\
		XnStatus Get(KeyType const& key, ValueType*& pValue) const									\
		{																							\
			XnKey _key = KeyTranslator::GetAsValue(key);											\
			XnValue _value;																			\
			XnStatus nRetVal = XnHash::Get(_key, _value);											\
			if (nRetVal != XN_STATUS_OK) return (nRetVal);											\
			pValue = &ValueTranslator::GetFromValue(_value);										\
			return XN_STATUS_OK;																	\
		}																							\
		XnStatus Remove(KeyType const& key)															\
		{																							\
			ValueType dummy;																		\
			return Remove(key, dummy);																\
		}																							\
		XnStatus Remove(KeyType const& key, ValueType& value)										\
		{																							\
			ConstIterator it = end();																\
			XnStatus nRetVal = Find(key, it);														\
			if (nRetVal != XN_STATUS_OK) return (nRetVal);											\
			value = it.Value();																		\
			return Remove(it);																		\
		}																							\
		inline XnStatus Remove(ConstIterator iter)													\
		{																							\
			XnKey key = KeyTranslator::GetAsValue(iter.Key());										\
			XnValue value = ValueTranslator::GetAsValue(iter.Value());								\
			XnStatus nRetVal = XnHash::Remove(iter);												\
			if (nRetVal != XN_STATUS_OK) return (nRetVal);											\
			KeyTranslator::FreeValue(key);															\
			ValueTranslator::FreeValue(value);														\
			return XN_STATUS_OK;																	\
		}																							\
		XnStatus Find(KeyType const& key, ConstIterator& hiter) const								\
		{																							\
			XnKey _key = KeyTranslator::GetAsValue(key);											\
			XnHash::ConstIterator it = XnHash::end();												\
			XnStatus nRetVal = XnHash::Find(_key, it);												\
			if (nRetVal != XN_STATUS_OK) return (nRetVal);											\
			hiter = it;																				\
			return XN_STATUS_OK;																	\
		}																							\
		XnStatus Find(KeyType const& key, Iterator& hiter)											\
		{																							\
			XnKey _key = KeyTranslator::GetAsValue(key);											\
			XnHash::Iterator it = XnHash::end();													\
			XnStatus nRetVal = XnHash::Find(_key, it);												\
			if (nRetVal != XN_STATUS_OK) return (nRetVal);											\
			hiter = it;																				\
			return XN_STATUS_OK;																	\
		}																							\
		inline Iterator begin() { return XnHash::begin(); }											\
		inline ConstIterator begin() const { return XnHash::begin(); }								\
		inline Iterator end() { return XnHash::end(); }												\
		inline ConstIterator end() const { return XnHash::end(); }									\
	protected:																						\
		virtual XnStatus Remove(XnHash::ConstIterator iter)											\
		{																							\
			return Remove(ConstIterator(iter));														\
		}																							\
		inline static XnHashValue Hash(const XnKey& key)											\
		{																							\
			KeyType const& _key = KeyTranslator::GetFromValue(key);									\
			return KeyManager::Hash(_key);															\
		}																							\
		inline static XnInt32 Compare(const XnKey& key1, const XnKey& key2)						\
		{																							\
			KeyType const _key1 = KeyTranslator::GetFromValue(key1);								\
			KeyType const _key2 = KeyTranslator::GetFromValue(key2);								\
			return KeyManager::Compare(_key1, _key2);												\
		}																							\
	};

/**
* Declares a hash table from @a KeyType to @a ValueType named @a ClassName. The hash table uses
* @a KeyTranslator and @a ValueTranslator for storing keys and values, and @a KeyManager for managing the keys.
*/
#define XN_DECLARE_HASH(KeyType, ValueType, ClassName, KeyTranslator, ValueTranslator, KeyManager)	\
	XN_DECLARE_HASH_DECL(, KeyType, ValueType, ClassName, KeyTranslator, ValueTranslator, KeyManager)

#define _XN_DEFAULT_KEY_MANAGER_NAME(ClassName)	_##ClassName##Manager

/**
* Declares a hash table from @a KeyType to @a ValueType named @a ClassName. The hash table uses
* @a KeyTranslator and @a ValueTranslator for storing keys and values. It declares and uses a default manager.
* It is declared using the @a decl declspec.
*/
#define XN_DECLARE_DEFAULT_MANAGER_HASH_DECL(decl, KeyType, ValueType, ClassName, KeyTranslator, ValueTranslator)	\
	XN_DECLARE_DEFAULT_KEY_MANAGER_DECL(decl, KeyType, _XN_DEFAULT_KEY_MANAGER_NAME(ClassName), KeyTranslator)		\
	XN_DECLARE_HASH_DECL(decl, KeyType, ValueType, ClassName, KeyTranslator, ValueTranslator, _XN_DEFAULT_KEY_MANAGER_NAME(ClassName))

/**
* Declares a hash table from @a KeyType to @a ValueType named @a ClassName. The hash table uses
* @a KeyTranslator and @a ValueTranslator for storing keys and values. It declares and uses a default manager.
*/
#define XN_DECLARE_DEFAULT_MANAGER_HASH(decl, KeyType, ValueType, ClassName, KeyTranslator, ValueTranslator)	\
	XN_DECLARE_DEFAULT_MANAGER_HASH_DECL(, KeyType, ValueType, ClassName, KeyTranslator, ValueTranslator)

#define _XN_DEFAULT_KEY_TRANSLATOR(ClassName)	_##ClassName##KeyTranslator
#define _XN_DEFAULT_VALUE_TRANSLATOR(ClassName)	_##ClassName##ValueTranslator

/**
* Declares a hash table from @a KeyType to @a ValueType named @a ClassName. The hash table uses
* default translators for storing keys and values. It declares and uses a default manager.
* It is declared using the @a decl declspec.
*/
#define XN_DECLARE_DEFAULT_HASH_DECL(decl, KeyType, ValueType, ClassName)											\
	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR_DECL(decl, KeyType, _XN_DEFAULT_KEY_TRANSLATOR(ClassName))					\
	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR_DECL(decl, ValueType, _XN_DEFAULT_VALUE_TRANSLATOR(ClassName))				\
	XN_DECLARE_DEFAULT_MANAGER_HASH_DECL(decl, KeyType, ValueType, ClassName, _XN_DEFAULT_KEY_TRANSLATOR(ClassName), _XN_DEFAULT_VALUE_TRANSLATOR(ClassName))

/**
* Declares a hash table from @a KeyType to @a ValueType named @a ClassName. The hash table uses
* default translators for storing keys and values. It declares and uses a default manager.
*/
#define XN_DECLARE_DEFAULT_HASH(KeyType, ValueType, ClassName)		\
	XN_DECLARE_DEFAULT_HASH_DECL(, KeyType, ValueType, ClassName)

#endif // _XN_HASH_H
