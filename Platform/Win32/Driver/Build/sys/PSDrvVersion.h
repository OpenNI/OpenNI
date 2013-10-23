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
#define PSUSBDRV_MAJOR_VERSION 3
#define PSUSBDRV_MINOR_VERSION 2
#define PSUSBDRV_MAINTENANCE_VERSION 0
#define PSUSBDRV_BUILD_VERSION 1

#define PSDRV_STRINGIFY(n) PSDRV_STRINGIFY_HELPER(n)
#define PSDRV_STRINGIFY_HELPER(n) #n

#define PSDRV_BRIEF_VERSION_STRING							\
		PSDRV_STRINGIFY(PSUSBDRV_MAJOR_VERSION) "."			\
		PSDRV_STRINGIFY(PSUSBDRV_MINOR_VERSION)	"."			\
		PSDRV_STRINGIFY(PSUSBDRV_MAINTENANCE_VERSION) "."	\
		PSDRV_STRINGIFY(PSUSBDRV_BUILD_VERSION)
