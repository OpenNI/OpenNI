package org.OpenNI.Samples.HandTracker;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;

public class HandTrackerApplication {

    /**
	 * 
	 */
	public HandTracker viewer;
	private boolean shouldRun = true;
	private JFrame frame;

    public HandTrackerApplication (JFrame frame)
    {
    	this.frame = frame;
    	frame.addKeyListener(new KeyListener()
		{
			@Override
			public void keyTyped(KeyEvent arg0) {}
			@Override
			public void keyReleased(KeyEvent arg0) {}
			@Override
			public void keyPressed(KeyEvent arg0) {
				if (arg0.getKeyCode() == KeyEvent.VK_ESCAPE)
				{
					shouldRun = false;
				}
			}
		});
    }

    public void buildUI()
    {
        if (viewer == null)
        {
            viewer = new HandTracker();
        }
        viewer.updateDepth();
        viewer.repaint();
    }

    public static void main(String s[])
    {
        JFrame f = new JFrame("OpenNI Hand Tracker");
        f.addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {System.exit(0);}
        });
        HandTrackerApplication app = new HandTrackerApplication(f);
        
        app.viewer = new HandTracker();
        f.add("Center", app.viewer);
        f.pack();
        f.setVisible(true);
        app.run();
    }

    void run()
    {
        while(shouldRun) {
            viewer.updateDepth();
            viewer.repaint();
        }
        frame.dispose();
    }
    
}
