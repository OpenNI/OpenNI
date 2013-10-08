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
using System.Runtime.Serialization;
using System.Security.Permissions;

namespace OpenNI
{
	public static class WrapperUtils
	{
		public static string GetErrorMessage(Int32 status)
		{
            return Marshal.PtrToStringAnsi(SafeNativeMethods.xnGetStatusString(status));
		}

		public static void ThrowOnError(Int32 status)
		{
			if (status != 0)
			{
				throw new StatusException(status);
			}
		}

		public static void CheckEnumeration(Int32 status, EnumerationErrors errors)
		{
			if (status != 0)
			{
				if (errors != null && !errors.IsEmpty())
				{
					throw new GeneralException(errors.ToString());
				}
				else
				{
					throw new StatusException(status);
				}
			}
		}
	}

	public abstract class HandleWrapper
	{
		internal HandleWrapper(Int32 handle)
		{
			this.handle = handle;
		}

		/// <summary>
		/// Gets the native (C language) OpenNI handle. This method should only be used for native-managed transitions.
		/// </summary>
		/// <returns>An OpenNI handle</returns>
		public Int32 ToNative()
		{
			return this.handle;
		}

		internal Int32 InternalHandle
		{
			get { return this.handle; }
		}

		private Int32 handle;
	}

	public abstract class ObjectWrapper : IDisposable
	{
		internal ObjectWrapper(IntPtr ptr)
		{
			if (ptr == IntPtr.Zero)
			{
				throw new GeneralException("c# wrappers: Trying to wrap a null object!");
			}

			this.ptr = ptr;
		}

		~ObjectWrapper()
		{
			Dispose(false);
		}

		/// <summary>
		/// Gets the native (C language) OpenNI object. This method should only be used for native-managed transitions.
		/// </summary>
		/// <returns>A pointer to the OpenNI object</returns>
		public IntPtr ToNative()
		{
			return this.InternalObject;
		}

		#region IDisposable Members

        public virtual void Dispose()
		{
			Dispose(true);
			GC.SuppressFinalize(this);
		}

		#endregion

		internal IntPtr InternalObject
		{
			get 
			{
				if (this.ptr == IntPtr.Zero)
				{
					throw new ObjectDisposedException(GetType().Name);
				}
				return this.ptr; 
			}
		}

		protected internal void UnsafeReplaceInternalObject(IntPtr ptr)
		{
			this.ptr = ptr;
		}

		protected abstract void FreeObject(IntPtr ptr, bool disposing);

		protected virtual void Dispose(bool disposing)
		{
			if (this.ptr != IntPtr.Zero)
			{
				FreeObject(this.ptr, disposing);
				this.ptr = IntPtr.Zero;
			}
		}

		private IntPtr ptr;
	}

	[Serializable]
	public class GeneralException : System.Exception
	{
		public GeneralException() :
			base()
		{
		}

		public GeneralException(string message) :
			base(message)
		{
		}

		public GeneralException(string message, Exception innerException) :
			base(message, innerException)
		{
		}

		protected GeneralException(SerializationInfo info, StreamingContext context) : 
			base(info, context)
		{
		}
	}

	[Serializable]
	public class StatusException : GeneralException
	{
		public StatusException() :
			this(0)
		{
		}

		public StatusException(Int32 status) :
			base(WrapperUtils.GetErrorMessage(status))
		{
			this.status = status;
		}

		public StatusException(string message) :
			base(message)
		{
		}

		public StatusException(string message, Exception innerException) :
			base(message, innerException)
		{
		}

		protected StatusException(SerializationInfo info, StreamingContext context) : 
			base(info, context)
		{
		}

		public Int32 Status
		{
			get { return status; }
		}

		[SecurityPermission(SecurityAction.Demand, SerializationFormatter = true)]
		public override void GetObjectData(SerializationInfo info, StreamingContext context)
		{
			info.AddValue("Status", status);
			base.GetObjectData(info, context);
		}

		private Int32 status;
	}

	public class LockHandle : HandleWrapper
	{
		internal LockHandle(Int32 handle)
			: base(handle)
		{
		}

		/// <summary>
		/// Creates a managed LockHandle object to wrap a native one.
		/// </summary>
		/// <param name="handle">The native lock handle</param>
		/// <returns>A managed LockHandle object</returns>
		static public LockHandle FromNative(Int32 handle)
		{
			return new LockHandle(handle);
		}
	}

	public struct CodecID
	{
		public CodecID(int internalValue)
		{
			this.val = internalValue;
		}

		public CodecID(byte byte1, byte byte2, byte byte3, byte byte4) :
			this(byte4 << 24 | byte3 << 16 | byte2 << 8 | byte1)
		{
		}

		public CodecID(char char1, char char2, char char3, char char4) :
			this((byte)char1, (byte)char2, (byte)char3, (byte)char4)
		{
		}

		public static readonly CodecID Null = new CodecID(0, 0, 0, 0);
		public static readonly CodecID Uncompressed = new CodecID('N', 'O', 'N', 'E');
		public static readonly CodecID Jpeg = new CodecID('J', 'P', 'E', 'G');
		public static readonly CodecID Z16 = new CodecID('1', '6', 'z', 'P');
		public static readonly CodecID Z16WithTables = new CodecID('1', '6', 'z', 'T');
		public static readonly CodecID Z8 = new CodecID('I', 'm', '8', 'z');

		internal int InternalValue
		{
			get { return this.val; }
		}

		private int val;
	}

	public class NodeWrapper : ObjectWrapper
	{
		internal NodeWrapper(IntPtr hNode, bool addRef)
			: base(hNode)
		{
			this.contextShuttingDownHandler = OnContextShuttingDown;

			if (addRef)
			{
				WrapperUtils.ThrowOnError(SafeNativeMethods.xnProductionNodeAddRef(hNode));
			}

			IntPtr pContext = SafeNativeMethods.xnGetRefContextFromNodeHandle(hNode);
			
			int status = SafeNativeMethods.xnContextRegisterForShutdown(pContext, this.contextShuttingDownHandler, IntPtr.Zero, out this.hShutdownCallback);
			WrapperUtils.ThrowOnError(status);

			SafeNativeMethods.xnContextRelease(pContext);
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
			get { return (this.InternalObject != IntPtr.Zero); }
		}

		public string Name
		{
			get
			{
				return Marshal.PtrToStringAnsi(SafeNativeMethods.xnGetNodeName(this.InternalObject));
			}
		}

		protected override void FreeObject(IntPtr ptr, bool disposing)
		{
			IntPtr pContext = SafeNativeMethods.xnGetRefContextFromNodeHandle(ptr);
			SafeNativeMethods.xnContextUnregisterFromShutdown(pContext, this.hShutdownCallback);
			SafeNativeMethods.xnContextRelease(pContext);
			SafeNativeMethods.xnProductionNodeRelease(ptr);
		}

		private void OnContextShuttingDown(IntPtr pContext, IntPtr pCookie)
		{
			// the context is shutting down
			// no need to unregister from shutting down event - the event is destroyed anyway
			UnsafeReplaceInternalObject(IntPtr.Zero);

			Dispose();
		}

		private SafeNativeMethods.XnContextShuttingDownHandler contextShuttingDownHandler;
		private IntPtr hShutdownCallback;
	};

	public class Capability : NodeWrapper
	{
		public Capability(ProductionNode node) :
			base(node.InternalObject, true)
		{
			this.node = node;
		}

		internal ProductionNode node;

        /// @todo this is a temporary solution for capability not being disposed by anyone external
        public override void Dispose()
        {
            // we do nothing because we basically want to make the public dispose do nothing!

        }

        /// @todo this is a temporary solution for capability not being disposed by anyone external
        internal virtual void InternalDispose()
        {
            base.Dispose();
        }
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

		protected virtual IntPtr Allocate()
		{
			return Marshal.AllocHGlobal(Marshal.SizeOf(this.obj));
		}

		protected virtual void Free(IntPtr ptr)
		{
			Marshal.FreeHGlobal(ptr);
		}

		protected virtual void ManagedToNative(T source, IntPtr dest)
		{
			Marshal.StructureToPtr(source, dest, false);
		}

		protected virtual void NativeToManaged(IntPtr source, T dest)
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
