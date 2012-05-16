/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
package org.OpenNI.Samples.SimpleViewer;

import org.OpenNI.*;

import java.nio.ShortBuffer;
import java.awt.*;
import java.awt.image.*;

class SimpleViewer extends Component {

    /**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private OutArg<ScriptNode> scriptNode;
    private Context context;
    private DepthGenerator depthGen;
    private byte[] imgbytes;
    private float histogram[];

    private BufferedImage bimg;
    int width, height;

    private final String SAMPLE_XML_FILE = "../../../../Data/SamplesConfig.xml";    
    public SimpleViewer() {

        try {
            scriptNode = new OutArg<ScriptNode>();
            context = Context.createFromXmlFile(SAMPLE_XML_FILE, scriptNode);

            depthGen = DepthGenerator.create(context);
            DepthMetaData depthMD = depthGen.getMetaData();

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
    
    private void calcHist(DepthMetaData depthMD)
    {
        // reset
        for (int i = 0; i < histogram.length; ++i)
            histogram[i] = 0;
        
        ShortBuffer depth = depthMD.getData().createShortBuffer();
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
            
            calcHist(depthMD);
            ShortBuffer depth = depthMD.getData().createShortBuffer();
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

    public void paint(Graphics g) {
        DataBufferByte dataBuffer = new DataBufferByte(imgbytes, width*height);
        Raster raster = Raster.createPackedRaster(dataBuffer, width, height, 8, null);
        bimg.setData(raster);

        g.drawImage(bimg, 0, 0, null);
    }
}

