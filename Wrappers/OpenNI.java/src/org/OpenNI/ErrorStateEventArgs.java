package org.OpenNI;

public class ErrorStateEventArgs extends EventArgs
{
	public ErrorStateEventArgs(int status)
	{
		if (status == 0)
		{
			this.currError = null;
		}
		else
		{
			this.currError = WrapperUtils.getErrorMessage(status);
		}
	}
	
	public String getCurrentError()
	{
		return currError;
	}

	private String currError;
}
