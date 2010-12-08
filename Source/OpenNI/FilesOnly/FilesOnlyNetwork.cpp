/*****************************************************************************
*                                                                            *
*  OpenNI 1.0 Alpha                                                          *
*  Copyright (C) 2010 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  OpenNI is free software: you can redistribute it and/or modify            *
*  it under the terms of the GNU Lesser General Public License as published  *
*  by the Free Software Foundation, either version 3 of the License, or      *
*  (at your option) any later version.                                       *
*                                                                            *
*  OpenNI is distributed in the hope that it will be useful,                 *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              *
*  GNU Lesser General Public License for more details.                       *
*                                                                            *
*  You should have received a copy of the GNU Lesser General Public License  *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.            *
*                                                                            *
*****************************************************************************/




//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_CORE_API XnStatus XnOSInitNetwork()
{
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSShutdownNetwork()
{
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSCreateSocket(const XnOSSocketType SocketType, const XN_CHAR* cpIPAddress, const XN_UINT16 nPort, XN_SOCKET_HANDLE* SocketPtr)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSCloseSocket(XN_SOCKET_HANDLE Socket)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSBindSocket(XN_SOCKET_HANDLE Socket)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSListenSocket(XN_SOCKET_HANDLE Socket)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSAcceptSocket(XN_SOCKET_HANDLE ListenSocket, XN_SOCKET_HANDLE* AcceptSocketPtr)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSConnectSocket(XN_SOCKET_HANDLE Socket)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSSetSocketBufferSize(XN_SOCKET_HANDLE Socket, const XN_UINT32 nSocketBufferSize)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSSendNetworkBuffer(XN_SOCKET_HANDLE Socket, const XN_CHAR* cpBuffer, const XN_UINT32 nBufferSize)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSSendToNetworkBuffer(XN_SOCKET_HANDLE Socket, const XN_CHAR* cpBuffer, const XN_UINT32 nBufferSize, XN_SOCKET_HANDLE SocketTo)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSReceiveNetworkBuffer(XN_SOCKET_HANDLE Socket, XN_CHAR* cpBuffer, XN_UINT32* pnBufferSize, XN_UINT32 nMicroSecondTimeout)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_CORE_API XnStatus XnOSReceiveFromNetworkBuffer(XN_SOCKET_HANDLE Socket, XN_CHAR* cpBuffer, XN_UINT32* pnBufferSize, XN_SOCKET_HANDLE* SocketFrom)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}
