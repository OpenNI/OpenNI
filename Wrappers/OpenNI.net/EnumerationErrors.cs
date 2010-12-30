using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
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

		public ProductionNodeDescription GetDescription()
		{
			return OpenNIImporter.xnEnumerationErrorsGetCurrentDescription(this.it);
		}

		public Status GetError()
		{
			return OpenNIImporter.xnEnumerationErrorsGetCurrentError(this.it);
		}

		private IntPtr it;
	}

	public class EnumerationErrors : ObjectWrapper, IEnumerable<EnumerationError>
	{
		internal EnumerationErrors(EnumerationErrorsSafeHandle pErrors) :
			base(pErrors)
		{
		}

		public EnumerationErrors() :
			this(Create())
		{
		}

        internal new EnumerationErrorsSafeHandle InternalObject
        {
            get { return (EnumerationErrorsSafeHandle)base.InternalObject; }
        }

        [Pure]
        public bool IsEmpty
		{
            get
            {
                IntPtr first = OpenNIImporter.xnEnumerationErrorsGetFirst(this.InternalObject);
                return !OpenNIImporter.xnEnumerationErrorsIteratorIsValid(first);
            }
		}

		public override string ToString()
		{
			const int size = 2048;
			StringBuilder sb = new StringBuilder(size);
			Status.ThrowOnFail(OpenNIImporter.xnEnumerationErrorsToString(this.InternalObject, sb, size));
			
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
					this.it = OpenNIImporter.xnEnumerationErrorsGetFirst(this.errors.InternalObject);
					this.reset = false;
				}
				else
				{
					this.it = OpenNIImporter.xnEnumerationErrorsGetNext(this.it);
				}

				return OpenNIImporter.xnEnumerationErrorsIteratorIsValid(this.it);
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

        private static EnumerationErrorsSafeHandle Create()
		{
            EnumerationErrorsSafeHandle pErrors;
            Status.ThrowOnFail(OpenNIImporter.xnEnumerationErrorsAllocate(out pErrors));
			
			return pErrors;
		}
	}
}