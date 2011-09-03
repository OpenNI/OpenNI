package org.OpenNI;

public class Log 
{
	public enum Severity
	{
		VERBOSE (0),
		INFO (1),
		WARNING (2),
		ERROR (3);
		
		Severity(int val)
		{
			this.val = val;
		}
		
		public int getValue() { return this.val; }
		
		private int val;
	}
	
	
}
