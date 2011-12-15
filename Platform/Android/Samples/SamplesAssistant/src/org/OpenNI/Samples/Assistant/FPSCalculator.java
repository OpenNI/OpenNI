package org.OpenNI.Samples.Assistant;

import java.util.*;

public class FPSCalculator
{
	private long last_ts;
	private long delta;
	private long[] cyc;
	private int p;
    private List<Long> all;
    
    public FPSCalculator()
    {
    	init(System.currentTimeMillis());
    }
    
    public void init(long ts)
    {
    	last_ts = ts;
    	all = new ArrayList<Long>(1024);
    	cyc = new long[15]; p=0;
    }
    
    public void update(long ts)
    {
    	delta = ts - last_ts;
    	last_ts = ts;
    	cyc[p] = delta; p = (p+1) % cyc.length;
    	
    	all.add(delta);
    }
    
    public void update()
    {
    	update(System.currentTimeMillis());
    }
    
    public double calcAvgFPS()
    {
    	return (1000/calcAvgMillis());
    }
    
	public double calcAvgMillis() {
		double avg = 0; int i;
    	for (i = 0; i < Math.min(cyc.length, all.size()); i++) {
			avg += cyc[i];
		}avg /= i;
		
		return avg;
	}
	
	public double summaryAvgFPS()
    {
    	return (1000/summaryAvgMillis());
    }
    
	public double summaryAvgMillis() {
		double avg = 0;
    	for (Long l : all) {
			avg += l;
		}avg /= all.size();
		
		return avg;
	}
	
	public String toString()
	{
		return String.format("Summary of FPSCalculator:\nAvg fps: %.1f\nAvg deltaT: %.1f\n", 
				summaryAvgFPS(),
				summaryAvgMillis());
	}
}  
