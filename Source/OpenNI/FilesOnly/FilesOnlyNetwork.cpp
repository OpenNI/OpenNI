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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSInitNetwork()
{
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSShutdownNetwork()
{
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateSocket(const XnOSSocketType SocketType, const XnChar* cpIPAddress, const XnUInt16 nPort, XN_SOCKET_HANDLE* SocketPtr)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseSocket(XN_SOCKET_HANDLE Socket)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSBindSocket(XN_SOCKET_HANDLE Socket)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSListenSocket(XN_SOCKET_HANDLE Socket)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSAcceptSocket(XN_SOCKET_HANDLE ListenSocket, XN_SOCKET_HANDLE* AcceptSocketPtr, XnUInt32 nMillisecsTimeout)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSConnectSocket(XN_SOCKET_HANDLE Socket, XnUInt32 nMillisecsTimeout)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetSocketBufferSize(XN_SOCKET_HANDLE Socket, const XnUInt32 nSocketBufferSize)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSendNetworkBuffer(XN_SOCKET_HANDLE Socket, const XnChar* cpBuffer, const XnUInt32 nBufferSize)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSendToNetworkBuffer(XN_SOCKET_HANDLE Socket, const XnChar* cpBuffer, const XnUInt32 nBufferSize, XN_SOCKET_HANDLE SocketTo)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReceiveNetworkBuffer(XN_SOCKET_HANDLE Socket, XnChar* cpBuffer, XnUInt32* pnBufferSize, XnUInt32 nMicroSecondTimeout)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReceiveFromNetworkBuffer(XN_SOCKET_HANDLE Socket, XnChar* cpBuffer, XnUInt32* pnBufferSize, XN_SOCKET_HANDLE* SocketFrom)
{
	XN_IMPLEMENT_OS;
	return (XN_STATUS_OK);
}
