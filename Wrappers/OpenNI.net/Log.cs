/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
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