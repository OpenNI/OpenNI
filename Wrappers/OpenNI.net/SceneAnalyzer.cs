using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.UInt32;

namespace xn
{
    public class SceneAnalyzer : MapGenerator
    {
		internal SceneAnalyzer(IntPtr nodeHandle, bool addRef)
			: base(nodeHandle, addRef)
        {

        }

        public SceneAnalyzer(Context context, Query query, EnumerationErrors errors) :
            this(Create(context, query, errors), false)
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
            UInt32 status =
                OpenNIImporter.xnCreateSceneAnalyzer(context.InternalObject,
                                                        out handle,
                                                        query == null ? IntPtr.Zero : query.InternalObject,
                                                        errors == null ? IntPtr.Zero : errors.InternalObject);
            WrapperUtils.CheckStatus(status);
            return handle;
        }

        public void GetFloor(ref Plane3D plane)
        {
            UInt32 status = OpenNIImporter.xnGetFloor(this.InternalObject, ref plane);
            WrapperUtils.CheckStatus(status);
        }

        public IntPtr GetLabelMapPtr()
        {
            return OpenNIImporter.xnGetLabelMap(this.InternalObject);
        }

        public MapData<UInt16> GetLabelMap()
        {
            return GetMapData<UInt16>(GetLabelMapPtr());
        }

        // GetMetaData


    }
}
