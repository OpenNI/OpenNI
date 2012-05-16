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
#ifndef __XN_STATUS_CODES_H__
#define __XN_STATUS_CODES_H__

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include "XnStatus.h"

//---------------------------------------------------------------------------
// Status Messages
//---------------------------------------------------------------------------
XN_STATUS_MESSAGE_MAP_START(XN_ERROR_GROUP_NI)
XN_STATUS_MESSAGE(XN_STATUS_ERROR, "Error!")
XN_STATUS_MESSAGE(XN_STATUS_NOT_INIT, "Module was not initialized!")
XN_STATUS_MESSAGE(XN_STATUS_ALREADY_INIT, "Module was already initialized!")
XN_STATUS_MESSAGE(XN_STATUS_NULL_INPUT_PTR, "Input pointer is null!")
XN_STATUS_MESSAGE(XN_STATUS_NULL_OUTPUT_PTR, "Output pointer is null!")
XN_STATUS_MESSAGE(XN_STATUS_INPUT_BUFFER_OVERFLOW, "Input buffer overflow!")
XN_STATUS_MESSAGE(XN_STATUS_OUTPUT_BUFFER_OVERFLOW, "Output buffer overflow!")
XN_STATUS_MESSAGE(XN_STATUS_INTERNAL_BUFFER_TOO_SMALL, "Internal buffer is too small!")
XN_STATUS_MESSAGE(XN_STATUS_INVALID_BUFFER_SIZE, "The buffer is of wrong size!")
XN_STATUS_MESSAGE(XN_STATUS_NO_MATCH, "No match found")
XN_STATUS_MESSAGE(XN_STATUS_IS_EMPTY, "No items in data structure")
XN_STATUS_MESSAGE(XN_STATUS_IS_NOT_EMPTY, "Items in data structure")
XN_STATUS_MESSAGE(XN_STATUS_ILLEGAL_POSITION, "Can't perform operation at this position in the data structure")
XN_STATUS_MESSAGE(XN_STATUS_NOT_IMPLEMENTED, "Function was not implemented!")
XN_STATUS_MESSAGE(XN_STATUS_NO_MODULES_FOUND, "OpenNI library can't find any module!")
XN_STATUS_MESSAGE(XN_STATUS_INVALID_GENERATOR, "Module generator is not valid!")
XN_STATUS_MESSAGE(XN_STATUS_UNKNOWN_GENERATOR_TYPE, "Generator type is unknown!")
XN_STATUS_MESSAGE(XN_STATUS_INVALID_OPERATION, "This operation is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_MISSING_NEEDED_TREE, "A needed production node is missing!")
XN_STATUS_MESSAGE(XN_STATUS_CORRUPT_FILE, "The file is corrupted!")
XN_STATUS_MESSAGE(XN_STATUS_BAD_PARAM, "The value is invalid!")
XN_STATUS_MESSAGE(XN_STATUS_NODE_IS_LOCKED, "The node is locked for changes!")
XN_STATUS_MESSAGE(XN_STATUS_WAIT_DATA_TIMEOUT, "A timeout has occurred when waiting for new data!")
XN_STATUS_MESSAGE(XN_STATUS_BAD_TYPE, "The operation requires an object of a different type!")
XN_STATUS_MESSAGE(XN_STATUS_UNSUPPORTED_VERSION, "Unsupported version")
XN_STATUS_MESSAGE(XN_STATUS_PROPERTY_NOT_SET, "The property is not set")
XN_STATUS_MESSAGE(XN_STATUS_BAD_FILE_EXT, "Bad file extension")
XN_STATUS_MESSAGE(XN_STATUS_NODE_NOT_LOADED, "No node with the requested description is loaded")
XN_STATUS_MESSAGE(XN_STATUS_NO_NODE_PRESENT, "Can't create any node of the requested type!")
XN_STATUS_MESSAGE(XN_STATUS_BAD_NODE_NAME, "No node by the specified name exists")
XN_STATUS_MESSAGE(XN_STATUS_UNSUPPORTED_CODEC, "Failed to find a matching codec")
XN_STATUS_MESSAGE(XN_STATUS_EOF, "End of file reached")
XN_STATUS_MESSAGE(XN_STATUS_MULTIPLE_NODES_ERROR, "More than one node is in error state!")
XN_STATUS_MESSAGE(XN_STATUS_DEVICE_NOT_CONNECTED, "The device is not connected!")
XN_STATUS_MESSAGE(XN_STATUS_NO_LICENSE, "No license to use this module!")
XN_STATUS_MESSAGE(XN_STATUS_NO_SUCH_PROPERTY, "There is no such property")
XN_STATUS_MESSAGE(XN_STATUS_NODE_ALREADY_RECORDED, "This node is already recorded!")
XN_STATUS_MESSAGE(XN_STATUS_PROTO_BAD_INTERFACE, "Bad protocol interface")
XN_STATUS_MESSAGE(XN_STATUS_PROTO_BAD_MSG_TYPE, "Bad protocol message type")
XN_STATUS_MESSAGE(XN_STATUS_PROTO_BAD_CID, "Bad protocol CID")
XN_STATUS_MESSAGE(XN_STATUS_PROTO_BAD_NODE_ID, "Bad protocol node ID")
XN_STATUS_MESSAGE(XN_STATUS_PROTO_BAD_MSG_SIZE, "Bad protocol message size")
XN_STATUS_MESSAGE(XN_STATUS_NO_SUCH_USER, "User could not be found")
XN_STATUS_MESSAGE(XN_STATUS_USER_IS_NOT_BEING_TRACKED, "User is not being tracked")
XN_STATUS_MESSAGE(XN_STATUS_JOINT_IS_NOT_ACTIVE, "Joint is not active")
XN_STATUS_MESSAGE_MAP_END(XN_ERROR_GROUP_NI)

#endif // __XN_STATUS_CODES_H__
