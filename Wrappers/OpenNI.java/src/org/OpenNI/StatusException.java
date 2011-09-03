package org.OpenNI;

public class StatusException extends GeneralException
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	public StatusException(int status)
	{
		super(WrapperUtils.getErrorMessage(status));
	}
}
