package org.OpenNI.Samples.Assistant;

import java.util.ArrayList;
import java.util.List;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Matrix;
import android.graphics.Paint;
import android.view.View;

public class Screen extends View {

	abstract class Shape
	{
		protected Paint paint;
		public abstract void draw(Canvas canvas);
	}

	class Line extends Shape
	{
		public Line(float x0, float y0, float x1, float y1, int color) 
		{
			this.x0 = x0;
			this.y0 = y0;
			this.x1 = x1;
			this.y1 = y1;
			paint = new Paint();
			paint.setStrokeWidth(4);
			paint.setColor(color);
		}
		public void draw(Canvas canvas)
		{
			canvas.drawLine(x0*magRatio, y0*magRatio, x1*magRatio, y1*magRatio, paint);
		}
		
		private float x0,y0,x1,y1;
	}
	
	class Label extends Shape
	{
		public static final float FONT_SIZE = 12; 
		public Label(String text, float x, float y, int color) 
		{
			this.x = x;
			this.y = y;
			this.text = text;
			paint = new Paint();
			paint.setTextSize(FONT_SIZE * magRatio);
			paint.setColor(color);
		}
		public void draw(Canvas canvas)
		{
			canvas.drawText(text, x*magRatio, y*magRatio, paint);
		}
		
		private float x, y;
		private String text;
	}
	
	class Circle extends Shape
	{
		public Circle(float x, float y, float radius, int color) 
		{
			this.x = x;
			this.y = y;
			this.radius = radius;
			paint = new Paint();
			paint.setColor(color);
		}
		public void draw(Canvas canvas)
		{
			canvas.drawCircle(x* magRatio, y* magRatio, radius* magRatio, paint);
		}

		private float x, y, radius;
	}
	
	class Frame extends Shape
	{
		public Frame(float left, float top, float width, float height, int color) 
		{
			this.x0 = left;
			this.y0 = top;
			this.x1 = left + width;
			this.y1 = top + height;
					
			paint = new Paint();
			paint.setStrokeWidth(4);
			paint.setColor(color);
		}
		public void draw(Canvas canvas)
		{
			float[] arr = {
					x0,y0,x1,y0,
					x1,y0,x1,y1,
					x1,y1,x0,y1,
					x0,y1,x0,y0,
					};
			canvas.drawLines(arr , 0, 16, paint);
		}
		
		private float x0,y0,x1,y1;
	}

	private Object theLock = new Object();
	private List<Shape> shapes2Draw;
	private List<Shape> shapes2Prepare;
	
	private int curColor = Color.WHITE;
	
	private int inputWidth = 320, inputHeight = 240;
	private float magRatio = 1;
	
	public boolean drawBorders = false;

	
	public Screen(Context context) {
		super(context);
		shapes2Draw = new ArrayList<Shape>();
		shapes2Prepare = new ArrayList<Shape>();
		
		FPS = new FPSCalculator();
		
		bitmap = BitmapFactory.decodeResource(getResources(),R.drawable.android_skeleton);
	}
	
	public void setDimensions(int width, int height)
	{
		inputWidth = width;
		inputHeight = height;
	}
	
	@Override
	protected void onMeasure(int widthMeasureSpec, int heightMeasureSpec) {
		// TODO Auto-generated method stub
		super.onMeasure(widthMeasureSpec, heightMeasureSpec);
		float w,h;
		w = View.MeasureSpec.getSize(widthMeasureSpec);
		h = View.MeasureSpec.getSize(heightMeasureSpec);
		magRatio = Math.min(w / inputWidth, h / inputHeight);
	}
	
	@Override
	protected void onDraw(Canvas canvas) 
	{	
		Matrix matrix = new Matrix();
		matrix.setScale(magRatio, magRatio);
		
		if (bitmap != null) {
			canvas.drawBitmap(bitmap,matrix, null);
		}
		
		synchronized (theLock) {
			for (Shape shape : shapes2Draw) {
				shape.draw(canvas);
			}
		}
		
		if(drawBorders)
		{
			Frame frame = new Frame(0, 0, inputWidth * magRatio, inputHeight * magRatio, Color.YELLOW);
			frame.draw(canvas);
		}
	}
	
	
	public Bitmap getBitmap() {
		return bitmap;
	}

	public void setBitmap(Bitmap bitmap) {
		this.bitmap = bitmap;
		this.postInvalidate();
	}
	
	public void drawLine(float x0, float y0, float x1, float y1)
	{
		shapes2Prepare.add(new Line(x0, y0, x1, y1, curColor));
	}
	
	public void drawLabel(String text, float x, float y)
	{
		shapes2Prepare.add(new Label(text, x, y, curColor));
	}
	
	public void drawCircle(float x, float y, float radius)
	{
		shapes2Prepare.add(new Circle(x, y, radius, curColor));
	}
	
	public void setColor(int color)
	{
		curColor = color;
	}
	
	public void redraw()
	{
		FPS.update();
		if(showFPS)
		{
			setColor(Color.WHITE);
			drawLabel(String.format("%.1f FPS", FPS.calcAvgFPS()),   2*magRatio, (float) (Label.FONT_SIZE*magRatio*0.35));
			drawLabel(String.format("%.1f ms", FPS.calcAvgMillis()), 2*magRatio, (float) (Label.FONT_SIZE*magRatio*0.7));
		}
        
		synchronized (theLock) {
			shapes2Draw.clear();
			shapes2Draw.addAll(shapes2Prepare);
			shapes2Prepare.clear();
		}
		this.postInvalidate();
	}

	
	private Bitmap bitmap;

	private FPSCalculator FPS;
	private boolean showFPS = false;
	
	public FPSCalculator getFPSCalculator() {
		return FPS;
	}

	public void showFPS(boolean show) {
		showFPS = show;
	}
}
