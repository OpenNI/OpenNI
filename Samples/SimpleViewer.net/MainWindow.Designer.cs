namespace SimpleViewer.net
{
	partial class MainWindow
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.panelView = new System.Windows.Forms.Panel();
			this.SuspendLayout();
			// 
			// panelView
			// 
			this.panelView.Dock = System.Windows.Forms.DockStyle.Fill;
			this.panelView.Location = new System.Drawing.Point(0, 0);
			this.panelView.Name = "panelView";
			this.panelView.Size = new System.Drawing.Size(602, 439);
			this.panelView.TabIndex = 0;
			this.panelView.Visible = false;
			// 
			// MainWindow
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(602, 439);
			this.Controls.Add(this.panelView);
			this.Name = "MainWindow";
			this.Text = "OpenNI Simple Viewer.net";
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Panel panelView;
	}
}

