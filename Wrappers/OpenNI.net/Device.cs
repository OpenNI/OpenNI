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

namespace OpenNI
{
	public class Device : ProductionNode
	{
		internal Device(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
            if (IsCapabilitySupported(Capabilities.DeviceIdentification))
                m_deviceIdentification = new DeviceIdentificationCapability(this);
            else
                m_deviceIdentification = null;
		}

        public Device(Context context, Query query, EnumerationErrors errors) :
			this(context, Create(context, query, errors), false)
		{
		}

		public Device(Context context, Query query) :
			this(context, query, null)
		{
		}

        public Device(Context context) :
			this(context, null, null)
		{
		}

        public DeviceIdentificationCapability DeviceIdentificationCapability
        {
            get { return m_deviceIdentification; }
        }

        public override void Dispose()
        {
            if (m_deviceIdentification != null)
                m_deviceIdentification.InternalDispose();
            m_deviceIdentification = null;
            base.Dispose();
        }

		private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
		{
            IntPtr handle;
            int status = SafeNativeMethods.xnCreateDevice(context.InternalObject, out handle,
                query == null ? IntPtr.Zero : query.InternalObject,
                errors == null ? IntPtr.Zero : errors.InternalObject);
            WrapperUtils.ThrowOnError(status);
            return handle;
        }

        private DeviceIdentificationCapability m_deviceIdentification;
	}
}