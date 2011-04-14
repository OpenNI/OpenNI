/****************************************************************************
*                                                                           *
*  OpenNI 1.1 Alpha                                                         *
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
#ifndef __XN_XML_H__
#define __XN_XML_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnStatus.h>
#include <tinyxml.h>

//---------------------------------------------------------------------------
// Inline helpers
//---------------------------------------------------------------------------
XnStatus xnXmlLoadDocument(TiXmlDocument& doc, const XnChar* strFileName);

XnStatus xnXmlGetChildElement(const TiXmlElement* pElem, const XnChar* strName, const TiXmlElement** ppChild);

XnStatus xnXmlReadStringAttribute(const TiXmlElement* pElem, const XnChar* strName, const XnChar** pstrValue);

XnStatus xnXmlReadBoolAttribute(const TiXmlElement* pElem, const XnChar* strName, XnBool* pbValue);

XnStatus xnXmlReadIntAttribute(const TiXmlElement* pElem, const XnChar* strName, XnInt* pnValue);

XnStatus xnXmlReadRealAttribute(const TiXmlElement* pElem, const XnChar* strName, XnDouble* pdValue);

XnStatus xnXmlReadTextAsInt(const TiXmlElement* pElem, XnInt* pnValue);


#endif // __XN_XML_H__