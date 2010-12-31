using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace OpenNI
{
	public static class Log
	{
		public static readonly string MaskAll = "ALL";

		public static void Init()
		{
			Status.ThrowOnFail(SafeNativeMethods.xnLogInitSystem());
		}

		public static void InitFromXmlFile(string xmlFile)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnLogInitFromXmlFile(xmlFile));
		}

		public static void Close()
		{
			Status.ThrowOnFail(SafeNativeMethods.xnLogClose());
		}

		public static void SetMaskState(string maskName, bool on)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnLogSetMaskState(maskName, on));
		}

		public static void SetSeverityFilter(LogSeverity severity)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnLogSetSeverityFilter(severity));
		}

		public static void SetConsoleOutput(bool on)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnLogSetConsoleOutput(on));
		}

		public static void SetFileOutput(bool on)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnLogSetFileOutput(on));
		}

		public static void SetOutputFolder(string folder)
		{
			Status.ThrowOnFail(SafeNativeMethods.xnLogSetOutputFolder(folder));
		}
	}
}