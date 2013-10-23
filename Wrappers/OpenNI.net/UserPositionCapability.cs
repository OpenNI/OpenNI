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