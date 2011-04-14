using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class DepthGenerator : MapGenerator
	{
		internal DepthGenerator(Context context, IntPtr nodeHandle, bool addRef) :
			base(context, nodeHandle, addRef)
		{
			this.fovChanged = new StateChangedEvent(this,
				SafeNativeMethods.xnRegisterToDepthFieldOfViewChange,
				SafeNativeMethods.xnUnregisterFromDepthFieldOfViewChange);
		}

		public DepthGenerator(Context context, Query query, EnumerationErrors errors) :
			this(context, Create(context, query, errors), false)
		{
		}

		public DepthGenerator(Context context, Query query) :
			this(context, query, null)
		{
		}

		public DepthGenerator(Context context) :
			this(context, null, null)
		{
		}

		public IntPtr DepthMapPtr
		{
			get
			{
				return SafeNativeMethods.xnGetDepthMap(this.InternalObject);
			}
		}

		public UInt16MapData GetDepthMap()
		{
			MapOutputMode mode = this.MapOutputMode;
			return new UInt16MapData(mode.XRes, mode.YRes, this.DepthMapPtr);
		}

		public int DeviceMaxDepth
		{
			get
			{
				return SafeNativeMethods.xnGetDeviceMaxDepth(this.InternalObject);
			}
		}

		public FieldOfView FieldOfView
		{
			get
			{
				FieldOfView fov = new FieldOfView();
				int status = SafeNativeMethods.xnGetDepthFieldOfView(this.InternalObject, ref fov);
				WrapperUtils.ThrowOnError(status);
				return fov;
			}
		}

		public event EventHandler FieldOfViewChanged
		{
			add { this.fovChanged.Event += value; }
			remove { this.fovChanged.Event -= value; }
		}

		public Point3D[] ConvertProjectiveToRealWorld(Point3D[] projectivePoints)
		{
			Point3D[] realWorld = new Point3D[projectivePoints.Length];
			int status = SafeNativeMethods.xnConvertProjectiveToRealWorld(this.InternalObject, (uint)projectivePoints.Length, projectivePoints, realWorld);
			WrapperUtils.ThrowOnError(status);
			return realWorld;
		}

        public Point3D ConvertProjectiveToRealWorld(Point3D projectivePoint)
        {
            Point3D[] projectivePoints = new Point3D[1];
            projectivePoints[0] = projectivePoint;

            return ConvertProjectiveToRealWorld(projectivePoints)[0];
        }

        public Point3D[] ConvertRealWorldToProjective(Point3D[] realWorldPoints)
        {
            Point3D[] projective = new Point3D[realWorldPoints.Length];
            int status = SafeNativeMethods.xnConvertRealWorldToProjective(this.InternalObject, (uint)realWorldPoints.Length, realWorldPoints, projective);
            WrapperUtils.ThrowOnError(status);
            return projective;
        }
        
        public Point3D ConvertRealWorldToProjective(Point3D realWorldPoint)
		{
            Point3D[] realWorldPoints = new Point3D[1];
            realWorldPoints[0] = realWorldPoint;

            return ConvertRealWorldToProjective(realWorldPoints)[0];
        }

		public UserPositionCapability UserPositionCapability
		{
			get
			{
				return new UserPositionCapability(this);
			}
		}

		public void GetMetaData(DepthMetaData depthMD)
		{
			using (IMarshaler marsh = depthMD.GetMarshaler(true))
			{
				SafeNativeMethods.xnGetDepthMetaData(this.InternalObject, marsh.Native);
			}
		}

		public DepthMetaData GetMetaData()
		{
			DepthMetaData depthMD = new DepthMetaData();
			GetMetaData(depthMD);
			return depthMD;
		}

		private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
		{
			IntPtr handle;
			int status = SafeNativeMethods.xnCreateDepthGenerator(context.InternalObject, out handle,
				query == null ? IntPtr.Zero : query.InternalObject,
				errors == null ? IntPtr.Zero : errors.InternalObject);
			WrapperUtils.ThrowOnError(status);
			return handle;
		}

		private StateChangedEvent fovChanged;
	}
}