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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnLicensing.h"
#include "XnInternalTypes.h"
#include "xnInternalFuncs.h"
#include "XnXml.h"

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_XML_LICENSES_ROOT	"Licenses"
#define XN_XML_LICENSE_NODE		"License"
#define XN_XML_LICENSE_VENDOR	"vendor"
#define XN_XML_LICENSE_KEY		"key"

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
class XnLicenseXml : public XnLicense
{
public:
	XnLicenseXml()
	{
		strVendor[0] = '\0';
		strKey[0] = '\0';
	}

	XnLicenseXml(XnLicense* pLicense)
	{
		strcpy(this->strVendor, pLicense->strVendor);
		strcpy(this->strKey, pLicense->strKey);
	}

	TiXmlElement ToElement() const
	{
		TiXmlElement element(XN_XML_LICENSE_NODE);
		element.SetAttribute(XN_XML_LICENSE_VENDOR, strVendor);
		element.SetAttribute(XN_XML_LICENSE_KEY, strKey);
		return element;
	}

	XnStatus FromElement(const TiXmlElement* pElement)
	{
		XnStatus nRetVal = XN_STATUS_OK;
		
		const XnChar* strVendor;
		nRetVal = xnXmlReadStringAttribute(pElement, XN_XML_LICENSE_VENDOR, &strVendor);
		XN_IS_STATUS_OK(nRetVal);

		const XnChar* strKey;
		nRetVal = xnXmlReadStringAttribute(pElement, XN_XML_LICENSE_KEY, &strKey);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnOSStrNCopy(this->strVendor, strVendor, xnOSStrLen(strVendor) + 1, sizeof(this->strVendor));
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = xnOSStrNCopy(this->strKey, strKey, xnOSStrLen(strKey) + 1, sizeof(this->strKey));
		XN_IS_STATUS_OK(nRetVal);
		
		return (XN_STATUS_OK);
	}
};

class XnLicensesXml : public XnListT<XnLicenseXml>
{
public:
	TiXmlElement ToElement() const
	{
		TiXmlElement element(XN_XML_LICENSES_ROOT);
		for (ConstIterator it = Begin(); it != End(); ++it)
		{
			const XnLicenseXml& license = *it;
			element.InsertEndChild(license.ToElement());
		}
		return element;
	}

	XnStatus FromElement(const TiXmlElement* pLicenses)
	{
		XnStatus nRetVal = XN_STATUS_OK;

		Clear();
		
		// read licenses
		const TiXmlElement* pLicense = pLicenses->FirstChildElement(XN_XML_LICENSE_NODE);
		while (pLicense != NULL)
		{
			XnLicenseXml license;
			nRetVal = license.FromElement(pLicense);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = AddLast(license);
			XN_IS_STATUS_OK(nRetVal);

			pLicense = pLicense->NextSiblingElement(XN_XML_LICENSE_NODE);
		}

		return (XN_STATUS_OK);
	}
};

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XnStatus resolveLicensesFile(XnChar* strFileName, XnUInt32 nBufSize)
{
	XnStatus nRetVal = XN_STATUS_OK;

	nRetVal = xnGetOpenNIConfFilesPath(strFileName, nBufSize);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnOSStrAppend(strFileName, "licenses.xml", nBufSize);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus loadLicensesFile(TiXmlDocument& doc)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnChar strFileName[XN_FILE_MAX_PATH];

	nRetVal = resolveLicensesFile(strFileName, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	XnBool bDoesExist = FALSE;
	nRetVal = xnOSDoesFileExist(strFileName, &bDoesExist);
	XN_IS_STATUS_OK(nRetVal);

	if (bDoesExist)
	{
		nRetVal = xnXmlLoadDocument(doc, strFileName);
		XN_IS_STATUS_OK(nRetVal);
	}
	else
	{
		TiXmlElement root(XN_XML_LICENSES_ROOT);
		doc.InsertEndChild(root);
		doc.SaveFile(strFileName);
	}
	
	return (XN_STATUS_OK);
}

XnStatus loadLicensesFile(XnLicensesXml& licenses)
{
	XnStatus nRetVal = XN_STATUS_OK;

	TiXmlDocument doc;
	nRetVal = loadLicensesFile(doc);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = licenses.FromElement(doc.RootElement());
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XnStatus saveLicensesFile(const XnLicensesXml& licenses)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnChar strFileName[XN_FILE_MAX_PATH];

	nRetVal = resolveLicensesFile(strFileName, XN_FILE_MAX_PATH);
	XN_IS_STATUS_OK(nRetVal);

	TiXmlDocument doc;
	doc.InsertEndChild(licenses.ToElement());

	if (!doc.SaveFile(strFileName))
	{
		return XN_STATUS_OS_FILE_WRITE_FAILED;
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnRegisterGlobalLicense(XnLicense* pLicense)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnLicensesXml licenses;
	nRetVal = loadLicensesFile(licenses);
	XN_IS_STATUS_OK(nRetVal);

	// check if it's already there
	XnBool bFound = FALSE;
	for (XnLicensesXml::ConstIterator it = licenses.Begin(); it != licenses.End(); ++it)
	{
		const XnLicenseXml& license = *it;
		if (strcmp(license.strVendor, pLicense->strVendor) == 0 && strcmp(license.strKey, pLicense->strKey) == 0)
		{
			bFound = TRUE;
			break;
		}
	}

	if (!bFound)
	{
		XnLicenseXml license(pLicense);
		nRetVal = licenses.AddLast(license);
		XN_IS_STATUS_OK(nRetVal);

		nRetVal = saveLicensesFile(licenses);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnUnregisterGlobalLicense(XnLicense* pLicense)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	XnLicensesXml licenses;
	nRetVal = loadLicensesFile(licenses);
	XN_IS_STATUS_OK(nRetVal);

	// check if it's there
	XnBool bFound = FALSE;
	for (XnLicensesXml::ConstIterator it = licenses.Begin(); it != licenses.End(); ++it)
	{
		const XnLicenseXml& license = *it;
		if (strcmp(license.strVendor, pLicense->strVendor) == 0 && strcmp(license.strKey, pLicense->strKey) == 0)
		{
			nRetVal = licenses.Remove(it);
			XN_IS_STATUS_OK(nRetVal);

			nRetVal = saveLicensesFile(licenses);
			XN_IS_STATUS_OK(nRetVal);

			bFound = TRUE;
			break;
		}
	}

	if (!bFound)
	{
		return (XN_STATUS_NO_MATCH);
	}
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnPrintRegisteredLicenses()
{
	XnStatus nRetVal = XN_STATUS_OK;

	XnLicensesXml licenses;
	nRetVal = loadLicensesFile(licenses);
	XN_IS_STATUS_OK(nRetVal);

	printf("%-20s%-20s\n", "VENDOR", "KEY");
	printf("%-20s%-20s\n", "======", "===");

	for (XnLicensesXml::ConstIterator it = licenses.Begin(); it != licenses.End(); ++it)
	{
		const XnLicenseXml& license = *it;
		printf("%-20s%-20s\n", license.strVendor, license.strKey);
	}

	return (XN_STATUS_OK);
}

XnStatus xnLoadLicensesFromElement(XnContext* pContext, TiXmlElement* pElem)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	// read licenses
	XnLicensesXml licenses;
	nRetVal = licenses.FromElement(pElem);
	XN_IS_STATUS_OK(nRetVal);

	// now add
	for (XnLicensesXml::ConstIterator it = licenses.Begin(); it != licenses.End(); ++it)
	{
		const XnLicenseXml& license = *it;
		nRetVal = xnAddLicense(pContext, &license);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus xnLoadLicensesFromXml(XnContext* pContext, TiXmlElement* pRootElem)
{
	XnStatus nRetVal = XN_STATUS_OK;

	TiXmlElement* pLicenses = pRootElem->FirstChildElement(XN_XML_LICENSES_ROOT);
	if (pLicenses != NULL)
	{
		nRetVal = xnLoadLicensesFromElement(pContext, pLicenses);
		XN_IS_STATUS_OK(nRetVal);
	}

	return (XN_STATUS_OK);
}

XnStatus xnLoadGlobalLicenses(XnContext* pContext)
{
	XnStatus nRetVal = XN_STATUS_OK;
	
	TiXmlDocument doc;
	nRetVal = loadLicensesFile(doc);
	XN_IS_STATUS_OK(nRetVal);

	nRetVal = xnLoadLicensesFromElement(pContext, doc.RootElement());
	XN_IS_STATUS_OK(nRetVal);
	
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnAddLicense(XnContext* pContext, const XnLicense* pLicense)
{
	XnStatus nRetVal = XN_STATUS_OK;

	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_INPUT_PTR(pLicense);

	nRetVal = pContext->licenses.AddLast(*pLicense);
	XN_IS_STATUS_OK(nRetVal);

	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnEnumerateLicenses(XnContext* pContext, XnLicense** paLicenses, XnUInt32* pnCount)
{
	XN_VALIDATE_INPUT_PTR(pContext);
	XN_VALIDATE_OUTPUT_PTR(paLicenses);
	XN_VALIDATE_OUTPUT_PTR(pnCount);

	*paLicenses = NULL;
	*pnCount = 0;

	XnUInt32 nCount = pContext->licenses.Size();

	// allocate list
	XnLicense* pList;
	XN_VALIDATE_CALLOC(pList, XnLicense, nCount);

	// copy data
	XnUInt32 i = 0;
	for (XnLicenseList::ConstIterator it = pContext->licenses.Begin(); it != pContext->licenses.End(); ++it, ++i)
	{
		pList[i] = *it;
	}

	// return to caller
	*paLicenses = pList;
	*pnCount = nCount;

	return (XN_STATUS_OK);
}

XN_C_API void xnFreeLicensesList(XnLicense* aLicenses)
{
	xnOSFree(aLicenses);
}
