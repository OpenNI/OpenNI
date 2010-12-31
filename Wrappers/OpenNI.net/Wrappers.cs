using System;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public abstract class HandleWrapper
	{
		internal HandleWrapper(UInt32 handle)
		{
			this.handle = handle;
		}

		/// <summary>
		/// Gets the native (C language) OpenNI handle. This method should only be used for native-managed transitions.
		/// </summary>
		/// <returns>An OpenNI handle</returns>
		public UInt32 ToNative()
		{
			return this.handle;
		}

		internal UInt32 InternalHandle
		{
			get { return this.handle; }
		}

		private UInt32 handle;
	}

    [ContractClass(typeof(Contracts.ObjectWrapperContract))]
	public abstract class ObjectWrapper : IDisposable
	{
        internal ObjectWrapper(SafeHandle handle)
		{
            Contract.Requires(handle != null);
            Contract.Requires(!handle.IsClosed);
            Contract.Ensures(this.handle != null);
            Contract.Ensures(!this.handle.IsClosed);

			this.handle = handle;
		}

		~ObjectWrapper()
		{
			Dispose(false);
		}

        ///// <summary>
        ///// Gets the native (C language) OpenNI object. This method should only be used for native-managed transitions.
        ///// </summary>
        ///// <returns>A pointer to the OpenNI object</returns>
        //public IntPtr ToNative()
        //{
        //    Contract.Ensures(Contract.Result<IntPtr>() != IntPtr.Zero, "Native pointer was disposed.");

        //    return this.handle;
        //}

		#region IDisposable Members

		public void Dispose()
		{
            Contract.Ensures(this.handle.IsClosed);
            
            Dispose(true);
			GC.SuppressFinalize(this);
		}

		#endregion

        internal SafeHandle InternalObject
		{
			get { return this.handle; }
		}

		protected virtual void Dispose(bool disposing)
		{
			if (!this.handle.IsClosed) // if not disposed already
			{
                if (disposing) // no need to dispose managed resources on termination
                {
                    this.handle.Dispose();
                    this.handle = null;
                }
			}
		}

		private SafeHandle handle;
	}

    #region Code contracts

    namespace Contracts
    {
        [ContractClassFor(typeof(ObjectWrapper))]
        internal abstract class ObjectWrapperContract
            : ObjectWrapper
        {
            internal ObjectWrapperContract(SafeHandle handle)
                : base(handle)
            {
            }

        }
    }

    #endregion

	public class LockHandle : HandleWrapper
	{
		internal LockHandle(UInt32 handle)
			: base(handle)
		{
		}

		/// <summary>
		/// Creates a managed LockHandle object to wrap a native one.
		/// </summary>
		/// <param name="handle">The native lock handle</param>
		/// <returns>A managed LockHandle object</returns>
		static public LockHandle FromNative(UInt32 handle)
		{
			return new LockHandle(handle);
		}
	}

	public struct CodecId
	{
		public CodecId(UInt32 internalValue)
		{
			this.val = internalValue;
		}

		public CodecId(byte byte1, byte byte2, byte byte3, byte byte4) :
			this((UInt32)(byte1 << 24 | byte2 << 16 | byte3 << 8 | byte4))
		{
		}

		public CodecId(char char1, char char2, char char3, char char4) :
			this((byte)char1, (byte)char2, (byte)char3, (byte)char4)
		{
		}

		public static readonly CodecId Null = new CodecId(0, 0, 0, 0);
		public static readonly CodecId Uncompressed = new CodecId('N', 'O', 'N', 'E');
		public static readonly CodecId Jpeg = new CodecId('J', 'P', 'E', 'G');
		public static readonly CodecId Z16 = new CodecId('1', '6', 'z', 'P');
		public static readonly CodecId Z16WithTables = new CodecId('1', '6', 'z', 'T');
		public static readonly CodecId Z8 = new CodecId('I', 'm', '8', 'z');

		internal UInt32 InternalValue
		{
			get { return this.val; }
		}

		private UInt32 val;
	}

	public abstract class NodeWrapper : ObjectWrapper, IEquatable<NodeWrapper>
	{
		internal NodeWrapper(NodeSafeHandle hNode, bool addRef)
			: base(hNode)
		{
			if (addRef)
			{
				SafeNativeMethods.xnProductionNodeAddRef(hNode);
			}
		}

        internal new NodeSafeHandle InternalObject
        {
            get { return (NodeSafeHandle)base.InternalObject; }
        }

		public override bool Equals(object obj)
		{
			return Equals(obj as NodeWrapper);
		}

		public bool Equals(NodeWrapper other)
		{
			if (other == null)
				return false;

			return (this.InternalObject == other.InternalObject);
		}

		public override int GetHashCode()
		{
			return this.InternalObject.GetHashCode();
		}

 		/// TRUE if the object points to an actual node, FALSE otherwise.
 		public bool IsValid
		{
			get { return this.InternalObject.IsClosed; }
		}

		public string Name
		{
            get
            {
                return SafeNativeMethods.xnGetNodeName(this.InternalObject);
            }
		}
	};

	public class Capability : NodeWrapper
	{
		public Capability(ProductionNode node) :
			base(node.InternalObject, true)
		{
			this.node = node;
		}

		internal ProductionNode node;
	}

	public interface IMarshaler : IDisposable
	{
		IntPtr Native { get; }
	}

	public class Marshaler<T> : IMarshaler
	{
		public Marshaler(T obj, bool marshalOut, params IMarshaler[] inner)
		{
			this.obj = obj;
			this.marshalOut = marshalOut;
			this.native = this.Allocate();
			// we always have to copy in (so that pointers are set correctly)
			ManagedToNative(obj, this.native);
			this.inner = inner;
		}

		#region IMarshaler Members

		public IntPtr Native
		{
			get { return this.native; }
		}

		#endregion

		#region IDisposable Members

		public void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		#endregion

		protected IntPtr Allocate()
		{
			return Marshal.AllocHGlobal(Marshal.SizeOf(this.obj));
		}

		protected void Free(IntPtr ptr)
		{
			Marshal.FreeHGlobal(ptr);
		}

		protected void ManagedToNative(T source, IntPtr dest)
		{
			Marshal.StructureToPtr(source, dest, false);
		}

		protected void NativeToManaged(IntPtr source, T dest)
		{
			Marshal.PtrToStructure(source, dest);
		}

		private void Dispose(bool disposing)
		{
			if (this.native != IntPtr.Zero)
			{
				if (this.marshalOut)
				{
					NativeToManaged(this.native, this.obj);
				}
				Free(this.native);
				this.native = IntPtr.Zero;

				foreach (IMarshaler marshaler in this.inner)
				{
					marshaler.Dispose();
				}
				this.inner = null;
			}
		}

		private T obj;
		private bool marshalOut;
		private IntPtr native;
		private IMarshaler[] inner;
	}
}
