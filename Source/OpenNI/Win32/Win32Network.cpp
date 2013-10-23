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
//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnOS.h>
#include <winsock2.h>
#include <xnLog.h>

//---------------------------------------------------------------------------
// Global Variables
//---------------------------------------------------------------------------
static WSADATA g_xnOSNetworkWSAData;
static XnBool g_xnOSNetworkWasInit = FALSE;

//---------------------------------------------------------------------------
// Structs
//---------------------------------------------------------------------------
/** The Xiron OS network socket structure. */ 
typedef struct xnOSSocket
{
	/** The OS socket handle. */ 
	SOCKET Socket;

	/** The OS socket address (IP and port). */ 
	SOCKADDR_IN SocketAddress;
	XnInt32 nSocketAddressLen;

	/** The socket type enum (UDP, TDP, etc...) */ 
	XnUInt32 nSocketType;
} xnOSSocket;

//---------------------------------------------------------------------------
// Code
//---------------------------------------------------------------------------
XN_C_API XnStatus xnOSInitNetwork()
{
	// Initialize the WinSock 2.2 subsystem (if needed) and make sure it succeeded (return value is 0)
	if (g_xnOSNetworkWasInit == FALSE)
	{
		XnInt32 nRetVal = WSAStartup(MAKEWORD(2,2), &g_xnOSNetworkWSAData);
		if (nRetVal != NO_ERROR)
		{
			return(XN_STATUS_OS_NETWORK_INIT_FAILED);
		}

		// Set the global network init flag as true
		g_xnOSNetworkWasInit = TRUE;
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSShutdownNetwork()
{
	// Was the network subsystem initialized?
	if (g_xnOSNetworkWasInit == TRUE)
	{
		// Cleanup the WinSock 2.2 subsystem and make sure it succeeded (return value is 0)
		XnInt32 nRetVal = WSACleanup();
		if (nRetVal == SOCKET_ERROR)
		{
			return(XN_STATUS_OS_NETWORK_SHUTDOWN_FAILED);
		}
	}

	// Set the global network init flag as false
	g_xnOSNetworkWasInit = FALSE;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCreateSocket(const XnOSSocketType SocketType, const XnChar* cpIPAddress, const XnUInt16 nPort, XN_SOCKET_HANDLE* SocketPtr)
{
	// Local function variables
	hostent* HostEnt = NULL;
	XN_SOCKET_HANDLE Socket = NULL;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(cpIPAddress);
	XN_VALIDATE_OUTPUT_PTR(SocketPtr);

	XN_VALIDATE_ALIGNED_CALLOC(*SocketPtr, xnOSSocket, 1, XN_DEFAULT_MEM_ALIGN);

	Socket = *SocketPtr;

	if (SocketType == XN_OS_UDP_SOCKET)
	{
		// Create a UDP socket
		Socket->Socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	}
	else if (SocketType == XN_OS_TCP_SOCKET)
	{
		// Create a TCP socket
		Socket->Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	}
	else
	{
		// Unknown socket type...
		XN_ALIGNED_FREE_AND_NULL(Socket);
		return (XN_STATUS_OS_NETWORK_INVALID_SOCKET_TYPE);
	}

	if (Socket->Socket == INVALID_SOCKET)
	{
		XN_ALIGNED_FREE_AND_NULL(Socket);
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_SOCKET_CREATION_FAILED, XN_MASK_OS, "socket() returned WinSock error: %d", WSAGetLastError());
	}

	// Set the socket server address
	Socket->SocketAddress.sin_family = AF_INET;

	if (isalpha(cpIPAddress[0])) 
	{
		HostEnt = gethostbyname(cpIPAddress);
		if (HostEnt == NULL)
		{
			XN_ALIGNED_FREE_AND_NULL(Socket);
			return (XN_STATUS_OS_NETWORK_BAD_HOST_NAME);
		}

		xnOSMemCopy(&Socket->SocketAddress.sin_addr, HostEnt->h_addr, HostEnt->h_length);
	}
	else
	{
		Socket->SocketAddress.sin_addr.s_addr = inet_addr(cpIPAddress);
	}

	Socket->SocketAddress.sin_port = htons(nPort);

	// Update socket address size
	Socket->nSocketAddressLen = sizeof(Socket->SocketAddress);

	// Remember the socket type
	Socket->nSocketType = SocketType;

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSCloseSocket(XN_SOCKET_HANDLE Socket)
{
	// Local function variables
	XnInt32 nRetVal = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_NULL(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Close the socket and make sure it succeeded (return value is 0)
	nRetVal = closesocket(Socket->Socket);
	if (nRetVal == SOCKET_ERROR)
	{
		return(XN_STATUS_OS_NETWORK_SHUTDOWN_FAILED);
	}

	XN_ALIGNED_FREE_AND_NULL(Socket);

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSBindSocket(XN_SOCKET_HANDLE Socket)
{
	// Local function variables
	XnInt32 nRetVal = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_NULL(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Bind the socket and make sure it succeeded
	nRetVal = bind(Socket->Socket, (SOCKADDR*)&Socket->SocketAddress, sizeof(Socket->SocketAddress));
	if (nRetVal	== SOCKET_ERROR)
	{
		return(XN_STATUS_OS_NETWORK_SOCKET_BIND_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSListenSocket(XN_SOCKET_HANDLE Socket)
{
	// Local function variables
	XnInt32 nRetVal = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_NULL(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Listen to the socket and make sure it succeeded
	nRetVal = listen(Socket->Socket, SOMAXCONN);
	if (nRetVal	== SOCKET_ERROR)
	{
		return(XN_STATUS_OS_NETWORK_SOCKET_LISTEN_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

static struct timeval* xnOSMillisecsToTimeVal(XnUInt32 nMilliseconds, struct timeval* pTv)
{
	if (nMilliseconds == XN_WAIT_INFINITE)
	{
		return NULL;
	}

	pTv->tv_sec = nMilliseconds / 1000; //Seconds
	pTv->tv_usec = (nMilliseconds % 1000) * 1000; //Microseconds
	return pTv;
}

XN_C_API XnStatus xnOSAcceptSocket(XN_SOCKET_HANDLE ListenSocket, XN_SOCKET_HANDLE* AcceptSocketPtr, XnUInt32 nMillisecondsTimeout)
{
	// Local function variables
	XnInt32 nRetVal = 0;
	struct timeval selectTimeOut;
	struct timeval* pTimeout = xnOSMillisecsToTimeVal(nMillisecondsTimeout, &selectTimeOut);
	fd_set fdReadHandles;
	XN_SOCKET_HANDLE AcceptSocket = NULL;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(ListenSocket);
	XN_VALIDATE_OUTPUT_PTR(AcceptSocketPtr);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_NULL(ListenSocket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Wait for connection request
	XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_CONDITION_IS_CONST_WARNING_ID);
	FD_ZERO(&fdReadHandles);
	FD_SET(ListenSocket->Socket, &fdReadHandles);
	XN_PRAGMA_STOP_DISABLED_WARNING_SECTION;

	nRetVal = select(1 /* ignored */, &fdReadHandles, NULL, NULL, pTimeout);
	if (nRetVal == 0)
	{
		return (XN_STATUS_OS_NETWORK_TIMEOUT);
	}
	else if (nRetVal == SOCKET_ERROR)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_SOCKET_ACCEPT_FAILED, XN_MASK_OS, "select() returned WinSock error: %d", WSAGetLastError());
	}

	// Allocate a new socket
	XN_VALIDATE_ALIGNED_CALLOC(*AcceptSocketPtr, xnOSSocket, 1, XN_DEFAULT_MEM_ALIGN);

	AcceptSocket = *AcceptSocketPtr;

	// Accept the socket and make sure it succeeded
	AcceptSocket->nSocketAddressLen = sizeof(AcceptSocket->SocketAddress);
	AcceptSocket->Socket = accept(ListenSocket->Socket, (sockaddr*)&AcceptSocket->SocketAddress, &AcceptSocket->nSocketAddressLen);
	if (AcceptSocket->Socket == INVALID_SOCKET)
	{
		xnOSCloseSocket(AcceptSocket);
		xnOSFreeAligned(*AcceptSocketPtr);
		return(XN_STATUS_OS_NETWORK_SOCKET_ACCEPT_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSConnectSocket(XN_SOCKET_HANDLE Socket, XnUInt32 nMillisecsTimeout)
{
	// Local function variables
	XnInt32 nRetVal = 0;
	sockaddr SocketAddress;
	fd_set fdWriteHandles;
	fd_set fdExceptHandles;
	struct timeval selectTimeOut;
	struct timeval* pTimeout = xnOSMillisecsToTimeVal(nMillisecsTimeout, &selectTimeOut);

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_NULL(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Connect to the socket and make sure it succeeded
	xnOSMemCopy(&SocketAddress, &Socket->SocketAddress, sizeof(SocketAddress));

	// if timeout is XN_SOCKET_DEFAULT_TIMEOUT, leave the socket as a blocking one
	if (nMillisecsTimeout != XN_SOCKET_DEFAULT_TIMEOUT)
	{
		// Make the socket non-blocking temporarily
		u_long nNonBlockingSocket = 1;
		if (ioctlsocket(Socket->Socket, FIONBIO, &nNonBlockingSocket) != 0)
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_SOCKET_CONNECT_FAILED, XN_MASK_OS, "ioctlsocket() failed with error %d", WSAGetLastError());
		}
	}

	nRetVal = connect(Socket->Socket, &SocketAddress, sizeof(SocketAddress));
	if ((nRetVal == SOCKET_ERROR) && (WSAGetLastError() != WSAEWOULDBLOCK))
	{
		xnLogError(XN_MASK_OS, "connect() failed with winsock error %d", WSAGetLastError());
		return(XN_STATUS_OS_NETWORK_SOCKET_CONNECT_FAILED);
	}

	if (nMillisecsTimeout != XN_SOCKET_DEFAULT_TIMEOUT)
	{
		XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_CONDITION_IS_CONST_WARNING_ID);
		FD_ZERO(&fdWriteHandles);
		FD_SET(Socket->Socket, &fdWriteHandles);
		FD_ZERO(&fdExceptHandles);
		FD_SET(Socket->Socket, &fdExceptHandles);
		XN_PRAGMA_STOP_DISABLED_WARNING_SECTION;

		nRetVal = select(1 /* ignored */, NULL, &fdWriteHandles, &fdExceptHandles, pTimeout);

		// in any case, make the socket blocking again before we check select()'s success
		u_long nBlockingSocket = 0;
		ioctlsocket(Socket->Socket, FIONBIO, &nBlockingSocket);

		if (nRetVal == 0)
		{
			return (XN_STATUS_OS_NETWORK_TIMEOUT);
		}
		else if (nRetVal == SOCKET_ERROR)
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_SOCKET_CONNECT_FAILED, XN_MASK_OS, "select() returned WinSock error: %d", WSAGetLastError());
		}
		else
		{
			// select returned due to socket state change. Check if an error occurred or everything is OK.
			if (FD_ISSET(Socket->Socket, &fdExceptHandles))
			{
				XnUInt32 nLastError = 0;
				XnInt32 nLastErrorSize = sizeof(nLastError);
				getsockopt(Socket->Socket, SOL_SOCKET, SO_ERROR, (char*)&nLastError, &nLastErrorSize);
				XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_SOCKET_CONNECT_FAILED, XN_MASK_OS, "Connect failed with error: %u", nLastError);
			}
			// else, it means it's in the writable state, which means connect succeeded.
			XN_ASSERT(FD_ISSET(Socket->Socket, &fdWriteHandles));
		}
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSetSocketBufferSize(XN_SOCKET_HANDLE Socket, const XnUInt32 nSocketBufferSize)
{
	// Local function variables
	XnInt32 nOptLen = sizeof(XnUInt32);
	XnInt32 nRetVal = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_NULL(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Change the socket sending buffer and make sure it succeeded
	nRetVal = setsockopt(Socket->Socket, SOL_SOCKET, SO_SNDBUF, (XnChar*)&nSocketBufferSize, nOptLen);
	if (nRetVal	== SOCKET_ERROR)
	{
		return(XN_STATUS_OS_NETWORK_SOCKET_BUFFER_FAILED);
	}

	// Change the socket receive buffer and make sure it succeeded
	nRetVal = setsockopt(Socket->Socket, SOL_SOCKET, SO_RCVBUF, (XnChar*)&nSocketBufferSize, nOptLen);
	if (nRetVal	== SOCKET_ERROR)
	{
		return(XN_STATUS_OS_NETWORK_SOCKET_BUFFER_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSendNetworkBuffer(XN_SOCKET_HANDLE Socket, const XnChar* cpBuffer, const XnUInt32 nBufferSize)
{
	// Local function variables
	XnInt32 nRetVal = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);
	XN_VALIDATE_INPUT_PTR(cpBuffer);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_NULL(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Send the data over our socket to the server and make sure the wanted number of bytes were actually sent
	nRetVal = send(Socket->Socket, cpBuffer, nBufferSize, 0);
	if (nRetVal != (int)nBufferSize)
	{
		return (XN_STATUS_OS_NETWORK_SEND_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSSendToNetworkBuffer(XN_SOCKET_HANDLE Socket, const XnChar* cpBuffer, const XnUInt32 nBufferSize, XN_SOCKET_HANDLE SocketTo)
{
	// Local function variables
	XnInt32 nRetVal = 0;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);
	XN_VALIDATE_INPUT_PTR(SocketTo);
	XN_VALIDATE_INPUT_PTR(cpBuffer);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_NULL(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Send the data over our UDP socket to the server and make sure the wanted number of bytes were actually sent
	nRetVal = sendto(Socket->Socket, cpBuffer, nBufferSize, 0, (SOCKADDR*)&SocketTo->SocketAddress, sizeof(SOCKADDR));
	if (nRetVal != (int)nBufferSize)
	{
		return (XN_STATUS_OS_NETWORK_SEND_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReceiveNetworkBuffer(XN_SOCKET_HANDLE Socket, XnChar* cpBuffer, XnUInt32* pnBufferSize, XnUInt32 nMillisecondsTimeout)
{
	// Local function variables
	XnInt32 nRetVal = 0;
	struct timeval selectTimeOut;
	struct timeval* pTimeout = xnOSMillisecsToTimeVal(nMillisecondsTimeout, &selectTimeOut);
	fd_set fdReadHandles;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);
	XN_VALIDATE_OUTPUT_PTR(cpBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnBufferSize);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_NULL(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	XN_PRAGMA_START_DISABLED_WARNING_SECTION(XN_CONDITION_IS_CONST_WARNING_ID);
	FD_ZERO(&fdReadHandles);
	FD_SET(Socket->Socket, &fdReadHandles);
	XN_PRAGMA_STOP_DISABLED_WARNING_SECTION;

	nRetVal = select(1 /* ignored */, &fdReadHandles, NULL, NULL, pTimeout);
	if (nRetVal == 0)
	{
		return (XN_STATUS_OS_NETWORK_TIMEOUT);
	}
	else if (nRetVal != 1)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_RECEIVE_FAILED, XN_MASK_OS, "select() failed with winsock error %d", WSAGetLastError());
	}

	// Receive the data from our socket to the buffer and make sure it succeeded
	*pnBufferSize = recv(Socket->Socket, cpBuffer, *pnBufferSize, 0);
	if (*pnBufferSize == 0)
	{
		xnLogVerbose(XN_MASK_OS, "Socket has been gracefully closed");
		return (XN_STATUS_OS_NETWORK_CONNECTION_CLOSED);
	}
	else if (*pnBufferSize == SOCKET_ERROR)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_RECEIVE_FAILED, XN_MASK_OS, "recv() failed with winsock error %d", WSAGetLastError());
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReceiveFromNetworkBuffer(XN_SOCKET_HANDLE Socket, XnChar* cpBuffer, XnUInt32* pnBufferSize, XN_SOCKET_HANDLE* SocketFrom)
{
	// Local function variables
	XnInt32 nLen = sizeof(SOCKADDR);	

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);
	XN_VALIDATE_INPUT_PTR(SocketFrom);
	XN_VALIDATE_OUTPUT_PTR(cpBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnBufferSize);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_NULL(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Receive the data from our UDP socket to the buffer and make sure it succeeded
	*pnBufferSize = recvfrom(Socket->Socket, cpBuffer, *pnBufferSize, 0, (SOCKADDR*)&(*SocketFrom)->SocketAddress, &nLen);
	if (*pnBufferSize == SOCKET_ERROR)
	{
		return (XN_STATUS_OS_NETWORK_RECEIVE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}
