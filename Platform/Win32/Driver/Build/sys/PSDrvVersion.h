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
#define PSUSBDRV_MAJOR_VERSION 3
#define PSUSBDRV_MINOR_VERSION 1
#define PSUSBDRV_MAINTENANCE_VERSION 3
#define PSUSBDRV_BUILD_VERSION 1

#define PSDRV_STRINGIFY(n) PSDRV_STRINGIFY_HELPER(n)
#define PSDRV_STRINGIFY_HELPER(n) #n

#define PSDRV_BRIEF_VERSION_STRING							\
		PSDRV_STRINGIFY(PSUSBDRV_MAJOR_VERSION) "."			\
		PSDRV_STRINGIFY(PSUSBDRV_MINOR_VERSION)	"."			\
		PSDRV_STRINGIFY(PSUSBDRV_MAINTENANCE_VERSION) "."	\
		PSDRV_STRINGIFY(PSUSBDRV_BUILD_VERSION)
