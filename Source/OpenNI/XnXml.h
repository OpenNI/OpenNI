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

XnStatus xnXmlReadUInt32Attribute(const TiXmlElement* pElem, const XnChar* strName, XnUInt32* pnValue);

XnStatus xnXmlReadUInt16Attribute(const TiXmlElement* pElem, const XnChar* strName, XnUInt16* pnValue);

XnStatus xnXmlReadUInt8Attribute(const TiXmlElement* pElem, const XnChar* strName, XnUInt8* pnValue);

XnStatus xnXmlReadRealAttribute(const TiXmlElement* pElem, const XnChar* strName, XnDouble* pdValue);

XnStatus xnXmlReadTextAsInt(const TiXmlElement* pElem, XnInt* pnValue);


#endif // __XN_XML_H__