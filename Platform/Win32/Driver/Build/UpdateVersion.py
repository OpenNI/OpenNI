#/****************************************************************************
#*                                                                           *
#*  OpenNI 1.x Alpha                                                         *
#*  Copyright (C) 2012 PrimeSense Ltd.                                       *
#*                                                                           *
#*  This file is part of OpenNI.                                             *
#*                                                                           *
#*  Licensed under the Apache License, Version 2.0 (the "License");          *
#*  you may not use this file except in compliance with the License.         *
#*  You may obtain a copy of the License at                                  *
#*                                                                           *
#*      http://www.apache.org/licenses/LICENSE-2.0                           *
#*                                                                           *
#*  Unless required by applicable law or agreed to in writing, software      *
#*  distributed under the License is distributed on an "AS IS" BASIS,        *
#*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
#*  See the License for the specific language governing permissions and      *
#*  limitations under the License.                                           *
#*                                                                           *
#****************************************************************************/
#
import sys
import os
import re
import time
import datetime

# ----------------------- PARAMS -------------------------
VERSION_MAJOR = 3
VERSION_MINOR = 2
VERSION_MAINTENANCE = 0
VERSION_BUILD = 1

# ---------------------- FUNCTIONS -----------------------
def regx_replace(findStr,repStr,filePath):
	"replaces all findStr by repStr in file filePath using regualr expression"
	findStrRegx = re.compile(findStr)
	tempName=filePath+'~~~'
	os.system("attrib -r " + filePath)
	input = open(filePath)
	output = open(tempName,'w')
	for s in input:
		output.write(findStrRegx.sub(repStr,s))
	output.close()
	input.close()
	os.remove(filePath)
	os.rename(tempName,filePath)

def update_self_defs (filePath):
	print "Updating self version defines: " + filePath
	regx_replace("VERSION_MAJOR = \d+\n", "VERSION_MAJOR = " + str(VERSION_MAJOR) + "\n", filePath)
	regx_replace("VERSION_MINOR = \d+\n", "VERSION_MINOR = " + str(VERSION_MINOR) + "\n", filePath)
	regx_replace("VERSION_MAINTENANCE = \d+\n", "VERSION_MAINTENANCE = " + str(VERSION_MAINTENANCE) + "\n", filePath)	
	regx_replace("VERSION_BUILD = \d+\n", "VERSION_BUILD = " + str(VERSION_BUILD) + "\n", filePath)

def update_src_ver_defs (filePath):
	print "Updating source version defines: " + filePath
	regx_replace("#define PSUSBDRV_MAJOR_VERSION (.*)", "#define PSUSBDRV_MAJOR_VERSION " + str(VERSION_MAJOR), filePath)
	regx_replace("#define PSUSBDRV_MINOR_VERSION (.*)", "#define PSUSBDRV_MINOR_VERSION " + str(VERSION_MINOR), filePath)
	regx_replace("#define PSUSBDRV_MAINTENANCE_VERSION (.*)", "#define PSUSBDRV_MAINTENANCE_VERSION " + str(VERSION_MAINTENANCE), filePath)	
	regx_replace("#define PSUSBDRV_BUILD_VERSION (.*)", "#define PSUSBDRV_BUILD_VERSION " + str(VERSION_BUILD), filePath)

def update_inf_ver_defs (filePath):
	print "Updating inf version defines: " + filePath
	regx_replace("DriverVer=(.*),(.*)", "DriverVer="+time.strftime("%m/%d/%Y")+","+str(VERSION_MAJOR)+"."+str(VERSION_MINOR)+"."+str(VERSION_MAINTENANCE)+"."+str(VERSION_BUILD), filePath)
	
# ----------------------- MAIN -------------------------
if len(sys.argv) == 5:
	VERSION_MAJOR=int(sys.argv[1])
	VERSION_MINOR=int(sys.argv[2])
	VERSION_MAINTENANCE=int(sys.argv[3])	
	VERSION_BUILD=int(sys.argv[4])

if (VERSION_MAJOR > 9):
	print "Illegal major version"
	sys.exit()

if (VERSION_MINOR > 99):
	print "Illegal minor version"
	sys.exit()

if (VERSION_MAINTENANCE > 99):
	print "Illegal maintenance version"
	sys.exit()
	
if (VERSION_BUILD > 9999):
	print "Illegal build version"
	sys.exit()

print "Going to update files to version: %d.%d.%d.%d" % (VERSION_MAJOR, VERSION_MINOR, VERSION_MAINTENANCE, VERSION_BUILD)

update_self_defs("./UpdateVersion.py")
update_src_ver_defs("./sys/PSDrvVersion.h")
update_inf_ver_defs("./inf/psdrv3.inf")

print "\n*** Done ***"
