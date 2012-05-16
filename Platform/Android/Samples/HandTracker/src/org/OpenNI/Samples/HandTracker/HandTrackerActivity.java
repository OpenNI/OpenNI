package org.OpenNI.Samples.HandTracker;

import java.io.*;

import org.OpenNI.GeneralException;
import org.OpenNI.Samples.Assistant.OpenNIBaseActivity;
import org.OpenNI.Samples.Assistant.Screen;

import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;

public class HandTrackerActivity extends OpenNIBaseActivity {
	
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		
		
		try {
			retrieveXml(HandTracker.SAMPLE_XML_FILE);
		} catch (IOException e) {
			Log.e(TAG,"onCreate() Failed!", e);
			return;
		}
		
		super.onCreate(savedInstanceState);
	}
	
	private HandTracker HandTracker;
	
	private Screen theScreen;
	

	@Override
	protected void myInitFunction() throws GeneralException {
		// TODO Auto-generated method stub
		theScreen = new Screen(this);
		theScreen.setBackgroundColor(Color.BLACK);
		theScreen.showFPS(true);
		setContentView(theScreen);
		HandTracker = new HandTracker();
	}
	@Override
	protected void myCleanupFunction() {
		// TODO Auto-generated method stub
		if (HandTracker != null)
		{
			HandTracker.Cleanup();
			HandTracker = null;
		}

		Log.i(TAG, theScreen.getFPSCalculator().toString());
	}
	@Override
	protected boolean myMainLoopContent() {
		// TODO Auto-generated method stub
		if (HandTracker.shouldDie)
			return false;
		
		try {
			HandTracker.updateDepth();
			HandTracker.doSmartThings();
			HandTracker.paint(theScreen);
			theScreen.redraw();
		} catch (Exception e) {
			Log.e(TAG, "An exception was caught during mainLoop:", e);
			return false;
		}
		
		return true;
	}
	
}
