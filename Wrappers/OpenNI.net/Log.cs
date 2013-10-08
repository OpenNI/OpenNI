/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
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