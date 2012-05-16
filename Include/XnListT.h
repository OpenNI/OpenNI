#ifndef _XNLISTT_H_
#define _XNLISTT_H_ 

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnPlatform.h>
#include <XnDataTypes.h>
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------

/** 
 * A node in a linked list.
 *
 * @tparam	T	the type of value in the list.
 */
template<class T>
struct XnLinkedNodeT
{
	XnLinkedNodeT() : pPrev(NULL), pNext(NULL) {}
	XnLinkedNodeT(T const& value) : pPrev(NULL), pNext(NULL), value(value) {}

	struct XnLinkedNodeT<T>* pPrev;
	struct XnLinkedNodeT<T>* pNext;
	T value;
};

/**
 * A default allocator for nodes in the linked list. The default allocator calls 'new' for allocating
 * new nodes and 'delete' for deallocating them.
 *
 * For information on how to use allocator, see @ref XnListT.
 *
 * @tparam	T	the type of value in the list.
 */
template<class T>
class XnLinkedNodeDefaultAllocatorT
{
public:
	typedef XnLinkedNodeT<T> LinkedNode;

	static LinkedNode* Allocate(T const& value)
	{
		return XN_NEW(LinkedNode, value);
	}

	static void Deallocate(LinkedNode* pNode)
	{
		XN_DELETE(pNode);
	}
};

/**
 * A linked list.
 *
 * @tparam	T		The type of value in the list
 * @tparam	TAlloc	[Optional] A class for allocating and deallocating nodes in the list.
 *					The allocator must have two static methods: Allocate() and Deallocate().
 */
template<class T, class TAlloc = XnLinkedNodeDefaultAllocatorT<T> >
class XnListT
{
public:
	typedef XnLinkedNodeT<T> LinkedNode;
	typedef T TValue;
	typedef TAlloc TAllocator;

	/**
	* An iterator for iterating the list without modifying values.
	*/
	class ConstIterator
	{
	public:
		inline ConstIterator() : m_pCurrent(NULL) {}

		inline ConstIterator(LinkedNode* pNode) : m_pCurrent(pNode) {}

		inline ConstIterator(const ConstIterator& other) : m_pCurrent(other.m_pCurrent) {}

		/**
		* Support ++iterator, go to the next object in the list
		*/
		inline ConstIterator& operator++()
		{
			m_pCurrent = m_pCurrent->pNext;
			return *this;
		}

		/**
		* Support iterator++, go to the next object in the list, returning the old value
		*/
		inline ConstIterator operator++(int)
		{
			ConstIterator retVal(*this);
			++*this;
			return retVal;
		}

		/**
		* Support --iterator, go to the next object in the list
		*/
		inline ConstIterator& operator--()
		{
			m_pCurrent = m_pCurrent->pPrev;
			return *this;
		}

		/**
		* Support iterator--, go to the next object in the list, returning the old value
		*/
		inline ConstIterator operator--(int)
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
			return m_pCurrent == other.m_pCurrent;
		}

		/**
		* Operator to check if two iterators point to different objects
		* 
		* @param	other	[in]	instance to compare with
		*/
		inline XnBool operator!=(const ConstIterator& other) const
		{
			return m_pCurrent != other.m_pCurrent;
		}

		/**
		* Get the value of the current object (const version)
		*/
		inline T const& operator*() const
		{
			return m_pCurrent->value;
		}

		/**
		* Get a pointer to the value of the current object (const version)
		*/
		inline T const* operator->() const
		{
			return &m_pCurrent->value;
		}

	protected:
		friend class XnListT;

		/** The current XnNode */
		LinkedNode* m_pCurrent;
	};

	/**
	* An iterator for iterating the list
	*/
	class Iterator : public ConstIterator
	{
	public:
		inline Iterator() : ConstIterator() {}

		inline Iterator(LinkedNode* pNode) : ConstIterator(pNode) {}

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
		inline T& operator*() const 
		{
			return this->m_pCurrent->value;
		}

		/**
		* Get a pointer to the value of the current object
		*/
		inline T* operator->() const
		{
			return &this->m_pCurrent->value;
		}
	};

public:
	XnListT()
	{
		Init();
	}

	XnListT(const XnListT& other)
	{
		Init();
		*this = other;
	}

	XnListT& operator=(const XnListT& other)
	{
		Clear();

		XnStatus nRetVal = XN_STATUS_OK;

		for (ConstIterator it = other.Begin(); it != other.End(); ++it)
		{
			nRetVal = AddLast(*it);
			XN_ASSERT(nRetVal == XN_STATUS_OK);
		}

		return *this;
	}

	~XnListT()
	{
		Clear();
	}

	/**
	* An iterator to the first entry of the list (non-const version)
	*/
	Iterator Begin()
	{
		return Iterator(m_anchor.pNext);
	}

	/**
	* An iterator to the first entry of the list (const version)
	*/
	ConstIterator Begin() const
	{
		return ConstIterator(const_cast<LinkedNode*>(m_anchor.pNext));
	}

	/**
	* An iterator 1to the end of the list (non-const version). This position is invalid.
	*/
	Iterator End()
	{
		return Iterator(&m_anchor);
	}

	/**
	* An iterator to the end of the list (const version). This position is invalid.
	*/
	ConstIterator End() const
	{
		return ConstIterator(const_cast<LinkedNode*>(&m_anchor));
	}

	/**
	* An iterator to the last entry of the list (non-const version)
	*/
	Iterator ReverseBegin()
	{
		return Iterator(m_anchor.pPrev);
	}

	/**
	* An iterator to the last entry of the list (const version)
	*/
	ConstIterator ReverseBegin() const
	{
		return ConstIterator(const_cast<LinkedNode*>(m_anchor.pPrev));
	}

	/**
	* An iterator to the beginning of the list (non-const version). This position is invalid.
	*/
	Iterator ReverseEnd()
	{
		return Iterator(&m_anchor);
	}

	/**
	* An iterator to the beginning of the list (const version). This position is invalid.
	*/
	ConstIterator ReverseEnd() const
	{
		return ConstIterator(const_cast<LinkedNode*>(&m_anchor));
	}

	/**
	* Add a new value after the object pointed to by the iterator
	* 
	* @param	where	[in]	iterator to the position after which to add the new value
	* @param	value	[in]	The value to add to the list
	*
	* @return	XN_STATUS_ALLOC_FAILED		failed to allocate new node
	*			XN_STATUS_ILLEGAL_POSITION	iterator is invalid
	*/
	XnStatus AddAfter(ConstIterator where, T const& value)
	{
		if (where == End())
		{
			return XN_STATUS_ILLEGAL_POSITION;
		}

		return InsertAfter(where.m_pCurrent, value);
	}

	/**
	* Add a new value before the object pointed to by the iterator
	* 
	* @param	where	[in]	iterator to the position before which to add the new value
	* @param	value		[in]	The value to add to the list
	*
	* @return	XN_STATUS_ALLOC_FAILED		failed to allocate new node
	*			XN_STATUS_ILLEGAL_POSITION	iterator is invalid
	*/
	XnStatus AddBefore(ConstIterator where, T const& value)
	{
		if (where == End())
		{
			return XN_STATUS_ILLEGAL_POSITION;
		}

		return InsertAfter(where.m_pCurrent->pPrev, value);
	}

	/**
	* Add a new value at the beginning of list
	* 
	* @param	value	[in]	The value to add to the head of the list
	*
	* @return	XN_STATUS_ALLOC_FAILED		failed to allocate new node
	*/
	XnStatus AddFirst(T const& value)
	{
		return InsertAfter(&m_anchor, value);
	}

	/**
	* Add a new value at the end of the list
	* 
	* @param	value	[in]	The value to add to the tail of the list
	*
	* @return	XN_STATUS_ALLOC_FAILED		failed to allocate new node
	*/
	XnStatus AddLast(T const& value)
	{
		return InsertAfter(ReverseBegin().m_pCurrent, value);
	}

	/**
	* Get an iterator pointing to a value in the list.
	* 
	* @param	value	[in]	The searched value 
	*
	* @return	End()	if value doesn't exist
	*/
	ConstIterator Find(T const& value) const
	{
		ConstIterator iter = Begin();
		for (; iter != End(); ++iter)
		{
			if (*iter == value)
				break;
		}
		return iter;
	}

	/**
	* Get an iterator pointing to a value in the list.
	* 
	* @param	value	[in]	The searched value 
	*
	* @return	End()	if value doesn't exist
	*/
	Iterator Find(T const& value)
	{
		ConstIterator iter = const_cast<const XnListT<T>*>(this)->Find(value);
		return Iterator(iter.m_pCurrent);
	}

	/**
	* Remove a value from the list
	* 
	* @param	where	[in]	Iterator pointing to an entry in the list
	*
	* @return XN_STATUS_ILLEGAL_POSITION	iterator was invalid
	*/
	XnStatus Remove(ConstIterator where)
	{
		// Verify iterator is valid
		if (where == End())
		{
			return XN_STATUS_ILLEGAL_POSITION;
		}

		XnLinkedNodeT<T>* pToRemove = where.m_pCurrent;

		// Connect other nodes to bypass the one removed
		pToRemove->pPrev->pNext = pToRemove->pNext;
		pToRemove->pNext->pPrev = pToRemove->pPrev;

		--m_nSize;

		// Free memory
		TAlloc::Deallocate(pToRemove);

		return XN_STATUS_OK;
	}

	/**
	* Removes the first occurrence of a value from the list
	* 
	* @param	value	[in]	The value to be removed
	*
	* @return XN_STATUS_NO_MATCH	value wasn't found.
	*/
	XnStatus Remove(T const& value)
	{
		ConstIterator it = Find(value);
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
	* Remove all entries from the list
	*/
	XnStatus Clear()
	{
		while (!IsEmpty())
			Remove(Begin());

		return XN_STATUS_OK;
	}

	/**
	* Check if list is empty
	*/
	XnBool IsEmpty() const
	{
		return (m_nSize == 0);
	}

	/**
	* Gets the current size of the list
	*/
	XnUInt32 Size() const
	{
		return m_nSize;
	}

	/**
	* Copies all values in the list to an array.
	*
	* @param	pArray	A pre-allocated array that values should be copied to. The allocation size can be 
						determined using @ref Size().
	*/
	void CopyTo(T* pArray) const
	{
		XN_ASSERT(pArray != NULL);

		XnUInt32 i = 0;
		for (ConstIterator iter = Begin(); iter != End(); ++iter, ++i)
		{
			pArray[i] = *iter;
		}
	}
	
protected:
	/**
	* Add a new value to the list
	* 
	* @param	pAfter	[in]	The node after which to add the new value
	* @param	val		[in]	The value to add to the list
	*
	* @return	XN_STATUS_ALLOC_FAILED		Failed to add to the list because no nodes are available,
	*/
	XnStatus InsertAfter(LinkedNode* pAfter, T const& val)
	{
		// Get a node from the pool for the entry
		LinkedNode* pNewNode = TAlloc::Allocate(val);
		if (pNewNode == NULL)
		{
			XN_ASSERT(FALSE);
			return XN_STATUS_ALLOC_FAILED;
		}
		pNewNode->pPrev = pAfter;
		pNewNode->pNext = pAfter->pNext;

		// push new node to position
		pAfter->pNext->pPrev = pNewNode;
		pAfter->pNext = pNewNode;

		++m_nSize;

		return XN_STATUS_OK;
	}

	// A dummy node, pointing to first node, and last node points back to it.
	LinkedNode m_anchor;

	XnUInt32 m_nSize;

private:
	void Init()
	{
		m_anchor.pNext = &m_anchor;
		m_anchor.pPrev = &m_anchor;
		m_nSize = 0;
	}
};

#endif // _XNLISTT_H_