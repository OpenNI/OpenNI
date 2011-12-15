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
#ifndef _XN_LIST_H
#define _XN_LIST_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDataTypes.h>
#include <IXnNodeAllocator.h>
#include <XnNodeAllocator.h>
#include <XnNode.h>
#include <XnStatusCodes.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
 * The linked list
 */
class XnList
{
public:
	class ConstIterator
	{
	public:
		friend class XnList;

		/**
		* Copy constructor
		* 
		* @param	other	[in]	instance to copy from
		*/
		ConstIterator(const ConstIterator& other) : m_pCurrent(other.m_pCurrent) {}

		/**
		* Support ++iterator, go to the next object in the list
		*/
		ConstIterator& operator++()
		{
			m_pCurrent = m_pCurrent->Next();
			return *this;
		}

		/**
		* Support iterator++, go to the next object in the list, returning the old value
		*/
		ConstIterator operator++(int)
		{
			ConstIterator other(m_pCurrent);
			m_pCurrent = m_pCurrent->Next();
			return other;
		}

		/**
		* Support --iterator, go to the next object in the list
		*/
		ConstIterator& operator--()
		{
			m_pCurrent = m_pCurrent->Previous();
			return *this;
		}

		/**
		* Support iterator--, go to the next object in the list, returning the old value
		*/
		ConstIterator operator--(int)
		{
			ConstIterator other = *this;
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
			return m_pCurrent == other.m_pCurrent;
		}
		/**
		* Operator to check if 2 iterators point to different objects
		* 
		* @param	other	[in]	instance to compare with
		*/
		XnBool operator!=(const ConstIterator& other) const
		{
			return m_pCurrent != other.m_pCurrent;
		}

		/**
		* Get the value of the current object (const version)
		*/
		const XnValue& operator*() const
		{
			return m_pCurrent->Data();
		}


		/**
		* Get the entire current object (const version)
		*/
		const XnNode* GetNode() const
		{
			return m_pCurrent;
		}

		/**
		* Get the entire current object (non-const version)
		*/
		XnNode* GetNode()
		{
			return m_pCurrent;
		}

	protected:
		/**
		* constructor to be used from inside the XnList. It points to the node supplied.
		* 
		* @param	pNode	[in]	The XnNode to which to currently point
		*/
		ConstIterator(XnNode* pNode) : m_pCurrent(pNode) {}

		/** The current XnNode */
		XnNode* m_pCurrent;
	};

	/**
 	 * Iterator to the XnList
	 */
	class Iterator : public ConstIterator
	{
	public:
		friend class XnList;

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
		* Get the value of the current object
		*/
		inline XnValue& operator*() const { return ((XnValue&)**(ConstIterator*)this); }

	protected:
		/**
		* constructor to be used from inside the XnList. It points to the node supplied.
		* 
		* @param	pNode	[in]	The XnNode to which to currently point
		*/
		inline Iterator(XnNode* pNode) : ConstIterator(pNode) {}
	};

public:
	/**
	* Constructor. Initialize internal representations
	*/
	XnList()
	{
		//Default node allocator is XnNodeAllocator
		Init(XN_NEW(XnNodeAllocator));
		m_bOwnsAllocator = TRUE;
	}

	/**
	* Destructor. Destroy internal representations
	*/
	virtual ~XnList()
	{
		Clear();

		// Return base node to the pool
		m_pNodeAllocator->Deallocate(m_pBase);

		if (m_bOwnsAllocator)
		{
			//We created the allocator in this object, so we must release it
			XN_DELETE(m_pNodeAllocator);
		}
	}

	/**
	* Add a new value at the beginning of list
	* 
	* @param	value	[in]	The value to add to the head of the list
	*
	* @return	XN_STATUS_ALLOC_FAILED	Failed to add to the list because no nodes are available.
	*/
	XnStatus AddFirst(const XnValue& value)
	{
		return Add(m_pBase, value);
	}

	/**
	* Add a new value at the end of the list
	* 
	* @param	value	[in]	The value to add to the tail of the list
	*
	* @return	XN_STATUS_ALLOC_FAILED	Failed to add to the list because no nodes are available.
	*/
	XnStatus AddLast(const XnValue& value)
	{
		return Add(rbegin().m_pCurrent, value);
	}

	/**
	* Add a new value after the object pointed to by the iterator
	* 
	* @param	where	[in]	iterator to the position after which to add the new value
	* @param	val		[in]	The value to add to the list
	*
	* @return	XN_STATUS_ALLOC_FAILED		Failed to add to the list because no nodes are available,
	*			XN_STATUS_ILLEGAL_POSITION	iterator is invalid
	*/
	XnStatus AddAfter(ConstIterator where, const XnValue& val)
	{
		if (where == end())
		{
			return XN_STATUS_ILLEGAL_POSITION;
		}

		return Add(where.m_pCurrent, val);
	}

	/**
	* Add a new value before the object pointed to by the iterator
	* 
	* @param	where	[in]	iterator to the position before which to add the new value
	* @param	val		[in]	The value to add to the list
	*
	* @return	XN_STATUS_ALLOC_FAILED		Failed to add to the list because no nodes are available,
	*/
	XnStatus AddBefore(ConstIterator where, const XnValue& val)
	{
		if (where == end())
		{
			return XN_STATUS_ILLEGAL_POSITION;
		}

		return Add(where.m_pCurrent->Previous(), val);
	}


	/**
	* Get an iterator pointing to a value in the list.
	* 
	* @param	value	[in]	The searched value 
	*
	* @return	end()	if value doesn't exist
	*/
	Iterator Find(const XnValue& value)
	{
		if (IsEmpty())
		{
			return end();
		}

		Iterator iter = begin();
		for (; iter != end(); ++iter)
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
	* @return	end()	if value doesn't exist
	*/
	ConstIterator Find(const XnValue& value) const
	{
		if (IsEmpty())
		{
			return end();
		}

		ConstIterator iter = begin();
		for (; iter != end(); ++iter)
		{
			if (*iter == value)
				break;
		}
		return iter;
	}


	/**
	* Remove a value from the list
	* 
	* @param	where	[in]	Iterator pointing to an entry in the list
	* @param	value	[out]	The value that was in the removed entry
	*
	* @return XN_STATUS_ILLEGAL_POSITION	iterator was invalid
	*/
	XnStatus Remove(ConstIterator where, XnValue& value)
	{
		value = *where;
		return Remove(where);
	}

	/**
	* Remove a value from the list
	* 
	* @param	where	[in]	Iterator pointing to an entry in the list
	*
	* @return XN_STATUS_ILLEGAL_POSITION	iterator was invalid
	*/
	virtual XnStatus Remove(ConstIterator where)
	{
		// Verify iterator is valid
		if (where == end())
		{
			return XN_STATUS_ILLEGAL_POSITION;
		}
		if (IsEmpty())
		{
			return XN_STATUS_IS_EMPTY;
		}

		XnNode* pToRemove = where.m_pCurrent;

		// Connect other nodes to bypass the one removed
		pToRemove->Previous()->Next() = pToRemove->Next();
		pToRemove->Next()->Previous() = pToRemove->Previous();

		// Return removed node to the pool
		m_pNodeAllocator->Deallocate(pToRemove);

		return XN_STATUS_OK;
	}


	/**
	* Remove all entries from the list
	*/
	XnStatus Clear()
	{
		while (!IsEmpty())
			Remove(begin());

		return XN_STATUS_OK;
	}

	/**
	* Check if list is empty
	*/
	XnBool IsEmpty() const
	{
		return (begin() == end());
	}

	/**
	* Current size of the list
	*/
	XnUInt32 Size() const
	{
		XnUInt32 nSize = 0;
		for (ConstIterator iter = begin(); iter != end(); ++iter, ++nSize)
			;

		return nSize;
	}

	/**
	* An iterator to the first entry of the list (non-const version)
	*/
	Iterator begin()
	{
		return Iterator(m_pBase->Next());
	}

	/**
	* An iterator to the first entry of the list (const version)
	*/
	ConstIterator begin() const
	{
		return ConstIterator(m_pBase->Next());
	}

	/**
	* An iterator 1to the end of the list (non-const version). The position is invalid.
	*/
	Iterator end()
	{
		return Iterator(m_pBase);
	}

	/**
	* An iterator to the end of the list (const version). The position is invalid.
	*/
	ConstIterator end() const
	{
		return ConstIterator(m_pBase);
	}

	/**
	* An iterator to the last entry of the list (non-const version)
	*/
	Iterator rbegin()
	{
		return Iterator(m_pBase->Previous());
	}

	/**
	* An iterator to the last entry of the list (const version)
	*/
	ConstIterator rbegin() const
	{
		return ConstIterator(m_pBase->Previous());
	}

	/**
	* An iterator to the beginning of the list (non-const version). This position is invalid
	*/
	Iterator rend()
	{
		return Iterator(m_pBase);
	}

	/**
	* An iterator to the beginning of the list (const version). This position is invalid
	*/
	ConstIterator rend() const
	{
		return ConstIterator(m_pBase);
	}
	
protected:
	friend class XnNodeManager;
	
	/**
	* Constructor. Initialize internal representations
	*/
	XnList(INiNodeAllocator* pNodeAllocator)
	{
		Init(pNodeAllocator);
		m_bOwnsAllocator = FALSE;
	}
	
	void Init(INiNodeAllocator* pNodeAllocator)
	{
		m_pNodeAllocator = pNodeAllocator;
		// Allocate a node to act as base node.
		m_pBase = m_pNodeAllocator->Allocate();
		if (m_pBase == NULL)
		{
			// OZOZ: Allocation failed in ctor...
		}

		m_pBase->Next() = m_pBase->Previous() = m_pBase;
	}

	/**
	* Add a new value to the list
	* 
	* @param	pWhere	[in]	The XnNode after which to add the new value
	* @param	val		[in]	The value to add to the list
	*
	* @return	XN_STATUS_ALLOC_FAILED		Failed to add to the list because no nodes are available,
	*/
	XnStatus Add(XnNode* pWhere, const XnValue& val)
	{
		// Get a node from the pool for the entry
		XnNode* pNewNode = m_pNodeAllocator->Allocate();
		if (pNewNode == NULL)
		{
			return XN_STATUS_ALLOC_FAILED;
		}
		// push new node to position
		pNewNode->Data() = val;
		pNewNode->Next() = pWhere->Next();
		pNewNode->Previous() = pWhere;
		pWhere->Next()->Previous() = pNewNode;
		pWhere->Next() = pNewNode;

		return XN_STATUS_OK;
	}


	/** The base node for the list */
	XnNode* m_pBase;
	
	INiNodeAllocator* m_pNodeAllocator;
	XnBool m_bOwnsAllocator;
};

/**
* Declares a list of type @a Type, which is named @a ClassName. The list uses translator @a Translator,
* and is declared using the @a decl declspec.
*/
#define XN_DECLARE_LIST_WITH_TRANSLATOR_DECL(decl, Type, ClassName, Translator)					\
	class decl ClassName : public XnList														\
	{																							\
	public:																						\
		class decl ConstIterator : public XnList::ConstIterator									\
		{																						\
		public:																					\
			friend class ClassName;																\
			inline ConstIterator(const ConstIterator& other) : XnList::ConstIterator(other) {}	\
			inline ConstIterator& operator++()													\
			{																					\
				++(*(XnList::ConstIterator*)this);												\
				return (*this);																	\
			}																					\
			inline ConstIterator operator++(int)												\
			{																					\
				ConstIterator result = *this;													\
				++*this;																		\
				return result;																	\
			}																					\
			inline ConstIterator& operator--()													\
			{																					\
				--(*(XnList::ConstIterator*)this);												\
				return (*this);																	\
			}																					\
			inline ConstIterator operator--(int)												\
			{																					\
				ConstIterator result = *this;													\
				--*this;																		\
				return result;																	\
			}																					\
			inline Type const& operator*() const												\
			{																					\
				return Translator::GetFromValue(**((XnList::ConstIterator*)this));				\
			}																					\
			inline Type const* operator->() const { return (&**this); }							\
		protected:																				\
			inline ConstIterator(XnNode* pNode) : XnList::ConstIterator(pNode) {}				\
			inline ConstIterator(const XnList::ConstIterator& other) :							\
				XnList::ConstIterator(other)  													\
			{}																					\
		};																						\
		class decl Iterator : public ConstIterator												\
		{																						\
		public:																					\
			friend class ClassName;																\
			Iterator(const Iterator& other) : ConstIterator(other) {}							\
			inline Iterator& operator++()														\
			{																					\
				++(*(ConstIterator*)this);														\
				return (*this);																	\
			}																					\
			inline Iterator operator++(int)														\
			{																					\
				Iterator result = *this;														\
				++*this;																		\
				return result;																	\
			}																					\
			inline Iterator& operator--()														\
			{																					\
				--(*(ConstIterator*)this);														\
				return (*this);																	\
			}																					\
			inline Iterator operator--(int)														\
			{																					\
				Iterator result = *this;														\
				--*this;																		\
				return result;																	\
			}																					\
			inline Type& operator*() const { return ((Type&)**(ConstIterator*)this); }			\
			inline Type* operator->() const { return (&**this); }								\
		protected:																				\
			inline Iterator(XnNode* pNode) : ConstIterator(pNode) {}							\
			inline Iterator(const XnList::Iterator& other) : ConstIterator(other) {}			\
		};																						\
	public:																						\
		ClassName()																				\
		{																						\
		}																						\
		ClassName(const ClassName& other)														\
		{																						\
			*this = other;																		\
		}																						\
		~ClassName()																			\
		{																						\
			while (!IsEmpty())																	\
				Remove(begin());																\
		}																						\
		ClassName& operator=(const ClassName& other)											\
		{																						\
			Clear();																			\
			for (ConstIterator it = other.begin(); it != other.end(); ++it)						\
			{																					\
				AddLast(*it);																	\
			}																					\
			return *this;																		\
		}																						\
		inline XnStatus AddFirst(Type const& value)												\
		{																						\
			XnValue val = Translator::CreateValueCopy(value);									\
			XnStatus nRetVal = XnList::AddFirst(val);											\
			if (nRetVal != XN_STATUS_OK)														\
			{																					\
				Translator::FreeValue(val);														\
				return (nRetVal);																\
			}																					\
			return XN_STATUS_OK;																\
		}																						\
		inline XnStatus AddLast(Type const& value)												\
		{																						\
			XnValue val = Translator::CreateValueCopy(value);									\
			XnStatus nRetVal = XnList::AddLast(val);											\
			if (nRetVal != XN_STATUS_OK)														\
			{																					\
				Translator::FreeValue(val);														\
				return (nRetVal);																\
			}																					\
			return XN_STATUS_OK;																\
		}																						\
		inline XnStatus AddAfter(ConstIterator where, Type const& value)						\
		{																						\
			XnValue val = Translator::CreateValueCopy(value);									\
			XnStatus nRetVal = XnList::AddAfter(where, val);									\
			if (nRetVal != XN_STATUS_OK)														\
			{																					\
				Translator::FreeValue(val);														\
				return (nRetVal);																\
			}																					\
			return XN_STATUS_OK;																\
		}																						\
		inline XnStatus AddBefore(ConstIterator where, Type const& value)						\
		{																						\
			XnValue val = Translator::CreateValueCopy(value);									\
			XnStatus nRetVal = XnList::AddBefore(where, val);									\
			if (nRetVal != XN_STATUS_OK)														\
			{																					\
				Translator::FreeValue(val);														\
				return (nRetVal);																\
			}																					\
			return XN_STATUS_OK;																\
		}																						\
		inline ConstIterator Find(Type const& value) const										\
		{																						\
			XnValue _value = Translator::GetAsValue(value);										\
			return XnList::Find(_value);														\
		}																						\
		inline Iterator Find(Type const& value)													\
		{																						\
			XnValue _value = Translator::GetAsValue(value);										\
			return XnList::Find(_value);														\
		}																						\
		inline XnStatus Remove(ConstIterator where)												\
		{																						\
			XnValue val = Translator::GetAsValue(*where);										\
			XnStatus nRetVal = XnList::Remove(where);											\
			if (nRetVal != XN_STATUS_OK) return (nRetVal);										\
			Translator::FreeValue(val);															\
			return XN_STATUS_OK;																\
		}																						\
		inline XnStatus Remove(Type const& value)												\
		{																						\
			Iterator it = Find(value);															\
			return Remove(it);																	\
		}																						\
		inline Iterator begin() { return XnList::begin(); }										\
		inline ConstIterator begin() const { return XnList::begin(); }							\
		inline Iterator end() { return XnList::end(); }											\
		inline ConstIterator end() const { return XnList::end(); }								\
		inline Iterator rbegin() { return XnList::rbegin(); }									\
		inline ConstIterator rbegin() const { return XnList::rbegin(); }						\
		inline Iterator rend() { return XnList::rend(); }										\
		inline ConstIterator rend() const { return XnList::rend(); }							\
	protected:																					\
		virtual XnStatus Remove(XnList::ConstIterator where)									\
		{																						\
			return Remove(ConstIterator(where));												\
		}																						\
	};

/**
* Declares a list of type @a Type, which is named @a ClassName. The list uses translator @a Translator.
*/
#define XN_DECLARE_LIST_WITH_TRANSLATOR(Type, ClassName, Translator)							\
	XN_DECLARE_LIST_WITH_TRANSLATOR_DECL(, Type, ClassName, Translator)

/**
* Declares a list of type @a Type, which is named @a ClassName. The list uses creates a default translator
* and is declared using the @a decl declspec.
*/
#define XN_DECLARE_LIST_DECL(decl, Type, ClassName)														\
	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR_DECL(decl, Type, XN_DEFAULT_TRANSLATOR_NAME(ClassName))			\
	XN_DECLARE_LIST_WITH_TRANSLATOR_DECL(decl, Type, ClassName, XN_DEFAULT_TRANSLATOR_NAME(ClassName))

/**
* Declares a list of type @a Type, which is named @a ClassName. The list uses creates a default translator.
*/
#define XN_DECLARE_LIST(Type, ClassName)		\
	XN_DECLARE_LIST_DECL(, Type, ClassName)
																							
#endif // _XN_LIST_H																		
																						
