package org.OpenNI.Samples.Assistant;

import org.OpenNI.Context;
import org.OpenNI.ImageMap;
import org.OpenNI.MapOutputMode;
import org.OpenNI.OutArg;
import org.OpenNI.StatusException;
import org.OpenNI.WrapperUtils;

public class BitmapGenerator {

	public BitmapGenerator(Context context, boolean hasUserGen, boolean hasDepthGen) throws StatusException {
		int status = NativeMethods.initFromContext(context.toNative(), hasUserGen, hasDepthGen);
		WrapperUtils.throwOnError(status);
		
		MapOutputMode mom = getMapOutputMode();
		tempMaps = new MyMap[] { 
		new MyMap(0, mom.getXRes(), mom.getYRes()),
		new MyMap(0, mom.getXRes(), mom.getYRes())
		};
		writeIndex = 0;
	}
	
	public void dispose() throws StatusException {
		int status = NativeMethods.dispose();
		WrapperUtils.throwOnError(status);
	}
	
	public MapOutputMode getMapOutputMode() throws StatusException
	{
		OutArg<Integer> xRes = new OutArg<Integer>();
		OutArg<Integer> yRes = new OutArg<Integer>();
		OutArg<Integer> FPS = new OutArg<Integer>();
		int status = NativeMethods.getMapOutputMode(xRes, yRes, FPS);
		WrapperUtils.throwOnError(status);
		return new MapOutputMode(xRes.value, yRes.value, FPS.value);
	}
	
	
	public long[] generateBitmap(boolean useScene, boolean useDepth, boolean useHistogram) throws StatusException
	{
		MyMap curMap = tempMaps[writeIndex];
//		MapOutputMode mom = getMapOutputMode();
//		curMap.resetRes(mom.getXRes(), mom.getYRes());
		
		long t0 = System.currentTimeMillis();
		
		// Generate the Bitmap
		OutArg<Long> outPtr = new OutArg<Long>();
		int status = NativeMethods.generateBitmapLocalBuffer(useScene, useDepth, useHistogram, outPtr);
		WrapperUtils.throwOnError(status);
		
		long t1 = System.currentTimeMillis();
		
		// Copy the Bitmap's data to the JVM
		curMap.fetchPixels();		
		long t2 = System.currentTimeMillis();
		
		
		// Refresh the current bitmap, and notify possible waiters
		swapMaps();
		
		//returns duration of the whole generating+fetching process, in millisecs.
		return new long[]{t1-t0, t2-t1};
	}
	
	public long[] generateBitmapDirect(boolean useScene, boolean useDepth, boolean useHistogram) throws StatusException
	{
		MyMap curMap = tempMaps[writeIndex];
//		MapOutputMode mom = getMapOutputMode();
//		curMap.resetRes(mom.getXRes(), mom.getYRes());
		
		long t0 = System.currentTimeMillis();
		
		int status = NativeMethods.generateBitmapJavaBuffer(useScene, useDepth, useHistogram, curMap.pixels);
		WrapperUtils.throwOnError(status);
		
		swapMaps();
		return new long[]{System.currentTimeMillis()-t0, 0};
	}
	
	private void swapMaps()
	{
		// Refresh the current bitmap, and notify possible waiters
		MyMap curMap = tempMaps[writeIndex];
		synchronized (swapLock) {
			hasFreshBitmap = true;
			lastGeneratedBitmap = curMap;
			writeIndex++; writeIndex %= 2;
			swapLock.notify();
		}
	}
	
	public long[] generateBitmap() throws StatusException
	{
		return generateBitmap(true, true, true);
	}
	
	public int[] getLastBitmap()
	{
		synchronized (swapLock) {
			
			while(!hasFreshBitmap)
			{
				try {
					swapLock.wait();
				} catch (InterruptedException e) {
					return null;
				}
			}
			
			return lastGeneratedBitmap.pixels;
		}
	}
	
	private MyMap lastGeneratedBitmap;
	private MyMap[] tempMaps;
	private int writeIndex;
	
	private boolean hasFreshBitmap = false;
	private Object swapLock = new Object();
	
	
	class MyMap extends ImageMap
	{
		public MyMap(long ptr, int xRes, int yRes)
		{
			this.xRes = xRes;
			this.yRes = yRes;
			pixels = new int[xRes * yRes];
			nativePtr = ptr;
		}
		protected MyMap(int xRes, int yRes)
		{
			this.xRes = xRes;
			this.yRes = yRes;
			pixels = new int[xRes * yRes];
			nativePtr = 0;
		}
		
		public void resetRes(int xRes, int yRes)
		{
			this.xRes = xRes;
			this.yRes = yRes;
		}
		
		public void fetchPixels()
		{
			NativeMethods.readLocalBitmap2JavaBuffer(nativePtr, pixels);
		}
		
		public int[] pixels;
		protected int xRes, yRes;
		private long nativePtr;
	}
	class MyMap2 extends MyMap
	{
		public MyMap2(int xRes, int yRes)
		{
			super(xRes, yRes);
			
		}
		public void fetchPixels() {} //irrelevant
	}
}
