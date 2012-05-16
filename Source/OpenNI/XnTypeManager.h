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
#ifndef __XN_TYPE_MANAGER_H__
#define __XN_TYPE_MANAGER_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnBitSet.h>
#include <XnTypes.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MAX_TYPES_COUNT	500

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class TypeManager
{
public:
	static TypeManager& GetInstance();

	XnStatus RegisterNewType(const XnChar* strName, XnProductionNodeType baseType, XnProductionNodeType* pNewType);

	XnStatus GetTypeName(XnProductionNodeType type, const XnChar** pstrName) const;
	XnStatus GetTypeByName(const XnChar* strName, XnProductionNodeType* pType) const;
	XnStatus IsTypeDerivedFrom(XnProductionNodeType type, XnProductionNodeType base, XnBool* pbIsDerived) const;
	XnStatus GetTypeHierarchy(XnProductionNodeType type, const XnBitSet*& pHierarchy) const;
	XnPredefinedProductionNodeType GetPredefinedBaseType(XnProductionNodeType type);

private:
	TypeManager();
	~TypeManager();

	struct NodeTypeInfo
	{
		NodeTypeInfo() : type(XN_NODE_TYPE_INVALID) 
		{
			this->strName[0] = '\0';
		}

		NodeTypeInfo(const XnChar* strName, XnProductionNodeType type) : type(type)
		{
			strcpy(this->strName, strName);
		}

		XnChar strName[XN_MAX_NAME_LENGTH];
		XnProductionNodeType type;
		XnBitSet inheritanceGraph;
	};

	XnStatus AddType(NodeTypeInfo& info);
	XnStatus AddNewType(const XnChar* strName, XnProductionNodeType type, XnProductionNodeType base);

	static TypeManager m_instance;

	XnProductionNodeType m_nNextExtendedNodeType;
	XN_CRITICAL_SECTION_HANDLE m_hLock;

	// we use a regular array instead of a collection (XnArray, XnList, etc.) because we want thread safety 
	// without locks. We know types are never removed from the array, only added.
	NodeTypeInfo* m_pTypesArray[XN_MAX_TYPES_COUNT];
};

#endif // __XN_TYPE_MANAGER_H__