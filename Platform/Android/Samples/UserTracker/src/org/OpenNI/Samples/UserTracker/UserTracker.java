package org.OpenNI.Samples.UserTracker;

import org.OpenNI.*;
import org.OpenNI.Samples.Assistant.BitmapGenerator;
import org.OpenNI.Samples.Assistant.Screen;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.graphics.Color;
import android.util.Log;

import java.nio.ShortBuffer;
import java.util.HashMap;


public class UserTracker
{
	private final String TAG = getClass().getSimpleName();
	public static final String SAMPLE_XML_FILE = "SamplesConfig.xml";
	
	class NewUserObserver implements IObserver<UserEventArgs>
	{
		@Override
		public void update(IObservable<UserEventArgs> observable,
				UserEventArgs args)
		{
			Log.i(TAG,"New user " + args.getId());
			try
			{
				if (skeletonCap.needPoseForCalibration())
				{
					poseDetectionCap.startPoseDetection(calibPose, args.getId());
				}
				else
				{
					skeletonCap.requestSkeletonCalibration(args.getId(), true);
				}
			} catch (StatusException e)
			{
				Log.e(TAG, "Exception!", e);
			}
		}
	}
	class LostUserObserver implements IObserver<UserEventArgs>
	{
		@Override
		public void update(IObservable<UserEventArgs> observable,
				UserEventArgs args)
		{
			Log.i(TAG,"Lost user " + args.getId());
			joints.remove(args.getId());
		}
	}
	
	class CalibrationCompleteObserver implements IObserver<CalibrationProgressEventArgs>
	{
		@Override
		public void update(IObservable<CalibrationProgressEventArgs> observable,
				CalibrationProgressEventArgs args)
		{
			Log.i(TAG,"Calibraion complete: " + args.getStatus());
			try
			{
			if (args.getStatus() == CalibrationProgressStatus.OK)
			{
				Log.i(TAG,"starting tracking "  +args.getUser());
				skeletonCap.startTracking(args.getUser());
				joints.put(new Integer(args.getUser()), new HashMap<SkeletonJoint, SkeletonJointPosition>());
			}
			else
			{
				if (skeletonCap.needPoseForCalibration())
				{
					poseDetectionCap.startPoseDetection(calibPose, args.getUser());
				}
				else
				{
					skeletonCap.requestSkeletonCalibration(args.getUser(), true);
				}
			}
			} catch (StatusException e)
			{
				Log.e(TAG, "Exception!", e);
			}
		}
	}
	class PoseDetectedObserver implements IObserver<PoseDetectionEventArgs>
	{
		@Override
		public void update(IObservable<PoseDetectionEventArgs> observable,
				PoseDetectionEventArgs args)
		{
			Log.i(TAG,"Pose " + args.getPose() + " detected for " + args.getUser());
			try
			{
				poseDetectionCap.stopPoseDetection(args.getUser());
				skeletonCap.requestSkeletonCalibration(args.getUser(), true);
			} catch (StatusException e)
			{
				Log.e(TAG, "Exception!", e);
			}
		}
	}
	
	
	public boolean shouldDie = false;

	private OutArg<ScriptNode> scriptNode;
	private Context context;
	private DepthGenerator depthGen;
	private UserGenerator userGen;
	private SkeletonCapability skeletonCap;
	private PoseDetectionCapability poseDetectionCap;
	private byte[] imgbytes;
	private float histogram[];
	String calibPose = null;
	HashMap<Integer, HashMap<SkeletonJoint, SkeletonJointPosition>> joints;
	
	private BitmapGenerator bitmapGenerator;
	
	private CrossWatcher crossDetector;

	private boolean drawBackground = true;
	private boolean drawPixels = true;
	private boolean drawSkeleton = true;
	private boolean drawHead = false;
	private boolean printID = true;
	private boolean printState = true;
	private boolean useJNI = true;
	
	int width, height;
	
	
	public UserTracker() throws GeneralException
	{
		crossDetector = new CrossWatcher(new Runnable() {
			@Override
			public void run() {
				shouldDie = true;
			}
		}, 2);
		
		try {
			scriptNode = new OutArg<ScriptNode>();
			String xmlName = UserTrackerActivity.getCurrentActivity().getFilesDir() +"/"+ SAMPLE_XML_FILE;
			context = Context.createFromXmlFile(xmlName, scriptNode);
			depthGen = DepthGenerator.create(context);

			DepthMetaData depthMD = depthGen.getMetaData();

			histogram = new float[10000];
			width = depthMD.getFullXRes();
			height = depthMD.getFullYRes();
			bitmap = Bitmap.createBitmap(width, height, Config.ARGB_8888);
			
			imgbytes = new byte[width*height*3];

			userGen = UserGenerator.create(context);

			bitmapGenerator = new BitmapGenerator(context, true, true);
			
			skeletonCap = userGen.getSkeletonCapability();
			poseDetectionCap = userGen.getPoseDetectionCapability();
			userGen.getNewUserEvent().addObserver(new NewUserObserver());
			userGen.getLostUserEvent().addObserver(new LostUserObserver());
			skeletonCap.getCalibrationCompleteEvent().addObserver(new CalibrationCompleteObserver());
			poseDetectionCap.getPoseDetectedEvent().addObserver(new PoseDetectedObserver());
			
			calibPose = skeletonCap.getSkeletonCalibrationPose();
			joints = new HashMap<Integer, HashMap<SkeletonJoint,SkeletonJointPosition>>();
			
			skeletonCap.setSkeletonProfile(SkeletonProfile.ALL);
			
			context.startGeneratingAll();

		} catch (GeneralException e) {
			e.fillInStackTrace();
			Log.e(TAG, e.toString());
			throw e;
		}
	}
	
	public void Cleanup()
	{
		Log.i(TAG, "Cleanup!");
		scriptNode.value.dispose();
		scriptNode = null;
		
		skeletonCap.dispose();
		skeletonCap = null;
		poseDetectionCap.dispose();
		poseDetectionCap = null;
		
		depthGen.dispose();
		depthGen = null;
		userGen.dispose();
		userGen = null;
		
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
	
	
	private void calcHist(ShortBuffer depth)
	{
		// reset
		for (int i = 0; i < histogram.length; ++i)
			histogram[i] = 0;
		
		depth.rewind();

		int points = 0;
		while(depth.remaining() > 0)
		{
			short depthVal = depth.get();
			if (depthVal != 0)
			{
				histogram[depthVal]++;
				points++;
			}
		}
		
		for (int i = 1; i < histogram.length; i++)
		{
			histogram[i] += histogram[i-1];
		}

		if (points > 0)
		{
			for (int i = 1; i < histogram.length; i++)
			{
				histogram[i] = 1.0f - (histogram[i] / (float)points);
			}
		}
	}

	void updateDepth_java() throws StatusException {
		context.waitAnyUpdateAll();

		DepthMetaData depthMD = depthGen.getMetaData();
		SceneMetaData sceneMD = userGen.getUserPixels(0);

		ShortBuffer scene = sceneMD.getData().createShortBuffer();
		ShortBuffer depth = depthMD.getData().createShortBuffer();
		calcHist(depth);
		depth.rewind();

		while (depth.remaining() > 0) {
			int pos = depth.position();
			short pixel = depth.get();
			short user = scene.get();

			imgbytes[3 * pos] = 0;
			imgbytes[3 * pos + 1] = 0;
			imgbytes[3 * pos + 2] = 0;

			if (drawBackground || pixel != 0) {
				int colorID = user % (colors.length - 1);
				if (user == 0) {
					colorID = colors.length - 1;
				}
				if (pixel != 0) {
					float histValue = histogram[pixel];
					imgbytes[3 * pos] = (byte) (histValue * Color
							.red(colors[colorID]));
					imgbytes[3 * pos + 1] = (byte) (histValue * Color
							.green(colors[colorID]));
					imgbytes[3 * pos + 2] = (byte) (histValue * Color
							.blue(colors[colorID]));
				}
			}
		}
	}

	void updateDepth_jni() throws StatusException {
		context.waitAnyUpdateAll();
		bitmapGenerator.generateBitmap();
	}
	
	void updateDepth() throws StatusException
	{
		if(useJNI)
			updateDepth_jni();
		else
			updateDepth_java();
	}

	int colors[] = {Color.GREEN, Color.RED, Color.YELLOW, Color.CYAN, Color.BLUE, Color.rgb(255, 127, 0), Color.rgb(127, 255, 0), Color.LTGRAY };
	
	public void getJoint(int user, SkeletonJoint joint) throws StatusException
	{
		SkeletonJointPosition pos = skeletonCap.getSkeletonJointPosition(user, joint);
		if (pos.getPosition().getZ() != 0)
		{
			joints.get(user).put(joint, new SkeletonJointPosition(depthGen.convertRealWorldToProjective(pos.getPosition()), pos.getConfidence()));
		}
		else
		{
			joints.get(user).put(joint, new SkeletonJointPosition(new Point3D(), 0));
		}
	}
	public void getJoints(int user) throws StatusException
	{
		getJoint(user, SkeletonJoint.HEAD);
		getJoint(user, SkeletonJoint.NECK);
		
		getJoint(user, SkeletonJoint.LEFT_SHOULDER);
		getJoint(user, SkeletonJoint.LEFT_ELBOW);
		getJoint(user, SkeletonJoint.LEFT_HAND);

		getJoint(user, SkeletonJoint.RIGHT_SHOULDER);
		getJoint(user, SkeletonJoint.RIGHT_ELBOW);
		getJoint(user, SkeletonJoint.RIGHT_HAND);

		getJoint(user, SkeletonJoint.TORSO);

		getJoint(user, SkeletonJoint.LEFT_HIP);
		getJoint(user, SkeletonJoint.LEFT_KNEE);
		getJoint(user, SkeletonJoint.LEFT_FOOT);

		getJoint(user, SkeletonJoint.RIGHT_HIP);
		getJoint(user, SkeletonJoint.RIGHT_KNEE);
		getJoint(user, SkeletonJoint.RIGHT_FOOT);

	}
	void drawLine(Screen g, HashMap<SkeletonJoint, SkeletonJointPosition> jointHash, SkeletonJoint joint1, SkeletonJoint joint2)
	{
		Point3D pos1 = jointHash.get(joint1).getPosition();
		Point3D pos2 = jointHash.get(joint2).getPosition();

		if (jointHash.get(joint1).getConfidence() == 0 || jointHash.get(joint1).getConfidence() == 0)
			return;

		g.drawLine((int)pos1.getX(), (int)pos1.getY(), (int)pos2.getX(), (int)pos2.getY());
	}
	void drawHead(Screen g, HashMap<SkeletonJoint, SkeletonJointPosition> jointHash)
	{
		SkeletonJointPosition h = jointHash.get(SkeletonJoint.HEAD); 
//		SkeletonJointPosition l = jointHash.get(SkeletonJoint.LEFT_SHOULDER);
//		SkeletonJointPosition r = jointHash.get(SkeletonJoint.RIGHT_SHOULDER);
		Point3D ph =  h.getPosition();
//		Point3D pl = l.getPosition();
//		Point3D pr = r.getPosition();

		if (h.getConfidence() == 0
//				|| l.getConfidence() == 0 || r.getConfidence() == 0
				)
			return;
		
		float Z = ((4000 - ph.getZ())/4000)*30;
		
		g.drawCircle(ph.getX(), ph.getY(), Z);
	}
	public void drawSkeleton(Screen g, int user) throws StatusException
	{
		getJoints(user);
		HashMap<SkeletonJoint, SkeletonJointPosition> dict = joints.get(new Integer(user));

		if(drawHead)
			drawHead(g, dict);
		
		drawLine(g, dict, SkeletonJoint.HEAD, SkeletonJoint.NECK);

		drawLine(g, dict, SkeletonJoint.LEFT_SHOULDER, SkeletonJoint.TORSO);
		drawLine(g, dict, SkeletonJoint.RIGHT_SHOULDER, SkeletonJoint.TORSO);

		drawLine(g, dict, SkeletonJoint.NECK, SkeletonJoint.LEFT_SHOULDER);
		drawLine(g, dict, SkeletonJoint.LEFT_SHOULDER, SkeletonJoint.LEFT_ELBOW);
		drawLine(g, dict, SkeletonJoint.LEFT_ELBOW, SkeletonJoint.LEFT_HAND);

		drawLine(g, dict, SkeletonJoint.NECK, SkeletonJoint.RIGHT_SHOULDER);
		drawLine(g, dict, SkeletonJoint.RIGHT_SHOULDER, SkeletonJoint.RIGHT_ELBOW);
		drawLine(g, dict, SkeletonJoint.RIGHT_ELBOW, SkeletonJoint.RIGHT_HAND);

		drawLine(g, dict, SkeletonJoint.LEFT_HIP, SkeletonJoint.TORSO);
		drawLine(g, dict, SkeletonJoint.RIGHT_HIP, SkeletonJoint.TORSO);
		drawLine(g, dict, SkeletonJoint.LEFT_HIP, SkeletonJoint.RIGHT_HIP);

		drawLine(g, dict, SkeletonJoint.LEFT_HIP, SkeletonJoint.LEFT_KNEE);
		drawLine(g, dict, SkeletonJoint.LEFT_KNEE, SkeletonJoint.LEFT_FOOT);

		drawLine(g, dict, SkeletonJoint.RIGHT_HIP, SkeletonJoint.RIGHT_KNEE);
		drawLine(g, dict, SkeletonJoint.RIGHT_KNEE, SkeletonJoint.RIGHT_FOOT);
		
		Point3D pl = dict.get(SkeletonJoint.LEFT_HAND).getPosition();
		Point3D pr = dict.get(SkeletonJoint.RIGHT_HAND).getPosition();
		g.drawLabel("L", (int)pl.getX(), (int)pl.getY());
		g.drawLabel("R", (int)pr.getX(), (int)pr.getY());

	}
	
	private Bitmap bitmap;
	private int[] pixels;
	
	public void paint(Screen screen) throws StatusException
	{
		screen.setDimensions(width, height);
		
		if (drawPixels)
		{
			if (useJNI) {
				pixels = bitmapGenerator.getLastBitmap();
			}
			else
			{
				if(pixels == null)
					pixels = new int[width*height];
				for (int i = 0; i < pixels.length; i++) {
					pixels[i] = Color.rgb(imgbytes[i*3], imgbytes[i*3+1], imgbytes[i*3+2]);
				}
			}
			bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
			screen.setBitmap(bitmap);
		}
		
		int[] users = userGen.getUsers();
		for (int i = 0; i < users.length; ++i) {
			int c = colors[(users[i] - 1) % colors.length];
			c = Color.rgb(255 - Color.red(c), 255 - Color.green(c),
					255 - Color.blue(c));

			screen.setColor(c);

			if (drawSkeleton && skeletonCap.isSkeletonTracking(users[i])) {
				drawSkeleton(screen, users[i]);
			}

			if (printID) {
				Point3D com = depthGen.convertRealWorldToProjective(userGen.getUserCoM(users[i]));
				String label = null;
				if (!printState) {
					label = new String("" + users[i]);
				} else if (skeletonCap.isSkeletonTracking(users[i])) {
					// Tracking
					label = new String(users[i] + " - Tracking");
				} else if (skeletonCap.isSkeletonCalibrating(users[i])) {
					// Calibrating
					label = new String(users[i] + " - Calibrating");
				} else {
					// Nothing
					label = new String(users[i] + " - Looking for pose ("
							+ calibPose + ")");
				}

				screen.drawLabel(label, (int) com.getX(), (int) com.getY());
			}
		}

		// draw exit warning, if necessary.
		Float time2Die = crossDetector.getSeconds2Callback();
		screen.setColor(Color.WHITE);
		if (time2Die != null)
			screen.drawLabel(String.format("Terminating in %.1f Seconds", time2Die),
					100, 10);
	}
	
	public void doSmartThings() throws StatusException {
		int[] users;
		users = userGen.getUsers();

		for (int i = 0; i < users.length; ++i) {
			int user = users[i];

			if (skeletonCap.isSkeletonTracking(user)) {
				getJoints(user);
				HashMap<SkeletonJoint, SkeletonJointPosition> dict = joints
						.get(user);

				Point3D posLE = dict.get(SkeletonJoint.LEFT_ELBOW)
						.getPosition();
				Point3D posRE = dict.get(SkeletonJoint.RIGHT_ELBOW)
						.getPosition();
				Point3D posLH = dict.get(SkeletonJoint.LEFT_HAND).getPosition();
				Point3D posRH = dict.get(SkeletonJoint.RIGHT_HAND)
						.getPosition();
				if (((posLE.getX() - posRE.getX()) * (posLH.getX() - posRH
						.getX())) < 0
						&& (posLH.getY() < posRE.getY())
						&& (posRH.getY() < posLE.getY())) {
					crossDetector.reportCross(user, true);

				} else {
					crossDetector.reportCross(user, false);
				}
			}
		}
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

