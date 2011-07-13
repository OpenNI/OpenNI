package org.OpenNI;

public class EnumerationErrors extends ObjectWrapper
{
	public EnumerationErrors() throws StatusException
	{
		this(create());
	}
	
	public boolean isEmpty()
	{
		long first = NativeMethods.xnEnumerationErrorsGetFirst(toNative());
		return !NativeMethods.xnEnumerationErrorsIteratorIsValid(first);
	}
	
	@Override
	public String toString() 
	{
		OutArg<String> result = new OutArg<String>();
		NativeMethods.xnEnumerationErrorsToString(toNative(), result);
		return result.value;
	}
	
	@Override
	protected void freeObject(long ptr) 
	{
		NativeMethods.xnEnumerationErrorsFree(ptr);
	}

	private EnumerationErrors(long ptr) 
	{
		super(ptr);
	}
	
	private static long create() throws StatusException
	{
		OutArg<Long> pErrors = new OutArg<Long>();
		int status = NativeMethods.xnEnumerationErrorsAllocate(pErrors);
		WrapperUtils.throwOnError(status);
		return pErrors.value;
	}
}
