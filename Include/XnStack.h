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
#ifndef _XN_STACK_H
#define _XN_STACK_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnList.h"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/**
* The stack
*/
class XnStack
{
public:
	/**
	* Constructor. Initialize internal representations
	*/
	XnStack() {}
	/**
	* Destructor. Destroy internal representations
	*/
	~XnStack() {}

	/**
	* Push a new value to the stack
	* 
	* @param	value	[in]	The value to add to the stack
	*
	* @return	XN_STATUS_ALLOC_FAILED	Failed to add to the stack because no nodes are available.
	*/
	XnStatus Push(XnValue const& value)
	{
		return m_List.AddFirst(value);
	}

	/**
	* Pop the value at the top of the stack
	* 
	* @param	value	[out]	The value that was at the top of the stack
	*
	* @return	XN_STATUS_IS_EMPTY	The stack is empty
	*/
	XnStatus Pop(XnValue& value)
	{
		if (IsEmpty())
		{
			return XN_STATUS_IS_EMPTY;
		}

		value = *(m_List.begin());
		return m_List.Remove(m_List.begin());
	}

	/**
	* Get the value at the top of the queue (it is user responsibility to check queue is not empty)
	* 
	* @return	a reference to the object at head of the queue.
	*/
	XnValue const& Top() const
	{
		return *(m_List.begin());
	}

	/**
	* Get the value at the top of the queue (it is user responsibility to check queue is not empty)
	* 
	* @return	a reference to the object at head of the queue.
	*/
	XnValue& Top()
	{
		return *(m_List.begin());
	}

	/**
	* Check if stack is empty
	*/
	XnBool IsEmpty() const
	{
		return m_List.IsEmpty();
	}

	/**
	* Get current size of the stack
	*/
	XnUInt32 Size() const
	{
		return m_List.Size();
	}

private:
	XN_DISABLE_COPY_AND_ASSIGN(XnStack);

	/** The internal XnList with which the stack is implemented. */
	XnList m_List;
};

/**
* Declares a stack of type @a Type, named @a ClassName. The class uses @a Translator for translating
* from @a Type to XnValue. It is declared using the declspec @a decl.
*/ 
#define XN_DECLARE_STACK_WITH_TRANSLATOR_DECL(decl, Type, ClassName, Translator)		\
	/* Note: we use queue declaration, as this is the same interface. */				\
	XN_DECLARE_QUEUE_WITH_TRANSLATOR_DECL(decl, Type, ClassName, Translator, XnStack)

/**
* Declares a stack of type @a Type, named @a ClassName. The class uses @a Translator for translating
* from @a Type to XnValue.
*/ 
#define XN_DECLARE_STACK_WITH_TRANSLATOR(Type, ClassName, Translator)	\
	XN_DECLARE_STACK_WITH_TRANSLATOR_DECL(, ClassName, Translator)

/**
* Declares a stack of type @a Type, named @a ClassName, that uses the default translator.
* It is declared using the declspec @a decl.
*/ 
#define XN_DECLARE_STACK_DECL(decl, Type, ClassName)													\
	XN_DECLARE_DEFAULT_VALUE_TRANSLATOR_DECL(decl, Type, XN_DEFAULT_TRANSLATOR_NAME(ClassName))			\
	XN_DECLARE_STACK_WITH_TRANSLATOR_DECL(decl, Type, ClassName, XN_DEFAULT_TRANSLATOR_NAME(ClassName))

/**
* Declares a stack of type @a Type, named @a ClassName, that uses the default translator.
*/ 
#define XN_DECLARE_STACK(Type, ClassName)			\
	XN_DECLARE_STACK_DECL(, Type, ClassName)


#endif // _XN_STACK_H
