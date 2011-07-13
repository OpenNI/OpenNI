package org.OpenNI;

public class FieldOfView 
{
	public FieldOfView(double hFOV, double vFOV) {
		super();
		this.hFOV = hFOV;
		this.vFOV = vFOV;
	}
	
	public double getHFOV() {
		return hFOV;
	}
	
	public void setHFOV(double hFOV) {
		this.hFOV = hFOV;
	}
	
	public double getVFOV() {
		return vFOV;
	}
	
	public void setVFOV(double vFOV) {
		this.vFOV = vFOV;
	}
	
	private double hFOV;
	private double vFOV;
}
