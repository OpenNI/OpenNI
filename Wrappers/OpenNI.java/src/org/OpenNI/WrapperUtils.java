package org.OpenNI;

public class WrapperUtils
{
	public static void throwOnError(int status) throws StatusException
	{
		if (status != 0)
		{
			throw new StatusException(status);
		}
	}
	
	public static void checkEnumeration(int status, EnumerationErrors errors) throws GeneralException
	{
		if (status != 0)
		{
			if (errors != null && !errors.isEmpty())
			{
				throw new GeneralException(errors.toString());
			}
			else
			{
				throw new StatusException(status);
			}
		}
	}
	
	public static String getErrorMessage(int status)
	{
		String message = NativeMethods.xnGetStatusString(status);
		return message;
	}
}
