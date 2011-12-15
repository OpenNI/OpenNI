package org.OpenNI.Samples.SimpleViewer;

import org.OpenNI.*;
import org.OpenNI.Samples.Assistant.BitmapGenerator;

import android.graphics.Bitmap;
import android.graphics.Bitmap.Config;
import android.util.Log;



class SimpleViewer  {
	private final String TAG = getClass().getSimpleName(); 
	
	private OutArg<ScriptNode> scriptNode;
    private Context context;
    private DepthGenerator depthGen;
    private BitmapGenerator bitmapGenerator;
    
    private Bitmap bitmap;

    public int width, height;
    private int[] pixels;
    

    public static final String SAMPLE_XML_FILE = "SamplesConfig.xml";    
    public SimpleViewer() {

        try {
            scriptNode = new OutArg<ScriptNode>();
            String xmlName = SimpleViewerActivity.getCurrentActivity().getFilesDir() +"/"+ SAMPLE_XML_FILE;
            context = Context.createFromXmlFile(xmlName, scriptNode);
            depthGen = DepthGenerator.create(context);
            
            bitmapGenerator = new BitmapGenerator(context, false, true);
            MapOutputMode mapOutputMode = bitmapGenerator.getMapOutputMode();
            width  = mapOutputMode.getXRes();
            height = mapOutputMode.getYRes();
            bitmap = Bitmap.createBitmap(width, height, Config.ARGB_8888);

        } catch (GeneralException e) {
            Log.e(TAG, e.toString());
            System.exit(1);
        }
    }
    
    public void Cleanup()
    {
    	Log.e(TAG, "Cleanup");
    	
    	scriptNode.value.dispose();
    	scriptNode = null;
    	
    	depthGen.dispose();
    	depthGen = null;
    	
    	try {
			bitmapGenerator.dispose();
		} catch (StatusException e) {
			Log.e(TAG, e.toString());
		}
    	
    	context.dispose();
    	context = null;

    	Log.e(TAG, "Cleanup Done");
    }
    

	void updateDepth() throws StatusException {
		context.waitAnyUpdateAll();
		bitmapGenerator.generateBitmap();
	}

   
    public Bitmap drawBitmap()
    {
    	pixels = bitmapGenerator.getLastBitmap();
    	bitmap.setPixels(pixels, 0, width, 0, 0, width, height);
        return bitmap;
    }
    
 
}


