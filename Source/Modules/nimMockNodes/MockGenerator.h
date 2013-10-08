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
#ifndef __MOCKGENERATOR_H__
#define __MOCKGENERATOR_H__

#include <XnModuleCppInterface.h>
#include <XnTypes.h>
#include "MockProductionNode.h"

XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_INHERITS_VIA_DOMINANCE_WARNING_ID)

class MockGenerator : 
	public MockProductionNode,
	virtual public xn::ModuleGenerator,
	virtual public xn::ModuleMirrorInterface,
	virtual public xn::ModuleFrameSyncInterface
{
public:
	MockGenerator(xn::Context& context, const XnChar* strName, XnBool bAggregateData = FALSE);
	virtual ~MockGenerator();

	/*ModuleProductionNode*/
	virtual XnBool IsCapabilitySupported(const XnChar* strCapabilityName);
	virtual XnStatus SetIntProperty(const XnChar* strName, XnUInt64 nValue);
	virtual XnStatus SetStringProperty(const XnChar* strName, const XnChar* strValue);
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

	/*ModuleFrameSyncInterface*/
	virtual XnBool CanFrameSyncWith(xn::ProductionNode& other);
	virtual XnStatus FrameSyncWith(xn::ProductionNode& other);
	virtual XnStatus StopFrameSyncWith(xn::ProductionNode& other);
	virtual XnBool IsFrameSyncedWith(xn::ProductionNode& other);
	virtual XnStatus RegisterToFrameSyncChange(XnModuleStateChangedHandler handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual void UnregisterFromFrameSyncChange(XnCallbackHandle hCallback);

protected:
	XnStatus OnStateReady();
	XnStatus ResizeBuffer(XnUInt32 nIndex, XnUInt32 nNeededSize);
	XnStatus SetNewDataAvailable();

	virtual XnUInt32 GetRequiredBufferSize();

private:
	XnStatus SetNextData(const void *pData, XnUInt32 nSize);
	XnStatus AppendToNextData(const void *pData, XnUInt32 nSize);
	void SetGenerating(XnBool bGenerating);
	XnStatus SetFrameSyncNode(const XnChar* strOther);
	void OnNodeCreation(xn::ProductionNode& createdNode);
	void OnNodeDestruction(const XnChar* strDestroyedNodeName);
	void FrameSyncChanged();

	static void XN_CALLBACK_TYPE OnNodeCreationCallback(xn::Context& context, xn::ProductionNode& createdNode, void* pCookie);
	static void XN_CALLBACK_TYPE OnNodeDestructionCallback(xn::Context& context, const XnChar* strDestroyedNodeName, void* pCookie);

	XnBool m_bAggregateData;
	PropChangeEvent m_generatingChangedEvent;
	PropChangeEvent m_newDataAvailableEvent;
	PropChangeEvent m_mirrorChangeEvent;
	PropChangeEvent m_frameSyncChangeEvent;

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
	XnBool m_bFrameSyncCap;
	XnChar m_strFrameSyncWith[XN_MAX_NAME_LENGTH]; // We store frame sync by name, as it might not be part of recording
	XnBool m_bFrameSyncWithExists;
	XnCallbackHandle m_hNodeCreationCallback;
	XnCallbackHandle m_hNodeDestructionCallback;
};

XN_PRAGMA_STOP_DISABLED_WARNING_SECTION

#endif // __MOCKGENERATOR_H__
