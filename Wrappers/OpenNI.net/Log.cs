using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public static class Log
	{
		public static readonly string MASK_ALL = "ALL";

		public static void Init()
		{
			Status.ThrowOnFail(OpenNIImporter.xnLogInitSystem());
		}

		public static void InitFromXmlFile(string xmlFile)
		{
			Status.ThrowOnFail(OpenNIImporter.xnLogInitFromXmlFile(xmlFile));
		}

		public static void Close()
		{
			Status.ThrowOnFail(OpenNIImporter.xnLogClose());
		}

		public static void SetMaskState(string maskName, bool on)
		{
			Status.ThrowOnFail(OpenNIImporter.xnLogSetMaskState(maskName, on));
		}

		public static void SetSeverityFilter(LogSeverity severity)
		{
			Status.ThrowOnFail(OpenNIImporter.xnLogSetSeverityFilter(severity));
		}

		public static void SetConsoleOutput(bool on)
		{
			Status.ThrowOnFail(OpenNIImporter.xnLogSetConsoleOutput(on));
		}

		public static void SetFileOutput(bool on)
		{
			Status.ThrowOnFail(OpenNIImporter.xnLogSetFileOutput(on));
		}

		public static void SetOutputFolder(string folder)
		{
			Status.ThrowOnFail(OpenNIImporter.xnLogSetOutputFolder(folder));
		}
	}
}