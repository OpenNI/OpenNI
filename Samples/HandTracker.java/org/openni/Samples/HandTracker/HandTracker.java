/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
package org.openni.Samples.HandTracker;

import org.openni.*;

import java.nio.ShortBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.awt.*;
import java.awt.image.*;

class HandTracker extends Component
{

	class MyGestureRecognized implements IObserver<GestureRecognizedEventArgs>
	{

		@Override
		public void update(IObservable<GestureRecognizedEventArgs> observable,
				GestureRecognizedEventArgs args)
		{
			try
			{
				handsGen.StartTracking(args.getEndPosition());
				gestureGen.removeGesture("Click");
			} catch (StatusException e)
			{
				e.printStackTrace();
			}
		}
	}
	class MyHandCreateEvent implements IObserver<ActiveHandEventArgs>
	{
		public void update(IObservable<ActiveHandEventArgs> observable,
				ActiveHandEventArgs args)
		{
			ArrayList<Point3D> newList = new ArrayList<Point3D>();
			newList.add(args.getPosition());
			history.put(new Integer(args.getId()), newList);
		}
	}
	class MyHandUpdateEvent implements IObserver<ActiveHandEventArgs>
	{
		public void update(IObservable<ActiveHandEventArgs> observable,
				ActiveHandEventArgs args)
		{
			ArrayList<Point3D> historyList = history.get(args.getId());
			
			historyList.add(args.getPosition());
			
			while (historyList.size() > historySize)
			{
				historyList.remove(0);
			}

		}
	}
	private int historySize = 10;
	class MyHandDestroyEvent implements IObserver<InactiveHandEventArgs>
	{
		public void update(IObservable<InactiveHandEventArgs> observable,
				InactiveHandEventArgs args)
		{
			history.remove(args.getId());
			if (history.isEmpty())
			{
				try
				{
					gestureGen.addGesture("Click");
				} catch (StatusException e)
				{
					e.printStackTrace();
				}
			}
		}
	}
	
    /**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private OutArg<ScriptNode> scriptNode;
    private Context context;
    private DepthGenerator depthGen;
    private GestureGenerator gestureGen;
    private HandsGenerator handsGen;
    private HashMap<Integer, ArrayList<Point3D>> history;
    private byte[] imgbytes;
    private float histogram[];

    private BufferedImage bimg;
    int width, height;
    
    private final String SAMPLE_XML_FILE = "../../../../Data/SamplesConfig.xml";
    public HandTracker()
    {

        try {
            scriptNode = new OutArg<ScriptNode>();
            context = Context.createFromXmlFile(SAMPLE_XML_FILE, scriptNode);

            gestureGen = GestureGenerator.create(context);
            gestureGen.addGesture("Click");
            gestureGen.getGestureRecognizedEvent().addObserver(new MyGestureRecognized());
            
            handsGen = HandsGenerator.create(context);
            handsGen.getHandCreateEvent().addObserver(new MyHandCreateEvent());
            handsGen.getHandUpdateEvent().addObserver(new MyHandUpdateEvent());
            handsGen.getHandDestroyEvent().addObserver(new MyHandDestroyEvent());
            
            depthGen = DepthGenerator.create(context);
            DepthMetaData depthMD = depthGen.getMetaData();

			context.startGeneratingAll();
			
            history = new HashMap<Integer, ArrayList<Point3D>>(); 
            
            histogram = new float[10000];
            width = depthMD.getFullXRes();
            height = depthMD.getFullYRes();
            
            imgbytes = new byte[width*height];
            
            DataBufferByte dataBuffer = new DataBufferByte(imgbytes, width*height);
            Raster raster = Raster.createPackedRaster(dataBuffer, width, height, 8, null);
            bimg = new BufferedImage(width, height, BufferedImage.TYPE_BYTE_GRAY);
            bimg.setData(raster);

        } catch (GeneralException e) {
            e.printStackTrace();
            System.exit(1);
        }
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
                histogram[i] = (int)(256 * (1.0f - (histogram[i] / (float)points)));
            }
        }
    }


    void updateDepth()
    {
        try {
            DepthMetaData depthMD = depthGen.getMetaData();

            context.waitAnyUpdateAll();
            
            ShortBuffer depth = depthMD.getData().createShortBuffer();
            calcHist(depth);
            depth.rewind();
            
            while(depth.remaining() > 0)
            {
                int pos = depth.position();
                short pixel = depth.get();
                imgbytes[pos] = (byte)histogram[pixel];
            }
        } catch (GeneralException e) {
            e.printStackTrace();
        }
    }


    public Dimension getPreferredSize() {
        return new Dimension(width, height);
    }

    Color colors[] = {Color.RED, Color.BLUE, Color.CYAN, Color.GREEN, Color.MAGENTA, Color.PINK, Color.YELLOW};
    public void paint(Graphics g) {
        DataBufferByte dataBuffer = new DataBufferByte(imgbytes, width*height);
        Raster raster = Raster.createPackedRaster(dataBuffer, width, height, 8, null);
        bimg.setData(raster);

        g.drawImage(bimg, 0, 0, null);

        for (Integer id : history.keySet())
        {
        	try
        	{
        	ArrayList<Point3D> points = history.get(id);
        	g.setColor(colors[id%colors.length]);
        	int[] xPoints = new int[points.size()];
        	int[] yPoints = new int[points.size()];
        	for (int i = 0; i < points.size(); ++i)
        	{
        		Point3D proj = depthGen.convertRealWorldToProjective(points.get(i));
        		xPoints[i] = (int)proj.getX();
        		yPoints[i] = (int)proj.getY();
        	}
            g.drawPolyline(xPoints, yPoints, points.size());
    		Point3D proj = depthGen.convertRealWorldToProjective(points.get(points.size()-1));
            g.drawArc((int)proj.getX(), (int)proj.getY(), 5, 5, 0, 360);
        	} catch (StatusException e)
        	{
        		e.printStackTrace();
        	}
        }
        
    }
}

