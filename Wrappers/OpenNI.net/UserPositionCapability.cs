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
	public class UserPositionCapability : Capability
	{
		internal UserPositionCapability(ProductionNode node) :
			base(node)
		{
			this.userPositionChanged = new StateChangedEvent(node,
				SafeNativeMethods.xnRegisterToUserPositionChange,
				SafeNativeMethods.xnUnregisterFromUserPositionChange);
		}

		public int SupportedPositionsCount
		{
			get
			{
				return (int)SafeNativeMethods.xnGetSupportedUserPositionsCount(this.InternalObject);
			}
		}

		public void SetPosition(int index, BoundingBox3D pos)
		{
			int status = SafeNativeMethods.xnSetUserPosition(this.InternalObject, (uint)index, ref pos);
			WrapperUtils.ThrowOnError(status);
		}

		public BoundingBox3D GetPosition(int index)
		{
			BoundingBox3D pos = new BoundingBox3D();
			int status = SafeNativeMethods.xnGetUserPosition(this.InternalObject, (uint)index, ref pos);
			WrapperUtils.ThrowOnError(status);
			return pos;
		}

		public event EventHandler UserPositionChanged
		{
			add { this.userPositionChanged.Event += value; }
			remove { this.userPositionChanged.Event -= value; }
		}

		private StateChangedEvent userPositionChanged;
	}
}