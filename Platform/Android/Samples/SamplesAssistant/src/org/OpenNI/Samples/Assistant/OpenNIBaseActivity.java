package org.OpenNI.Samples.Assistant;

import java.io.*;
import java.util.Timer;
import java.util.TimerTask;


import android.app.Activity;
import android.os.Bundle;
import android.util.Log;

public abstract class OpenNIBaseActivity extends Activity {
    
	protected final String TAG = getClass().getSimpleName();
	
	private static Activity activity = null;
	public static Activity getCurrentActivity() { return activity; }
	
	
	@Override
	public void onCreate(Bundle savedInstanceState) {
		Log.d(TAG, "OnCreate()");
		activity = this;

		/* 
		 * As for now, we couldn't find a 100% solution for a reboot-persistent 
		 * usb 666 permissions.
		 * This code is made to ensure that /proc/bus/usb is mounted.
		 * It will ask for 'su' permission only if it's not mounted.
		 */
		try {
			enforceUSBMount();
		} catch (Exception e) {
			Log.e(TAG, "onCreate() Failed!", e);
			return;
		}

		super.onCreate(savedInstanceState);
	}
    
    @Override
    protected void onDestroy() {
    	Log.d(TAG, "OnDestroy()");
    	// TODO Auto-generated method stub
    	super.onDestroy();
    }
    
    @Override
    protected void onRestart() {
    	Log.d(TAG, "OnRestart()");
    	// TODO Auto-generated method stub
    	super.onRestart();
    }
    
    @Override
    protected void onStart() {
    	Log.i(TAG, "OnStart()");
    	try {
			openNIInit();
		} catch (Exception e) {
			Log.e(TAG,"Could not initialize Activity:", e);
			return;
		}

        shouldRun = true;
        mainLoopThread = new Thread() {
        	public void run() {
        		mainLoop();
        	};
        };
        mainLoopThread.setName("OpenNI MainLoop Thread");
        mainLoopThread.start();
    	
    	// TODO Auto-generated method stub
    	super.onStart();
    }
    
    @Override
    protected void onStop() {
    	Log.i(TAG, "OnStop()");
    	killOnTimeout = false;
    	
    	openNITerminate(); // I'll be back...
    	
    	// TODO Auto-generated method stub
    	super.onStop();
    }
    
    @Override
    protected void onResume() {
    	Log.d(TAG, "OnResume()");
    	killOnTimeout = false;
    	// TODO Auto-generated method stub
    	super.onResume();
    }
    
    @Override
    protected void onPause() {
    	Log.d(TAG, "OnPause()");
    	
    	//we wanna make sure that he's not kidding. if he won't resume soon, we kill him.
    	Timer timer = new Timer();
    	killOnTimeout = true;
    	timer.schedule(new TimerTask() {
			
			@Override
			public void run() {
				if(killOnTimeout)
				{
					Log.i(TAG,"onPause() without onStop() timed out (" + killOnTimeoutMillisec + "ms): It's time to DIE!");
					openNITerminate();
					Log.d(TAG, "triggering finish()...");
					finish();				
				}
			}
		}, killOnTimeoutMillisec);
    	
    	// TODO Auto-generated method stub
    	super.onPause();
    }
    

	///////////////////////////////////////////
	
	protected void retrieveXml(String filename) throws IOException
	{
		InputStream is = getAssets().open(filename);
		
		deleteFile(filename);
		OutputStream os = openFileOutput(filename, MODE_WORLD_READABLE);
		
		byte[] buffer = new byte[is.available()];
		is.read(buffer);
		is.close();
		os.write(buffer);
		os.close();
	}
	
	private void enforceUSBMount() throws Exception
	{
		String check = "mount";
    	String output = runInShell("sh", check);
    	
		if (!output.contains("/proc/bus/usb")) {
			Log.i(TAG, "Mounting usbfs on /proc/bus/usb ...");
			
			String mount = "mount -t usbfs -o devmode=0666 none /proc/bus/usb";
			runInShell("su", mount);
		}
	}

	private static String runInShell(String shell, String script)
			throws Exception {
		Process process = Runtime.getRuntime().exec(shell);
		DataOutputStream os = new DataOutputStream(process.getOutputStream());
		DataInputStream is = new DataInputStream(process.getInputStream());

		os.writeBytes(script + "\n");

		os.writeBytes("exit\n");
		os.flush();
		os.close();

		process.waitFor();

		byte[] arr = new byte[1000];
		int nRead = is.read(arr);
		if (nRead > 0)
			return new String(arr, 0, nRead);
		else
			return "";
	}
	
	
    
    private boolean isOpenNIInitialized = false;
    private Thread mainLoopThread;
    private boolean shouldRun = true;
    private boolean killOnTimeout;
    private int killOnTimeoutMillisec = 1000;

    
    private synchronized void openNIInit() throws Exception
    {
    	if(isOpenNIInitialized) {
    		return;
    	}
    	myInitFunction();
    	isOpenNIInitialized = true;
    }
    
    private synchronized void openNITerminate()
    {
    	if(!isOpenNIInitialized) {
    		return;
    	}
    	shouldRun = false;
		while (mainLoopThread != null) {
			try {
				mainLoopThread.join();
				mainLoopThread = null;
				break;
			} catch (InterruptedException e) { /* i don't care. */}
		}
		// now clean up!
		isOpenNIInitialized = false;
		myCleanupFunction();
    }
    
    
    
    
    protected abstract void myInitFunction() throws Exception;
    protected abstract void myCleanupFunction(); 
    // returns true only if the loop should continue running
    protected abstract boolean myMainLoopContent();
    
    
    
    
    private void mainLoop()
    {
    	while(shouldRun && myMainLoopContent());
    	
    	Log.i(TAG, "mainLoop() finished");
    	
    	if (!shouldRun) {
			// killed from the outside
    		return;
		}
    	Log.d(TAG, "triggering finish()...");
		finish();
    }
 
}