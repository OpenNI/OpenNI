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
#include "TrackingInitializer.h"

TrackingInitializer::TrackingInitializer(xn::UserGenerator *pUserGenerato) : 
                                                       m_pUserSelector(NULL),
                                                       m_pUserGenerator(pUserGenerato),
                                                       m_bValid(FALSE)
{
    if(m_pUserGenerator==NULL)
        return; // nothing to work with so we stay invalid.
    m_bValid=TRUE;
}

TrackingInitializer::~TrackingInitializer()
{

}

XnStatus TrackingInitializer::SetUserTracker(UserSelector *pUserSelector)
{
    if(m_pUserSelector!=NULL)
        return XN_STATUS_ERROR; // we already have a user selector
    m_pUserSelector=pUserSelector;
    return XN_STATUS_OK;
}

