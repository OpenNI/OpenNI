﻿using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using OpenNI;
using System.Threading;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Linq;

namespace UserTracker.net
{
	public partial class MainWindow : Form
	{
		public MainWindow()
		{
			InitializeComponent();

			this.context = Context.InitFromXml(SAMPLE_XML_FILE);
			this.depth = context.FindExistingNode(NodeType.Depth) as DepthGenerator;
			if (this.depth == null)
			{
				throw new Exception("Viewer must have a depth node!");
			}

            this.userGenerator = new UserGenerator(this.context);
            this.skeletonCapability = this.userGenerator.GetSkeletonCap();
            this.poseDetectionCapability = this.userGenerator.GetPoseDetectionCap();
            this.calibPose = this.skeletonCapability.CalibrationPose;

            //this.userGenerator.UserFound += new EventHandler<UserFoundArgs>(userGenerator_UserFound);
            //this.userGenerator.UserLost += new EventHandler<UserLostArgs>(userGenerator_LostUser);
            //this.poseDetectionCapability.PoseDetected += new EventHandler<PoseDetectionArgs>(poseDetectionCapability_PoseDetected);
            //this.skeletonCapability.CalibrationEnded += new EventHandler<CalibrationEndedArgs>(skeletonCapability_CalibrationEnded);

            // create observables for the events
            var userFound = Observable.FromEvent<EventHandler<UserFoundArgs>, UserFoundArgs>(d => d.Invoke, h => this.userGenerator.UserFound += h, h => this.userGenerator.UserFound -= h);
            var userLost = Observable.FromEvent<EventHandler<UserLostArgs>, UserLostArgs>(d => d.Invoke, h => this.userGenerator.UserLost += h, h => this.userGenerator.UserLost -= h);
            var poseDetected = Observable.FromEvent<EventHandler<PoseDetectionArgs>, PoseDetectionArgs>(d => d.Invoke, h => this.poseDetectionCapability.PoseDetected += h, h => this.poseDetectionCapability.PoseDetected -= h);
            var calibrationEnded = Observable.FromEvent<EventHandler<CalibrationEndedArgs>, CalibrationEndedArgs>(d => d.Invoke, h => this.skeletonCapability.CalibrationEnded += h, h => this.skeletonCapability.CalibrationEnded -= h);

            // retrieve the event values
            var userFoundValues = userFound
                .Select(e => e.EventArgs);
            var userLostValues = userLost
                .Select(e => e.EventArgs);
            var poseDetectedValues = poseDetected
                .Select(e => e.EventArgs);
            var calibrationEndedValues = calibrationEnded
                .Select(e => e.EventArgs);

            // handle the observables
            userFoundValues.Subscribe(
                args => this.poseDetectionCapability.StartPoseDetection(this.calibPose, args.UserId));
            userLostValues.Subscribe(
                args => this.joints.Remove(args.UserId));
            poseDetectedValues.Subscribe(
                delegate(PoseDetectionArgs args)
                {
                    this.poseDetectionCapability.StopPoseDetection(args.UserId);
                    this.skeletonCapability.RequestCalibration(args.UserId, true);
                });
            calibrationEndedValues.Subscribe(
                delegate(CalibrationEndedArgs args)
                {
                    if (args.Success)
                    {
                        this.skeletonCapability.StartTracking(args.UserId);
                        this.joints.Add(args.UserId, new Dictionary<SkeletonJoint, SkeletonJointPosition>());
                    }
                    else
                    {
                        this.poseDetectionCapability.StartPoseDetection(calibPose, args.UserId);
                    }
                });
                

            this.skeletonCapability.SetSkeletonProfile(SkeletonProfile.All);
            this.joints = new Dictionary<uint,Dictionary<SkeletonJoint,SkeletonJointPosition>>();
            this.userGenerator.StartGenerating();

			this.histogram = new int[this.depth.DeviceMaxDepth];

			MapOutputMode mapMode = this.depth.MapOutputMode;

			this.bitmap = new Bitmap((int)mapMode.XRes, (int)mapMode.YRes/*, System.Drawing.Imaging.PixelFormat.Format24bppRgb*/);
			this.shouldRun = true;
			this.readerThread = new Thread(ReaderThread);
			this.readerThread.Start();
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint(e);

			lock (this)
			{
				e.Graphics.DrawImage(this.bitmap,
					this.panelView.Location.X,
					this.panelView.Location.Y,
					this.panelView.Size.Width,
					this.panelView.Size.Height);
            }
		}

		protected override void OnPaintBackground(PaintEventArgs pevent)
		{
			//Don't allow the background to paint
		}

		protected override void OnClosing(CancelEventArgs e)
		{
			this.shouldRun = false;
			this.readerThread.Join();
			base.OnClosing(e);
		}

		protected override void OnKeyPress(KeyPressEventArgs e)
		{
			if (e.KeyChar == 27)
			{
				Close();
			}
            switch (e.KeyChar)
            {
            case (char)27:
            	break;
            case 'b':
                this.shouldDrawBackground = !this.shouldDrawBackground;
                break;
            case 'x':
                this.shouldDrawPixels = !this.shouldDrawPixels;
                break;
            case 's':
                this.shouldDrawSkeleton = !this.shouldDrawSkeleton;
                break;
            case 'i':
                this.shouldPrintID = !this.shouldPrintID;
                break;
            case 'l':
                this.shouldPrintState = !this.shouldPrintState;
                break;

            }
			base.OnKeyPress(e);
		}

		private unsafe void CalcHist(DepthMetadata depthMetadata)
		{
			// reset
			for (int i = 0; i < this.histogram.Length; ++i)
				this.histogram[i] = 0;

			ushort* pDepth = (ushort*)depthMetadata.DepthMapPtr.ToPointer();

			int points = 0;
			for (int y = 0; y < depthMetadata.YRes; ++y)
			{
				for (int x = 0; x < depthMetadata.XRes; ++x, ++pDepth)
				{
					ushort depthVal = *pDepth;
					if (depthVal != 0)
					{
						this.histogram[depthVal]++;
						points++;
					}
				}
			}

			for (int i = 1; i < this.histogram.Length; i++)
			{
				this.histogram[i] += this.histogram[i-1];
			}

			if (points > 0)
			{
				for (int i = 1; i < this.histogram.Length; i++)
				{
					this.histogram[i] = (int)(256 * (1.0f - (this.histogram[i] / (float)points)));
				}
			}
		}

        private Color[] colors = { Color.Red, Color.Blue, Color.ForestGreen, Color.Yellow, Color.Orange, Color.Purple, Color.White };
        private Color[] anticolors = { Color.Green, Color.Orange, Color.Red, Color.Purple, Color.Blue, Color.Yellow, Color.Black };
        private int ncolors = 6;

        private void GetJoint(uint user, SkeletonJoint joint)
        {
            SkeletonJointPosition pos = this.skeletonCapability.GetSkeletonJointPosition(user, joint);
			if (pos.Position.Z == 0)
			{
				pos.Confidence = 0;
			}
			else
			{
				pos.Position = this.depth.ConvertRealWorldToProjective(pos.Position);
			}
			this.joints[user][joint] = pos;
        }

        private void GetJoints(uint user)
        {
            GetJoint(user, SkeletonJoint.Head);
            GetJoint(user, SkeletonJoint.Neck);

            GetJoint(user, SkeletonJoint.LeftShoulder);
            GetJoint(user, SkeletonJoint.LeftElbow);
            GetJoint(user, SkeletonJoint.LeftHand);

            GetJoint(user, SkeletonJoint.RightShoulder);
            GetJoint(user, SkeletonJoint.RightElbow);
            GetJoint(user, SkeletonJoint.RightHand);

            GetJoint(user, SkeletonJoint.Torso);

            GetJoint(user, SkeletonJoint.LeftHip);
            GetJoint(user, SkeletonJoint.LeftKnee);
            GetJoint(user, SkeletonJoint.LeftFoot);

            GetJoint(user, SkeletonJoint.RightHip);
            GetJoint(user, SkeletonJoint.RightKnee);
            GetJoint(user, SkeletonJoint.RightFoot);
        }

        private void DrawLine(Graphics g, Color color, Dictionary<SkeletonJoint, SkeletonJointPosition> dict, SkeletonJoint j1, SkeletonJoint j2)
        {
			Point3D pos1 = dict[j1].Position;
			Point3D pos2 = dict[j2].Position;

			if (dict[j1].Confidence == 0 || dict[j2].Confidence == 0)
				return;

            g.DrawLine(new Pen(color),
						new Point((int)pos1.X, (int)pos1.Y),
                        new Point((int)pos2.X, (int)pos2.Y));

        }
        private void DrawSkeleton(Graphics g, Color color, uint user)
        {
            GetJoints(user);
            Dictionary<SkeletonJoint, SkeletonJointPosition> dict = this.joints[user];

            DrawLine(g, color, dict, SkeletonJoint.Head, SkeletonJoint.Neck);

            DrawLine(g, color, dict, SkeletonJoint.LeftShoulder, SkeletonJoint.Torso);
            DrawLine(g, color, dict, SkeletonJoint.RightShoulder, SkeletonJoint.Torso);

            DrawLine(g, color, dict, SkeletonJoint.Neck, SkeletonJoint.LeftShoulder);
            DrawLine(g, color, dict, SkeletonJoint.LeftShoulder, SkeletonJoint.LeftElbow);
            DrawLine(g, color, dict, SkeletonJoint.LeftElbow, SkeletonJoint.LeftHand);

            DrawLine(g, color, dict, SkeletonJoint.Neck, SkeletonJoint.RightShoulder);
            DrawLine(g, color, dict, SkeletonJoint.RightShoulder, SkeletonJoint.RightElbow);
            DrawLine(g, color, dict, SkeletonJoint.RightElbow, SkeletonJoint.RightHand);

            DrawLine(g, color, dict, SkeletonJoint.LeftHip, SkeletonJoint.Torso);
            DrawLine(g, color, dict, SkeletonJoint.RightHip, SkeletonJoint.Torso);
            DrawLine(g, color, dict, SkeletonJoint.LeftHip, SkeletonJoint.RightHip);

            DrawLine(g, color, dict, SkeletonJoint.LeftHip, SkeletonJoint.LeftKnee);
            DrawLine(g, color, dict, SkeletonJoint.LeftKnee, SkeletonJoint.LeftFoot);

            DrawLine(g, color, dict, SkeletonJoint.RightHip, SkeletonJoint.RightKnee);
            DrawLine(g, color, dict, SkeletonJoint.RightKnee, SkeletonJoint.RightFoot);
        }

		private unsafe void ReaderThread()
		{
			DepthMetadata depthMetadata = new DepthMetadata();

			while (this.shouldRun)
			{
				try
				{
					this.context.WaitOneUpdateAll(this.depth);
				}
				catch (Exception)
				{
				}

				this.depth.GetMetadata(depthMetadata);

				CalcHist(depthMetadata);

				lock (this)
				{
					Rectangle rect = new Rectangle(0, 0, this.bitmap.Width, this.bitmap.Height);
					BitmapData data = this.bitmap.LockBits(rect, ImageLockMode.WriteOnly, System.Drawing.Imaging.PixelFormat.Format24bppRgb);


                    if (this.shouldDrawPixels)
                    {
                        ushort* pDepth = (ushort*)this.depth.GetDepthMapPtr().ToPointer();
                        ushort* pLabels = (ushort*)this.userGenerator.GetUserPixels(0).SceneMapPtr.ToPointer();

                        // set pixels
                        for (int y = 0; y < depthMetadata.YRes; ++y)
                        {
                            byte* pDest = (byte*)data.Scan0.ToPointer() + y * data.Stride;
                            for (int x = 0; x < depthMetadata.XRes; ++x, ++pDepth, ++pLabels, pDest += 3)
                            {
                                pDest[0] = pDest[1] = pDest[2] = 0;

                                ushort label = *pLabels;
                                if (this.shouldDrawBackground || *pLabels != 0)
                                {
                                    Color labelColor = Color.White;
                                    if (label != 0)
                                    {
                                        labelColor = colors[label % ncolors];
                                    }

                                    byte pixel = (byte)this.histogram[*pDepth];
                                    pDest[0] = (byte)(pixel * (labelColor.B / 256.0));
                                    pDest[1] = (byte)(pixel * (labelColor.G / 256.0));
                                    pDest[2] = (byte)(pixel * (labelColor.R / 256.0));
                                }
                            }
                        }
                    }
                    this.bitmap.UnlockBits(data);

                    Graphics g = Graphics.FromImage(this.bitmap);
                    uint[] users = this.userGenerator.GetUsers();
                    foreach (uint user in users)
                    {
                        if (this.shouldPrintID)
                        {
                            Point3D com = this.userGenerator.GetCenterOfMass(user);
                            com = this.depth.ConvertRealWorldToProjective(com);

                            string label = "";
                            if (!this.shouldPrintState)
                                label += user;
                            else if (this.skeletonCapability.IsTracking(user))
                                label += user + " - Tracking";
                            else if (this.skeletonCapability.IsCalibrating(user))
                                label += user + " - Calibrating...";
                            else
                                label += user + " - Looking for pose";

                            g.DrawString(label, new Font("Arial", 6), new SolidBrush(anticolors[user % ncolors]), com.X, com.Y);

                        }

                        if (this.shouldDrawSkeleton && this.skeletonCapability.IsTracking(user))
//                        if (this.skeletonCapability.IsTracking(user))
                            DrawSkeleton(g, anticolors[user % ncolors], user);

                    }
                    g.Dispose();
				}

				this.Invalidate();
			}
		}

		private readonly string SAMPLE_XML_FILE = @"../../../../Data/SamplesConfig.xml";

		private readonly Context context;
        private readonly DepthGenerator depth;
        private readonly UserGenerator userGenerator;
        private readonly SkeletonCapability skeletonCapability;
        private readonly PoseDetectionCapability poseDetectionCapability;
        private string calibPose;
        private readonly Thread readerThread;
		private bool shouldRun;
        private readonly Bitmap bitmap;
        private readonly int[] histogram;

        private Dictionary<uint, Dictionary<SkeletonJoint, SkeletonJointPosition>> joints;

        private bool shouldDrawPixels = true;
        private bool shouldDrawBackground = true;
        private bool shouldPrintID = true;
        private bool shouldPrintState = true;
        private bool shouldDrawSkeleton = true;
	}
}
