using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;
using System.Text;

namespace OpenNI
{
	public class ErrorCollectionItem
	{
		internal ErrorCollectionItem(IntPtr it)
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

		public Status Error
		{
            get
            {
                return SafeNativeMethods.xnEnumerationErrorsGetCurrentError(this.it);
            }
		}

		private IntPtr it;
	}

	public class ErrorCollection : ObjectWrapper, IEnumerable<ErrorCollectionItem>
	{
		internal ErrorCollection(EnumerationErrorsSafeHandle pErrors) :
			base(pErrors)
		{
		}

		public ErrorCollection() :
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
                IntPtr first = SafeNativeMethods.xnEnumerationErrorsGetFirst(this.InternalObject);
                return !SafeNativeMethods.xnEnumerationErrorsIteratorIsValid(first);
            }
		}

		public override string ToString()
		{
			const int size = 2048;
			StringBuilder sb = new StringBuilder(size);
			Status.ThrowOnFail(SafeNativeMethods.xnEnumerationErrorsToString(this.InternalObject, sb, size));
			
			return sb.ToString();
		}

		#region ErrorCollectionEnumerator class

		private class ErrorCollectionEnumerator : IEnumerator<ErrorCollectionItem>
		{
			public ErrorCollectionEnumerator(ErrorCollection errors)
			{
				this.errors = errors;
				this.reset = true;
				this.it = IntPtr.Zero;
			}

            ~ErrorCollectionEnumerator()
            {
                Dispose(false);
            }

            private void Dispose(bool disposing)
            {
                if (disposing)
                {
				    this.reset = true;
				    this.it = IntPtr.Zero;
                }
            }

            #region IEnumerator<ErrorCollectionItem> Members

			public ErrorCollectionItem Current
			{
				get { return new ErrorCollectionItem(this.it); }
			}

			#endregion

			#region IDisposable Members

			public void Dispose()
			{
                Dispose(true);
                GC.SuppressFinalize(this);
            }

			#endregion

			#region IEnumerator Members

			object System.Collections.IEnumerator.Current
			{
				get { return ((IEnumerator<ErrorCollectionItem>)this).Current; }
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

			private ErrorCollection errors;
			private IntPtr it;
			private bool reset;
		}

		#endregion

		#region IEnumerable<ErrorCollectionItem> Members

		public IEnumerator<ErrorCollectionItem> GetEnumerator()
		{
			return new ErrorCollectionEnumerator(this);
		}

		#endregion

		#region IEnumerable Members

		System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
		{
			return ((IEnumerable<ErrorCollectionItem>)this).GetEnumerator();
		}

		#endregion

        private static EnumerationErrorsSafeHandle Create()
		{
            EnumerationErrorsSafeHandle pErrors;
            Status.ThrowOnFail(SafeNativeMethods.xnEnumerationErrorsAllocate(out pErrors));
			
			return pErrors;
		}
	}
}