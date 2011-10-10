package org.OpenNI.Samples.SimpleViewer;

import java.io.IOException;

import org.OpenNI.Samples.Assistant.OpenNIBaseActivity;
import org.OpenNI.Samples.Assistant.Screen;

import android.graphics.Bitmap;
import android.os.Bundle;
import android.util.Log;

public class SimpleViewerActivity extends OpenNIBaseActivity {
	private SimpleViewer simpleViewer;

	private Screen theScreen;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		try {
			retrieveXml(SimpleViewer.SAMPLE_XML_FILE);
		} catch (IOException e) {
			Log.e(TAG, "onCreate() Failed!", e);
			return;
		}
		Log.d(TAG, "create done");
	}

	private void initScreen(int width, int height) {
		theScreen = new Screen(this);
		theScreen.setDimensions(width, height);
		theScreen.showFPS(true);
		setContentView(theScreen);
	}

	public void draw(Bitmap bmp) {
		theScreen.setBitmap(bmp);
		theScreen.redraw();
	}

	@Override
	protected void myInitFunction() {
		// TODO Auto-generated method stub
		Log.d(TAG, "init");
		
		simpleViewer = new SimpleViewer();
		initScreen(simpleViewer.width, simpleViewer.height);

		Log.d(TAG, "init done");
	}

	@Override
	protected void myCleanupFunction() {
		// TODO Auto-generated method stub
		simpleViewer.Cleanup();
	}

	@Override
	protected boolean myMainLoopContent() {
		try {
			simpleViewer.updateDepth();
			draw(simpleViewer.drawBitmap());
		} 
		catch (Exception e) {
			Log.e(TAG, "An exception was caught during mainLoop:", e);
			return false;
		}

		return true;
	}
}
