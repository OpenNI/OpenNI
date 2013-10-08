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