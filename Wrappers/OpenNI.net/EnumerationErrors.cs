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
	public class EnumerationError
	{
		internal EnumerationError(IntPtr it)
		{
			this.it = it;
		}

		public ProductionNodeDescription Description
		{
			get
			{
				return SafeNativeMethods.xnEnumerationErrorsGetCurrentDescription(this.it);
			}
		}

		public string Error
		{
			get
			{
				int status = SafeNativeMethods.xnEnumerationErrorsGetCurrentError(this.it);
				return WrapperUtils.GetErrorMessage(status);
			}
		}

		private IntPtr it;
	}

	public class EnumerationErrors : ObjectWrapper, IEnumerable<EnumerationError>
	{
		internal EnumerationErrors(IntPtr pErrors) :
			base(pErrors)
		{
		}

		public EnumerationErrors() :
			this(Create())
		{
		}

		public bool IsEmpty()
		{
			IntPtr first = SafeNativeMethods.xnEnumerationErrorsGetFirst(this.InternalObject);
			return !SafeNativeMethods.xnEnumerationErrorsIteratorIsValid(first);
		}

		public override string ToString()
		{
			const int size = 2048;
			StringBuilder sb = new StringBuilder(size);
			int status = SafeNativeMethods.xnEnumerationErrorsToString(this.InternalObject, sb, size);
			WrapperUtils.ThrowOnError(status);
			return sb.ToString();
		}

		#region EnumerationErrosEnumerator class

		private class EnumerationErrosEnumerator : IEnumerator<EnumerationError>
		{
			public EnumerationErrosEnumerator(EnumerationErrors errors)
			{
				this.errors = errors;
				this.reset = true;
				this.it = IntPtr.Zero;
			}

			#region IEnumerator<EnumerationError> Members

			public EnumerationError Current
			{
				get { return new EnumerationError(this.it); }
			}

			#endregion

			#region IDisposable Members

			public void Dispose()
			{
			}

			#endregion

			#region IEnumerator Members

			object System.Collections.IEnumerator.Current
			{
				get { return ((IEnumerator<EnumerationError>)this).Current; }
			}

			public bool MoveNext()
			{
				if (this.reset)
				{
					this.it = SafeNativeMethods.xnEnumerationErrorsGetFirst(this.errors.InternalObject);
					this.reset = false;
				}
				else
				{
					this.it = SafeNativeMethods.xnEnumerationErrorsGetNext(this.it);
				}

				return SafeNativeMethods.xnEnumerationErrorsIteratorIsValid(this.it);
			}

			public void Reset()
			{
				this.reset = true;
			}

			#endregion

			private EnumerationErrors errors;
			private IntPtr it;
			private bool reset;
		}

		#endregion

		#region IEnumerable<EnumerationError> Members

		public IEnumerator<EnumerationError> GetEnumerator()
		{
			return new EnumerationErrosEnumerator(this);
		}

		#endregion

		#region IEnumerable Members

		System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
		{
			return ((IEnumerable<EnumerationError>)this).GetEnumerator();
		}

		#endregion

		protected override void FreeObject(IntPtr ptr, bool disposing)
		{
			SafeNativeMethods.xnEnumerationErrorsFree(ptr);
		}

		private static IntPtr Create()
		{
			IntPtr pErrors;
			int status = SafeNativeMethods.xnEnumerationErrorsAllocate(out pErrors);
			WrapperUtils.ThrowOnError(status);
			return pErrors;
		}
	}
}