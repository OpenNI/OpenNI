package org.OpenNI.Samples.UserTracker;

import java.io.*;

import org.OpenNI.GeneralException;
import org.OpenNI.Samples.Assistant.OpenNIBaseActivity;
import org.OpenNI.Samples.Assistant.Screen;

import android.graphics.Color;
import android.os.Bundle;
import android.util.Log;

public class UserTrackerActivity extends OpenNIBaseActivity {
	
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		
		
		try {
			retrieveXml(UserTracker.SAMPLE_XML_FILE);
		} catch (IOException e) {
			Log.e(TAG,"onCreate() Failed!", e);
			return;
		}
		
		super.onCreate(savedInstanceState);
	}
	
	private UserTracker userTracker;
	
	private Screen theScreen;
	

	@Override
	protected void myInitFunction() throws GeneralException {
		// TODO Auto-generated method stub
		theScreen = new Screen(this);
		theScreen.setBackgroundColor(Color.BLACK);
		theScreen.showFPS(true);
		setContentView(theScreen);
		userTracker = new UserTracker();
	}
	@Override
	protected void myCleanupFunction() {
		// TODO Auto-generated method stub
		if (userTracker != null)
		{
			userTracker.Cleanup();
			userTracker = null;
		}

		Log.i(TAG, theScreen.getFPSCalculator().toString());
	}
	@Override
	protected boolean myMainLoopContent() {
		// TODO Auto-generated method stub
		if (userTracker.shouldDie)
			return false;
		
		try {
			userTracker.updateDepth();
			userTracker.doSmartThings();
			userTracker.paint(theScreen);
			theScreen.redraw();
		} catch (Exception e) {
			Log.e(TAG, "An exception was caught during mainLoop:", e);
			return false;
		}
		
		return true;
	}
	
}
