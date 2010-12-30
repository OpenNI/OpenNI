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
			UInt32 status = OpenNIImporter.xnLogInitSystem();
			WrapperUtils.CheckStatus(status);
		}

		public static void InitFromXmlFile(string xmlFile)
		{
			UInt32 status = OpenNIImporter.xnLogInitFromXmlFile(xmlFile);
			WrapperUtils.CheckStatus(status);
		}

		public static void Close()
		{
			UInt32 status = OpenNIImporter.xnLogClose();
			WrapperUtils.CheckStatus(status);
		}

		public static void SetMaskState(string maskName, bool on)
		{
			UInt32 status = OpenNIImporter.xnLogSetMaskState(maskName, on);
			WrapperUtils.CheckStatus(status);
		}

		public static void SetSeverityFilter(LogSeverity severity)
		{
			UInt32 status = OpenNIImporter.xnLogSetSeverityFilter(severity);
			WrapperUtils.CheckStatus(status);
		}

		public static void SetConsoleOutput(bool on)
		{
			UInt32 status = OpenNIImporter.xnLogSetConsoleOutput(on);
			WrapperUtils.CheckStatus(status);
		}

		public static void SetFileOutput(bool on)
		{
			UInt32 status = OpenNIImporter.xnLogSetFileOutput(on);
			WrapperUtils.CheckStatus(status);
		}

		public static void SetOutputFolder(string folder)
		{
			UInt32 status = OpenNIImporter.xnLogSetOutputFolder(folder);
			WrapperUtils.CheckStatus(status);
		}
	}
}