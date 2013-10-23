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
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ctype.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <XnLog.h>

//---------------------------------------------------------------------------
// Structs
//---------------------------------------------------------------------------
/** The Xiron OS network socket structure. */ 
typedef struct xnOSSocket
{
	/** The OS socket handle. */ 
	int Socket;

	/** The OS socket address (IP and port). */ 
	sockaddr_in SocketAddress;
	socklen_t nSocketAddressLen;

	/** The socket type enum (UDP, TDP, etc...) */ 
	XnUInt32 nSocketType;
} xnOSSocket;

//---------------------------------------------------------------------------
// Macros
//---------------------------------------------------------------------------
/** Returns Y if X is -1. */
#define XN_RET_IF_INVALID(x, y)	\
		if (x == -1)			\
		{						\
			return (y);			\
		}

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

	if (Socket->Socket == -1)
	{
		XN_ALIGNED_FREE_AND_NULL(Socket);
		return (XN_STATUS_OS_NETWORK_SOCKET_CREATION_FAILED);
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

	// turn on NODELAY option
	int on = 1;
	setsockopt(Socket->Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&on, sizeof(on));

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
	XN_RET_IF_INVALID(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// shut down the socket
	if (-1 == shutdown(Socket->Socket, SHUT_RDWR))
	{
		return(XN_STATUS_OS_NETWORK_SHUTDOWN_FAILED);
	}

	// Close the socket and make sure it succeeded (return value is 0)
	if (-1 == close(Socket->Socket))
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
	XN_RET_IF_INVALID(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);
	
	// Workaround Linux annoying behavior. Linux keeps a port open in a TIME_WAIT state after it is close,
	// and does not allow to bind it again. If socket is closed, you have to wait a couple of minutes before
	// you can re-bind it. We disable this option
	const int bOn = 1;
	setsockopt(Socket->Socket, SOL_SOCKET, SO_REUSEADDR, &bOn, sizeof(bOn));

	// Bind the socket and make sure it succeeded
	nRetVal = bind(Socket->Socket, (const sockaddr*)&Socket->SocketAddress, sizeof(Socket->SocketAddress));
	if (nRetVal	== -1)
	{
		xnLogWarning(XN_MASK_OS, "Failed to bind socket: errno is %d", errno);
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
	XN_RET_IF_INVALID(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Listen to the socket and make sure it succeeded
	nRetVal = listen(Socket->Socket, SOMAXCONN);
	if (nRetVal	== -1)
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
	XN_RET_IF_INVALID(ListenSocket->Socket, XN_STATUS_OS_INVALID_SOCKET);
	// Wait for connection request
	FD_ZERO(&fdReadHandles);
	FD_SET(ListenSocket->Socket, &fdReadHandles);
	nRetVal = select(ListenSocket->Socket + 1, &fdReadHandles, NULL, NULL, pTimeout);
	if (nRetVal == 0)
	{
		return (XN_STATUS_OS_NETWORK_TIMEOUT);
	}
	else if (nRetVal == -1)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_SOCKET_ACCEPT_FAILED, XN_MASK_OS, "select() returned error: %d", errno);
	}

	// Allocate a new socket
	XN_VALIDATE_ALIGNED_CALLOC(*AcceptSocketPtr, xnOSSocket, 1, XN_DEFAULT_MEM_ALIGN);

	AcceptSocket = *AcceptSocketPtr;

	// Accept the socket and make sure it succeeded
	AcceptSocket->nSocketAddressLen = sizeof(AcceptSocket->SocketAddress);
	AcceptSocket->Socket = accept(ListenSocket->Socket, (sockaddr*)&AcceptSocket->SocketAddress, &AcceptSocket->nSocketAddressLen);
	if (AcceptSocket->Socket== -1)
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
	int nFlags;
	struct timeval* pTimeout = xnOSMillisecsToTimeVal(nMillisecsTimeout, &selectTimeOut);

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_INVALID(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Connect to the socket and make sure it succeeded
	if (sizeof(SocketAddress) != sizeof(Socket->SocketAddress))
	{
		return(XN_STATUS_OS_NETWORK_SOCKET_CONNECT_FAILED);
	}

	xnOSMemCopy(&SocketAddress, &Socket->SocketAddress, sizeof(SocketAddress));
	
	// if timeout is XN_SOCKET_DEFAULT_TIMEOUT, leave the socket as a blocking one
	if (nMillisecsTimeout != XN_SOCKET_DEFAULT_TIMEOUT)
	{
		// Make the socket non-blocking temporarily
		nFlags = fcntl(Socket->Socket, F_GETFL, 0);
		int nNonBlockFlags = nFlags | O_NONBLOCK;
		if (-1 == fcntl(Socket->Socket, F_SETFL, nNonBlockFlags))
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_SOCKET_CONNECT_FAILED, XN_MASK_OS, "fcntl() failed with error %d", errno);
		}
	}

	nRetVal = connect(Socket->Socket, &SocketAddress, sizeof(SocketAddress));
	if (nRetVal	== -1 && errno != EINPROGRESS)
	{
		xnLogError(XN_MASK_OS, "connect() failed with error %d", errno);
		return(XN_STATUS_OS_NETWORK_SOCKET_CONNECT_FAILED);
	}

	if (nMillisecsTimeout != XN_SOCKET_DEFAULT_TIMEOUT)
	{
		FD_ZERO(&fdWriteHandles);
		FD_SET(Socket->Socket, &fdWriteHandles);
		FD_ZERO(&fdExceptHandles);
		FD_SET(Socket->Socket, &fdExceptHandles);
		nRetVal = select(Socket->Socket + 1, NULL, &fdWriteHandles, &fdExceptHandles, pTimeout);

		//Make the socket blocking again before we check select()'s success
		fcntl(Socket->Socket, F_SETFL, nFlags);

		if (nRetVal == 0)
		{
			return (XN_STATUS_OS_NETWORK_TIMEOUT);
		}
		else if (nRetVal == -1)
		{
			XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_SOCKET_ACCEPT_FAILED, XN_MASK_OS, "select() returned error: %d", errno);
		}
		else
		{
			// select returned due to socket state change. Check if an error occurred or everything is OK.
			if (FD_ISSET(Socket->Socket, &fdExceptHandles))
			{
				XnUInt32 nLastError = 0;
				socklen_t nLastErrorSize = sizeof(nLastError);
				getsockopt(Socket->Socket, SOL_SOCKET, SO_ERROR, &nLastError, &nLastErrorSize);
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
	XN_RET_IF_INVALID(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Change the socket sending buffer and make sure it succeeded
	nRetVal = setsockopt(Socket->Socket, SOL_SOCKET, SO_SNDBUF, (XnChar*)&nSocketBufferSize, nOptLen);
	if (nRetVal	== -1)
	{
		return(XN_STATUS_OS_NETWORK_SOCKET_BUFFER_FAILED);
	}

	// Change the socket receive buffer and make sure it succeeded
	nRetVal = setsockopt(Socket->Socket, SOL_SOCKET, SO_RCVBUF, (XnChar*)&nSocketBufferSize, nOptLen);
	if (nRetVal	== -1)
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
	XN_RET_IF_INVALID(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Send the data over our UDP socket to the server and make sure the wanted number of bytes were actually sent
	// Linux note: we use MSG_NOSIGNAL so that we won't get SIGPIPE if socket is closed on the other side. instead,
	// we'll get EPIPE.
#if (XN_PLATFORM == XN_PLATFORM_MACOSX)
	int on = 1;
	setsockopt(Socket->Socket, SOL_SOCKET, SO_NOSIGPIPE, (char*)&on, sizeof(on));	
	nRetVal = send(Socket->Socket, cpBuffer, nBufferSize, NULL); 
#else
	nRetVal = send(Socket->Socket, cpBuffer, nBufferSize, MSG_NOSIGNAL);
#endif

	if (nRetVal != nBufferSize)
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
	XN_RET_IF_INVALID(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Send the data over our UDP socket to the server and make sure the wanted number of bytes were actually sent
	nRetVal = sendto(Socket->Socket, cpBuffer, nBufferSize, 0, (const sockaddr*)&SocketTo->SocketAddress, sizeof(sockaddr));
	if (nRetVal != nBufferSize)
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
	XnInt32 nLen = sizeof(sockaddr);	
	struct timeval selectTimeOut;
	struct timeval* pTimeout = xnOSMillisecsToTimeVal(nMillisecondsTimeout, &selectTimeOut);
	fd_set fdReadHandles;

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);
	XN_VALIDATE_OUTPUT_PTR(cpBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnBufferSize);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_INVALID(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	FD_ZERO(&fdReadHandles);
	FD_SET(Socket->Socket, &fdReadHandles);
	nRetVal = select(Socket->Socket + 1, &fdReadHandles, NULL, NULL, pTimeout);
	if (nRetVal != 1)
	{
		return (XN_STATUS_OS_NETWORK_TIMEOUT);
	}

	// Receive the data from our socket to the buffer and make sure it succeeded
	*pnBufferSize = recv(Socket->Socket, cpBuffer, *pnBufferSize, 0);
	if (*pnBufferSize == 0)
	{
		xnLogVerbose(XN_MASK_OS, "Socket has been gracefully closed");
		return (XN_STATUS_OS_NETWORK_CONNECTION_CLOSED);
	}
	else if (*pnBufferSize == -1)
	{
		XN_LOG_ERROR_RETURN(XN_STATUS_OS_NETWORK_RECEIVE_FAILED, XN_MASK_OS, "recv() failed with error %d", errno);
	}

	// All is good...
	return (XN_STATUS_OK);
}

XN_C_API XnStatus xnOSReceiveFromNetworkBuffer(XN_SOCKET_HANDLE Socket, XnChar* cpBuffer, XnUInt32* pnBufferSize, XN_SOCKET_HANDLE* SocketFrom)
{
	// Local function variables
	XnInt32 nRetVal = 0;
	socklen_t nLen = sizeof(sockaddr);	

	// Validate the input/output pointers (to make sure none of them is NULL)
	XN_VALIDATE_INPUT_PTR(Socket);
	XN_VALIDATE_INPUT_PTR(SocketFrom);
	XN_VALIDATE_OUTPUT_PTR(cpBuffer);
	XN_VALIDATE_OUTPUT_PTR(pnBufferSize);

	// Make sure the actual socket handle isn't NULL
	XN_RET_IF_INVALID(Socket->Socket, XN_STATUS_OS_INVALID_SOCKET);

	// Receive the data from our UDP socket to the buffer and make sure it succeeded
	*pnBufferSize = recvfrom(Socket->Socket, cpBuffer, *pnBufferSize, 0, (sockaddr*)&(*SocketFrom)->SocketAddress, &nLen);
	if (*pnBufferSize == -1)
	{
		return (XN_STATUS_OS_NETWORK_RECEIVE_FAILED);
	}

	// All is good...
	return (XN_STATUS_OK);
}

