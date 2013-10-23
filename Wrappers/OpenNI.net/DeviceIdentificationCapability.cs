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
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace OpenNI
{
    public class DeviceIdentificationCapability : Capability
    {
        internal DeviceIdentificationCapability(ProductionNode node) :
            base(node)
        {
        }

        public string DeviceName
        {
            get
            {
                uint size = 1024;
                StringBuilder sb = new StringBuilder((int)size);
                int status = SafeNativeMethods.xnGetDeviceName(this.InternalObject, sb, ref size);
                WrapperUtils.ThrowOnError(status);
                return sb.ToString();
            }
        }

        public string VendorSpecificData
        {
            get
            {
                uint size = 1024;
                StringBuilder sb = new StringBuilder((int)size);
                int status = SafeNativeMethods.xnGetVendorSpecificData(this.InternalObject, sb, ref size);
                WrapperUtils.ThrowOnError(status);
                return sb.ToString();
            }
        }

        public string SerialNumber
        {
            get
            {
                uint size = 1024;
                StringBuilder sb = new StringBuilder((int)size);
                int status = SafeNativeMethods.xnGetSerialNumber(this.InternalObject, sb, ref size);
                WrapperUtils.ThrowOnError(status);
                return sb.ToString();
            }
        }
   }
}