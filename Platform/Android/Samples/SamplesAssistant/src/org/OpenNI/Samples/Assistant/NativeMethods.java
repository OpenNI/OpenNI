package org.OpenNI.Samples.Assistant;

import java.nio.ByteBuffer;

import org.OpenNI.OutArg;

class NativeMethods
{
	static 
	{ 
		String arch = System.getenv("PROCESSOR_ARCHITECTURE");
		if ((arch != null) && ((arch.equals("AMD64")) || (arch.equals("IA64"))))
			System.loadLibrary("OpenNISamplesAssistant.jni64"); 
		else
			System.loadLibrary("OpenNISamplesAssistant.jni");
	}

	// Marshaling
	static native byte readByte(long ptr);
	static native short readShort(long ptr);
	static native int readInt(long ptr);
	static native long readLong(long ptr);
	static native void copyToBuffer(ByteBuffer buffer, long ptr, int size);

	
	
	
	static native int initFromContext(
			long pContext, 
			boolean hasUserGen, 
			boolean hasDepthGen);
	
	static native int dispose();
	
	static native int getMapOutputMode(
			OutArg<Integer> xRes,
			OutArg<Integer> yRes, 
			OutArg<Integer> fps
			);
	
	static native int generateBitmapLocalBuffer(
			boolean useScene, 
			boolean useDepth,
			boolean useHistogram,
			OutArg<Long> outPtr);
	
	static native int generateBitmapJavaBuffer(
			boolean useScene, 
			boolean useDepth,
			boolean useHistogram,
			int[] buffer);
	
	static native int readLocalBitmap2JavaBuffer(
			long nativePtr, 
			int[] javaBuffer
			);
}
