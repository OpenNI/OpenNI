using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.UInt32;

namespace OpenNI
{
    public class SceneAnalyzer : MapGenerator
    {
		internal SceneAnalyzer(Context context, IntPtr nodeHandle, bool addRef)
			: base(context, nodeHandle, addRef)
        {

        }

        public SceneAnalyzer(Context context, Query query, EnumerationErrors errors) :
			this(context, Create(context, query, errors), false)
        {
        }
        public SceneAnalyzer(Context context, Query query)
            : this(context, query, null)
        {
        }
        public SceneAnalyzer(Context context)
            : this(context, null, null)
        {
        }

        private static IntPtr Create(Context context, Query query, EnumerationErrors errors)
        {
            IntPtr handle;
            int status =
                SafeNativeMethods.xnCreateSceneAnalyzer(context.InternalObject,
                                                        out handle,
                                                        query == null ? IntPtr.Zero : query.InternalObject,
                                                        errors == null ? IntPtr.Zero : errors.InternalObject);
            WrapperUtils.ThrowOnError(status);
            return handle;
        }

		public Plane3D Floor
        {
			get
			{
				Plane3D plane = new Plane3D();
				int status = SafeNativeMethods.xnGetFloor(this.InternalObject, ref plane);
				WrapperUtils.ThrowOnError(status);
				return plane;
			}
        }

        public IntPtr LabelMapPtr
        {
			get
			{
				return SafeNativeMethods.xnGetLabelMap(this.InternalObject);
			}
        }

        public UInt16MapData GetLabelMap()
        {
			MapOutputMode mode = this.MapOutputMode;
			return new UInt16MapData(mode.XRes, mode.YRes, LabelMapPtr);
        }

        // GetMetaData


    }
}
