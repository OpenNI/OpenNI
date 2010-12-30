using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public class DepthGenerator : MapGenerator
	{
		internal DepthGenerator(NodeSafeHandle nodeHandle, bool addRef) :
			base(nodeHandle, addRef)
		{
			this.fovChanged = new StateChangedEvent(this,
				OpenNIImporter.xnRegisterToDepthFieldOfViewChange,
				OpenNIImporter.xnUnregisterFromDepthFieldOfViewChange);
		}

		public DepthGenerator(Context context, Query query, EnumerationErrors errors) :
			this(Create(context, query, errors), false)
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

		public IntPtr GetDepthMapPtr()
		{
			return OpenNIImporter.xnGetDepthMap(this.InternalObject);
		}

		public MapData<UInt16> GetDepthMap()
		{
			return GetMapData<UInt16>(GetDepthMapPtr());
		}

		public UInt16 GetDeviceMaxDepth()
		{
			return OpenNIImporter.xnGetDeviceMaxDepth(this.InternalObject);
		}

		public FieldOfView GetFieldOfView()
		{
			FieldOfView fov = new FieldOfView();
            Status.ThrowOnFail(OpenNIImporter.xnGetDepthFieldOfView(this.InternalObject, ref fov));
			
			return fov;
		}

		public event StateChangedHandler FieldOfViewChanged
		{
			add { this.fovChanged.Event += value; }
			remove { this.fovChanged.Event -= value; }
		}

		public Point3D[] ConvertProjectiveToRealWorld(Point3D[] projectivePoints)
		{
			Point3D[] realWorld = new Point3D[projectivePoints.Length];
            Status.ThrowOnFail(OpenNIImporter.xnConvertProjectiveToRealWorld(this.InternalObject, (uint)projectivePoints.Length, projectivePoints, realWorld));
			
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
            Status.ThrowOnFail(OpenNIImporter.xnConvertRealWorldToProjective(this.InternalObject, (uint)realWorldPoints.Length, realWorldPoints, projective));
            
            return projective;
        }
        
        public Point3D ConvertRealWorldToProjective(Point3D realWorldPoint)
		{
            Point3D[] realWorldPoints = new Point3D[1];
            realWorldPoints[0] = realWorldPoint;

            return ConvertRealWorldToProjective(realWorldPoints)[0];
        }

		public UserPositionCapability GetUserPositionCap()
		{
			return new UserPositionCapability(this);
		}

		public void GetMetaData(DepthMetaData depthMD)
		{
			using (IMarshaler marsh = depthMD.GetMarshaler(true))
			{
				OpenNIImporter.xnGetDepthMetaData(this.InternalObject, marsh.Native);
			}
		}

		public DepthMetaData MetaData
		{
            get
            {
                DepthMetaData depthMD = new DepthMetaData();
                GetMetaData(depthMD);
                return depthMD;
            }
		}

		private static NodeSafeHandle Create(Context context, Query query, EnumerationErrors errors)
		{
            NodeSafeHandle handle;
            Status.ThrowOnFail(OpenNIImporter.xnCreateDepthGenerator(context.InternalObject, out handle,
				query == null ? QuerySafeHandle.Zero : query.InternalObject,
                errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
			
			return handle;
		}

		private StateChangedEvent fovChanged;
	}
}