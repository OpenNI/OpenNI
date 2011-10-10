package org.OpenNI.Samples.Assistant;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;

public class Profiler {

	private HashMap<String, List<Long>> dict;
	private long prev;
	
	public Profiler() {
		dict = new HashMap<String, List<Long>>();
	}
	
	public void sampleNow(String id)
	{
		if (!dict.containsKey(id)) {
			dict.put(id, new ArrayList<Long>(1024));
		}
		dict.get(id).add(getDelta());
	}
	
	public void addManualSample(String id, long delta)
	{
		if (!dict.containsKey(id)) {
			dict.put(id, new ArrayList<Long>(1024));
		}
		dict.get(id).add(delta);
	}
	
	public String toString()
	{
		HashMap<Double, String> res = new HashMap<Double, String>();
		for (String id : dict.keySet()) {
			double avg = 0;
			for (Long l : dict.get(id)) {
				avg+=l;
			}avg /= dict.get(id).size();
			res.put(avg, id);
		}
		
		StringBuilder sb = new StringBuilder("Profiler's summary:\n");
		
		List<Double> sorted = new ArrayList<Double>(res.keySet());
		Collections.sort(sorted);
		Collections.reverse(sorted);
		for (Double avg : sorted) {
			
			sb.append(String.format("avg for '%s':\t%.2fms\n", res.get(avg), avg));
		}
		return sb.toString();
	}
	
	public void reset()
	{
		prev = System.currentTimeMillis();
	}
	
	private long getDelta()
	{
		long now, delta;
		now = System.currentTimeMillis();
		delta = now - prev;
		prev = now;
		return delta;
	}
}
