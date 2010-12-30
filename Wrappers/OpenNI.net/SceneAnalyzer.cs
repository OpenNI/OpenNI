﻿using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using UserID = System.UInt32;

namespace OpenNI
{
    public class SceneAnalyzer : MapGenerator
    {
        internal SceneAnalyzer(NodeSafeHandle nodeHandle, bool addRef)
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

        private static NodeSafeHandle Create(Context context, Query query, EnumerationErrors errors)
        {
            NodeSafeHandle handle;
            Status.ThrowOnFail(OpenNIImporter.xnCreateSceneAnalyzer(context.InternalObject,
                                                        out handle,
                                                        query == null ? QuerySafeHandle.Zero : query.InternalObject,
                                                        errors == null ? EnumerationErrorsSafeHandle.Zero : errors.InternalObject));
            
            return handle;
        }

        public void GetFloor(ref Plane3D plane)
        {
            Status.ThrowOnFail(OpenNIImporter.xnGetFloor(this.InternalObject, ref plane));
            
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
