using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
using System.Drawing;

namespace SimpleViewer.net
{
	static class Program
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			Application.EnableVisualStyles();
			Application.SetCompatibleTextRenderingDefault(false);
			try
			{
				Application.Run(new MainWindow());
			}
			catch (System.Exception ex)
			{
				MessageBox.Show("Error: " + ex.Message, "SimpleViewer", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}
	}
}
