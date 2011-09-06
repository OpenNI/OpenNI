package org.OpenNI;

public class Codec extends ProductionNode
{
	Codec(Context context, long nodeHandle, boolean addRef) throws StatusException
	{
		super(context, nodeHandle, addRef);
	}

	public static Codec create(Context context, CodecID codecID, ProductionNode initializer) throws GeneralException
	{
		OutArg<Long> handle = new OutArg<Long>();
		int status = NativeMethods.xnCreateCodec(context.toNative(), codecID.toNative(), initializer.toNative(), handle);
		WrapperUtils.throwOnError(status);
		Codec result = (Codec)context.createProductionNodeObject(handle.value, NodeType.CODEC);
		NativeMethods.xnProductionNodeRelease(handle.value);
		return result;
	}

	public CodecID getCodecID()
	{
		int codecID = NativeMethods.xnGetCodecID(toNative());
		return new CodecID(codecID);
	}
	
	public int EncodeData(long pSrcPtr, int nSrcSize, long pDstPtr, int nDstSize) throws StatusException
	{
		OutArg<Integer> written = new OutArg<Integer>();
		int status = NativeMethods.xnEncodeData(toNative(), pSrcPtr, nSrcSize, pDstPtr, nDstSize, written);
		WrapperUtils.throwOnError(status);
		return written.value;
	}

	public int DecodeData(long pSrcPtr, int nSrcSize, long pDstPtr, int nDstSize) throws StatusException
	{
		OutArg<Integer> written = new OutArg<Integer>();
		int status = NativeMethods.xnDecodeData(toNative(), pSrcPtr, nSrcSize, pDstPtr, nDstSize, written);
		WrapperUtils.throwOnError(status);
		return written.value;
	}
}
