package org.OpenNI.Samples.HandTracker;

import org.OpenNI.*;
import org.OpenNI.Samples.Assistant.BitmapGenerator;
import org.OpenNI.Samples.Assistant.Screen;
import org.OpenNI.Samples.HandTracker.HandTracker.Trailer.Trail;
import org.OpenNI.Samples.HandTracker.HandTracker.Trailer.Trail.Dot;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Color;
import android.util.Log;


import java.util.HashMap;
import java.util.Iterator;


public class HandTracker
{
    class GestureRecognizedObserver implements IObserver<GestureRecognizedEventArgs>
    {   
            @Override
            public void update(IObservable<GestureRecognizedEventArgs> observable,
            		GestureRecognizedEventArgs args)
            {   
                    Log.i(TAG,"Gesture detected: " + args.getGesture());
                    try 
                    {   
                        handsGen.StartTracking(args.getEndPosition());   
                    } catch (StatusException e)
                    {   
                            Log.e(TAG, "Exception!", e); 
                    }   
            }   
    }
    class HandCreatedObserver implements IObserver<ActiveHandEventArgs>
    {   
            @Override
            public void update(IObservable<ActiveHandEventArgs> observable,
            		ActiveHandEventArgs args)
            {   
                    Log.i(TAG,"New hand created: " + args.getId());
                    handsManager.addNewHand(args.getId(), args.getPosition()); 
            }   
    }
    class HandUpdatedObserver implements IObserver<ActiveHandEventArgs>
    {   
            @Override
            public void update(IObservable<ActiveHandEventArgs> observable,
            		ActiveHandEventArgs args)
            {   
//                String pos = String.format("X: %.1f   Y: %.1f   Z: %.1f", args.getPosition().getX(),args.getPosition().getY(),args.getPosition().getZ()); 
//                Log.d(TAG,String.format("Hand %d update: %s", args.getId(),pos));
                handsManager.updateHandPosition(args.getId(), args.getPosition());
            }   
    }
    class HandDestroyedObserver implements IObserver<InactiveHandEventArgs>
    {   
            @Override
            public void update(IObservable<InactiveHandEventArgs> observable,
            		InactiveHandEventArgs args)
            {   
                    Log.i(TAG,"Hand destroyed: " + args.getId());
                    handsManager.removeHand(args.getId());
            }   
    }
    
	private final String TAG = getClass().getSimpleName();
	public static final String SAMPLE_XML_FILE = "SamplesConfig.xml";
	
	public boolean shouldDie = false;

	private OutArg<ScriptNode> scriptNode;
	private Context context;
	private DepthGenerator depthGen;
	private UserGenerator userGen;
	private GestureGenerator gestureGen;
	private HandsGenerator handsGen;

	private BitmapGenerator bitmapGenerator;
	
	private boolean drawPixels = true;
	
	int width, height;
	
	private CrossWatcher crossDetector;
	private HandsManager handsManager;
	private Trailer trailer;
	
	
	public HandTracker() throws GeneralException
	{
		crossDetector = new CrossWatcher(new Runnable() {
			@Override
			public void run() {
				shouldDie = true;
			}
		}, 2);
		
		try {
			scriptNode = new OutArg<ScriptNode>();
			String xmlName = HandTrackerActivity.getCurrentActivity().getFilesDir() +"/"+ SAMPLE_XML_FILE;
			context = Context.createFromXmlFile(xmlName, scriptNode);
			
			depthGen = DepthGenerator.create(context);

			DepthMetaData depthMD = depthGen.getMetaData();

			width = depthMD.getFullXRes();
			height = depthMD.getFullYRes();
			bitmap = Bitmap.createBitmap(width, height, Config.ARGB_8888);

//			userGen = UserGenerator.create(context);
			gestureGen = GestureGenerator.create(context);
			handsGen = HandsGenerator.create(context);
			
			gestureGen.addGesture("Click");
			gestureGen.addGesture("Wave");
			gestureGen.getGestureRecognizedEvent().addObserver(new GestureRecognizedObserver());
			
			handsGen.getHandCreateEvent().addObserver(new HandCreatedObserver());
			handsGen.getHandUpdateEvent().addObserver(new HandUpdatedObserver());
			handsGen.getHandDestroyEvent().addObserver(new HandDestroyedObserver());

			handsManager = new HandsManager();
			trailer = new Trailer();			
			
			bitmapGenerator = new BitmapGenerator(context, false, true);
			
			context.startGeneratingAll();

		} catch (GeneralException e) {
			Log.e(TAG, "Exception!", e); 
			throw e;
		}
	}
	
	public void Cleanup()
	{
		Log.i(TAG, "Cleanup!");
		scriptNode.value.dispose();
		scriptNode = null;

		depthGen.dispose();
		depthGen = null;
//		userGen.dispose();
		userGen = null;
		gestureGen.dispose();
		gestureGen = null;
		handsGen.dispose();
		handsGen = null;
		
		try {
			bitmapGenerator.dispose();
		} catch (StatusException e) {
			Log.e(TAG, "Exception!", e);
		}
		bitmapGenerator = null;
		
		context.dispose();
		context = null;
		Log.i(TAG, "Cleanup done.");
	}
	

	void updateDepth() throws StatusException
	{
		context.waitAnyUpdateAll();
		bitmapGenerator.generateBitmap();
	}

	int colors[] = {Color.GREEN, Color.RED, Color.YELLOW, Color.CYAN, Color.BLUE, Color.rgb(255, 127, 0), Color.rgb(127, 255, 0), Color.LTGRAY };

	
	private Bitmap bitmap;
	private int[] pixels;
	
	public void paint(Screen screen) throws StatusException
	{
		screen.setDimensions(width, height);
		
		if (drawPixels)
		{
			pixels = bitmapGenerator.getLastBitmap();
			
			bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
			screen.setBitmap(bitmap);
		}
		for (Trail trail : trailer.getTrails()) {
			screen.setColor(trail.color);
			Dot dot,prev=null;
			for(Iterator<Dot> iter = trail.readIter(); iter.hasNext();)
			{
				dot = iter.next();
				if(prev != null)
					screen.drawLine(prev.x, prev.y, dot.x, dot.y);
				prev = dot;
			}
			screen.drawCircle(prev.x, prev.y, prev.size);
			
		}

		// draw exit warning, if necessary.
		Float time2Die = crossDetector.getSeconds2Callback();
		screen.setColor(Color.WHITE);
		if (time2Die != null)
			screen.drawLabel(String.format("Terminating in %.1f Seconds", time2Die),
					100, 10);
	}
	
	public void doSmartThings() throws StatusException {

	}
	
	class HandsManager {
		public HandsManager() {
			map = new HashMap<Integer, Point3D>();
		}
		
		public void addNewHand(int handID, Point3D startPos)
		{
			map.put(handID, startPos);
			trailer.addTrail(handID);
			updateHandPosition(handID, startPos);
		}
		
		public void removeHand(int handID)
		{
			map.remove(handID);
			trailer.delTrail(handID);
		}
		
		public void updateHandPosition(int handID, Point3D curPos)
		{
			try {
				trailer.addDot(handID, curPos, map.get(handID));
			} catch (StatusException e) {
				Log.e(TAG, "Exception!", e); 
			}
		}
		
		public HashMap<Integer, Point3D> map;
		
		
	}
	public class Trailer
	{
		public static final int TRAIL_LENGTH = 10; 
		public class Trail
		{
			class Dot 
			{
				public Dot(float x, float y, float size)
				{
					this.x = x;
					this.y = y;
					this.size = size;
				}
				public float x,y,size;
			}
			
			public Trail(int color) {
				dots = new Dot[TRAIL_LENGTH];
				cycPos = cycFull = 0;
				this.color = color;
			}
			
			public void addDot(Point3D curHandWorld, Point3D baseHandWorld) throws StatusException
			{
				float z = curHandWorld.getZ() - baseHandWorld.getZ();
				z = 3 + ((-z)/10);
				if (z<3) z = 3;
				Point3D proj = depthGen.convertRealWorldToProjective(curHandWorld);
				
				dots[cycPos] = new Dot(proj.getX(), proj.getY(),z);
				
				if (cycFull < TRAIL_LENGTH) cycFull++;
				cycPos = (cycPos + 1) % TRAIL_LENGTH;
			}
			
			public Iterator<Dot> readIter()
			{
				return new Iterator<HandTracker.Trailer.Trail.Dot>() {
					
					int read = (cycFull < TRAIL_LENGTH ? 0 : (cycPos + 1) % TRAIL_LENGTH);
					
					@Override
					public void remove() {
						// TODO Auto-generated method stub
						
					}
					
					@Override
					public Dot next() {
						if (read == cycPos)
							return null;
						Dot d = dots[read++];
						read %= TRAIL_LENGTH;
						return d;
					}
					
					@Override
					public boolean hasNext() {
						return (!(read == cycPos));
					}
				};
			}
			
			private Dot dots[];
			private int cycPos;
			private int cycFull;
			
			public int color;
			
		}
		
		public Trailer() {
			trails = new HashMap<Integer, HandTracker.Trailer.Trail>();
		}
		
		public void addTrail(int id)
		{
			trails.put(id, new Trail(colors[id % colors.length]));
		}
		
		public void delTrail(int id)
		{
			trails.remove(id);
		}
		
		public void addDot(int id, Point3D curPos, Point3D basePos) throws StatusException
		{
			trails.get(id).addDot(curPos, basePos);
		}
		
		public Iterable<Trail> getTrails()
		{
			return trails.values();
		}
		
		private HashMap<Integer, Trail> trails;
	}
	
	class CrossWatcher
	{
		public CrossWatcher(Runnable terminatorCallback, int seconds2Callback) {
			dict = new HashMap<Integer, Long>();
			terminator = terminatorCallback;
			timeout = seconds2Callback * 1000;
		}
		
		public void reportCross(int user, boolean isCrossing)
		{
			long now = System.currentTimeMillis();
			if(isCrossing)
			{
				if (dict.containsKey(user))
				{
					if(now - dict.get(user) >= timeout)
					{
						terminator.run();
					}
				}
				else
				{
					dict.put(user, now);
				}
			}
			else
			{
				if (dict.containsKey(user))
				{
					dict.remove(user);
				}
			}
		}
		public Float getSeconds2Callback() {
			float max = 0;
			long now = System.currentTimeMillis();
			for (Long l : dict.values()) {
				max = Math.max((float)(now -l), max);
			}
			float left = timeout - max;
			if (left == timeout || left < 0) return null;
			return left / 1000;
		}
		
		private Runnable terminator;
		private int timeout;
		private HashMap<Integer,Long> dict;
		
	}
  
}

