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