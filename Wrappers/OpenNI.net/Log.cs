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
			int status = SafeNativeMethods.xnLogInitSystem();
			WrapperUtils.ThrowOnError(status);
		}

		public static void InitFromXmlFile(string xmlFile)
		{
			int status = SafeNativeMethods.xnLogInitFromXmlFile(xmlFile);
			WrapperUtils.ThrowOnError(status);
		}

		public static void Close()
		{
			int status = SafeNativeMethods.xnLogClose();
			WrapperUtils.ThrowOnError(status);
		}

		public static void SetMaskState(string maskName, bool on)
		{
			int status = SafeNativeMethods.xnLogSetMaskState(maskName, on);
			WrapperUtils.ThrowOnError(status);
		}

		public static void SetSeverityFilter(LogSeverity severity)
		{
			int status = SafeNativeMethods.xnLogSetSeverityFilter(severity);
			WrapperUtils.ThrowOnError(status);
		}

		public static void SetConsoleOutput(bool on)
		{
			int status = SafeNativeMethods.xnLogSetConsoleOutput(on);
			WrapperUtils.ThrowOnError(status);
		}

		public static void SetFileOutput(bool on)
		{
			int status = SafeNativeMethods.xnLogSetFileOutput(on);
			WrapperUtils.ThrowOnError(status);
		}

		public static void SetOutputFolder(string folder)
		{
			int status = SafeNativeMethods.xnLogSetOutputFolder(folder);
			WrapperUtils.ThrowOnError(status);
		}
	}
}