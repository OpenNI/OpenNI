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
#ifndef __MOCKGENERATOR_H__
#define __MOCKGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include <XnEvent.h>
#include "MockProductionNode.h"

class MockGenerator : 
	public MockProductionNode,
	virtual public xn::ModuleGenerator,
	virtual public xn::ModuleMirrorInterface
{
public:
	MockGenerator(const XnChar* strName, XnBool bAggregateData = FALSE);
	virtual ~MockGenerator();

	/*ModuleProductionNode*/
	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName);
	virtual XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus SetGeneralProperty(const XnChar* strName, XnUInt32 nBufferSize, const void* pBuffer);

	/*ModuleGenerator*/
	virtual XnStatus StartGenerating();
	virtual XnBool IsGenerating();
	virtual void StopGenerating();
	virtual XnStatus RegisterToGenerationRunningChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromGenerationRunningChange(XnCallbackHandle hCallback);
	virtual XnStatus RegisterToNewDataAvailable(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromNewDataAvailable(XnCallbackHandle hCallback);
	virtual XnBool IsNewDataAvailable(XnUInt64& nTimestamp);
	virtual XnStatus UpdateData();
	virtual const void* GetData();
	virtual XnUInt32 GetDataSize();
	virtual XnUInt64 GetTimestamp();
	virtual XnUInt32 GetFrameID();
	virtual xn::ModuleMirrorInterface* GetMirrorInterface();
	virtual xn::ModuleAlternativeViewPointInterface* GetAlternativeViewPointInterface();
	virtual xn::ModuleFrameSyncInterface* GetFrameSyncInterface();

	/*ModuleMirrorInterface*/
	virtual XnStatus SetMirror(XnBool bMirror);
	virtual XnBool IsMirrored();
	virtual XnStatus RegisterToMirrorChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromMirrorChange(XnCallbackHandle hCallback);

protected:
	XnStatus OnStateReady();
	XnStatus ResizeBuffer(XnUInt32 nIndex, XnUInt32 nNeededSize);
	XnStatus SetNewDataAvailable();

	virtual XnUInt32 GetRequiredBufferSize();

private:
	XnStatus SetNextData(const void *pData, XnUInt32 nSize);
	XnStatus AppendToNextData(const void *pData, XnUInt32 nSize);
	void SetGenerating(XnBool bGenerating);

	XnBool m_bAggregateData;
	PropChangeEvent m_generatingChangedEvent;
	PropChangeEvent m_newDataAvailableEvent;
	PropChangeEvent m_mirrorChangeEvent;

	enum {NUM_BUFFERS = 2};
	struct DataInfo
	{
		void *pData;
		XnUInt32 nAllocatedSize;
		XnUInt32 nDataSize;
		XnUInt64 nTimeStamp;
		XnUInt32 nFrameID;
	} m_data[NUM_BUFFERS];
	/*We save two buffers - one for current data and one for next data. The current data is in 
	  m_data[m_nCurrentDataIdx], and the next data is in m_data[m_nNextDataIdx]. UpdateData just exchanges the
	  two indices, and so the next data becomes the current data. The next data buffer's contents are not 
	  interesting anymore, and will be overwritten by the next call to SetNextData().
	*/

	XnUInt32 m_nCurrentDataIdx;
	XnUInt32 m_nNextDataIdx;

	XnBool m_bGenerating;
	XnBool m_bMirror;
	XnBool m_bNewDataAvailable;
	XnBool m_bMirrorCap;
};

#endif // __MOCKGENERATOR_H__
