using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using OpenNI;
using System.Threading;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;

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
            this.skeletonCapability = new SkeletonCapability(this.userGenerator);
            this.poseDetectionCapability = new PoseDetectionCapability(this.userGenerator);
            this.calibPose = this.skeletonCapability.GetCalibrationPose();

            this.userGenerator.UserFound += new EventHandler<UserFoundArgs>(userGenerator_UserFound);
            this.userGenerator.UserLost += new EventHandler<UserLostArgs>(userGenerator_LostUser);
            this.poseDetectionCapability.PoseDetected += new EventHandler<PoseDetectionArgs>(poseDetectionCapability_PoseDetected);
            this.skeletonCapability.CalibrationEnded += new EventHandler<CalibrationEndedArgs>(skeletonCapability_CalibrationEnded);

            this.skeletonCapability.SetSkeletonProfile(SkeletonProfile.All);
            this.joints = new Dictionary<uint,Dictionary<SkeletonJoint,SkeletonJointPosition>>();
            this.userGenerator.StartGenerating();


			this.histogram = new int[this.depth.GetDeviceMaxDepth()];

			MapOutputMode mapMode = this.depth.MapOutputMode;

			this.bitmap = new Bitmap((int)mapMode.XRes, (int)mapMode.YRes/*, System.Drawing.Imaging.PixelFormat.Format24bppRgb*/);
			this.shouldRun = true;
			this.readerThread = new Thread(ReaderThread);
			this.readerThread.Start();
		}

        void skeletonCapability_CalibrationEnded(object sender, CalibrationEndedArgs args)
        {
            if (args.Success)
            {
                this.skeletonCapability.StartTracking(args.UserID);
                this.joints.Add(args.UserID, new Dictionary<SkeletonJoint, SkeletonJointPosition>());
            }
            else
            {
                this.poseDetectionCapability.StartPoseDetection(calibPose, args.UserID);
            }
        }

        void poseDetectionCapability_PoseDetected(object sender, PoseDetectionArgs args)
        {
            this.poseDetectionCapability.StopPoseDetection(args.UserID);
            this.skeletonCapability.RequestCalibration(args.UserID, true);
        }

        void userGenerator_UserFound(object sender, UserFoundArgs args)
        {
            this.poseDetectionCapability.StartPoseDetection(this.calibPose, args.UserID);
        }

        void userGenerator_LostUser(object sender, UserLostArgs args)
        {
            this.joints.Remove(args.UserID);
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

		private unsafe void CalcHist(DepthMetaData depthMD)
		{
			// reset
			for (int i = 0; i < this.histogram.Length; ++i)
				this.histogram[i] = 0;

			ushort* pDepth = (ushort*)depthMD.DepthMapPtr.ToPointer();

			int points = 0;
			for (int y = 0; y < depthMD.YRes; ++y)
			{
				for (int x = 0; x < depthMD.XRes; ++x, ++pDepth)
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
            SkeletonJointPosition pos = new SkeletonJointPosition();
            this.skeletonCapability.GetSkeletonJointPosition(user, joint, ref pos);
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
			DepthMetaData depthMD = new DepthMetaData();

			while (this.shouldRun)
			{
				try
				{
					this.context.WaitOneUpdateAll(this.depth);
				}
				catch (Exception)
				{
				}

				this.depth.GetMetaData(depthMD);

				CalcHist(depthMD);

				lock (this)
				{
					Rectangle rect = new Rectangle(0, 0, this.bitmap.Width, this.bitmap.Height);
					BitmapData data = this.bitmap.LockBits(rect, ImageLockMode.WriteOnly, System.Drawing.Imaging.PixelFormat.Format24bppRgb);


                    if (this.shouldDrawPixels)
                    {
                        ushort* pDepth = (ushort*)this.depth.GetDepthMapPtr().ToPointer();
                        ushort* pLabels = (ushort*)this.userGenerator.GetUserPixels(0).SceneMapPtr.ToPointer();

                        // set pixels
                        for (int y = 0; y < depthMD.YRes; ++y)
                        {
                            byte* pDest = (byte*)data.Scan0.ToPointer() + y * data.Stride;
                            for (int x = 0; x < depthMD.XRes; ++x, ++pDepth, ++pLabels, pDest += 3)
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
                            Point3D com = this.userGenerator.GetCoM(user);
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

		private Context context;
		private DepthGenerator depth;
        private UserGenerator userGenerator;
        private SkeletonCapability skeletonCapability;
        private PoseDetectionCapability poseDetectionCapability;
        private string calibPose;
		private Thread readerThread;
		private bool shouldRun;
		private Bitmap bitmap;
		private int[] histogram;

        private Dictionary<uint, Dictionary<SkeletonJoint, SkeletonJointPosition>> joints;

        private bool shouldDrawPixels = true;
        private bool shouldDrawBackground = true;
        private bool shouldPrintID = true;
        private bool shouldPrintState = true;
        private bool shouldDrawSkeleton = true;
	}
}
