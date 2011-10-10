#/***************************************************************************
#*                                                                          *
#*  OpenNI 1.x Alpha                                                        *
#*  Copyright (C) 2011 PrimeSense Ltd.                                      *
#*                                                                          *
#*  This file is part of OpenNI.                                            *
#*                                                                          *
#*  OpenNI is free software: you can redistribute it and/or modify          *
#*  it under the terms of the GNU Lesser General Public License as published*
#*  by the Free Software Foundation, either version 3 of the License, or    *
#*  (at your option) any later version.                                     *
#*                                                                          *
#*  OpenNI is distributed in the hope that it will be useful,               *
#*  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
#*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the            *
#*  GNU Lesser General Public License for more details.                     *
#*                                                                          *
#*  You should have received a copy of the GNU Lesser General Public License*
#*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.          *
#*                                                                          *
#***************************************************************************/
#
import sys
import os
import re

# ----------------------- PARAMS -------------------------
VERSION_MAJOR = 1
VERSION_MINOR = 3
VERSION_MAINTENANCE = 3
VERSION_BUILD = 6

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
	regx_replace("#define XN_MAJOR_VERSION (.*)", "#define XN_MAJOR_VERSION " + str(VERSION_MAJOR), filePath)
	regx_replace("#define XN_MINOR_VERSION (.*)", "#define XN_MINOR_VERSION " + str(VERSION_MINOR), filePath)
	regx_replace("#define XN_MAINTENANCE_VERSION (.*)", "#define XN_MAINTENANCE_VERSION " + str(VERSION_MAINTENANCE), filePath)	
	regx_replace("#define XN_BUILD_VERSION (.*)", "#define XN_BUILD_VERSION " + str(VERSION_BUILD), filePath)

def update_redist_defs (filePath):
	print "Updating RedistMaker version defines: " + filePath
	regx_replace("set MAJOR_VERSION=(.*)", "set MAJOR_VERSION=" + str(VERSION_MAJOR), filePath)
	regx_replace("set MINOR_VERSION=(.*)", "set MINOR_VERSION=" + str(VERSION_MINOR), filePath)
	regx_replace("set MAINTENANCE_VERSION=(.*)", "set MAINTENANCE_VERSION=" + str(VERSION_MAINTENANCE), filePath)	
	regx_replace("set BUILD_VERSION=(.*)", "set BUILD_VERSION=" + str(VERSION_BUILD), filePath)

def update_redist_script (filePath):
	print "Updating Redist script version defines: " + filePath
	regx_replace("set MAJOR_VERSION=(.*)", "set MAJOR_VERSION=" + str(VERSION_MAJOR), filePath)
	regx_replace("set MINOR_VERSION=(.*)", "set MINOR_VERSION=" + str(VERSION_MINOR), filePath)
	regx_replace("set MAINTENANCE_VERSION=(.*)", "set MAINTENANCE_VERSION=" + str(VERSION_MAINTENANCE), filePath)	
	regx_replace("set BUILD_VERSION=(.*)", "set BUILD_VERSION=" + str(VERSION_BUILD), filePath)

def update_redist_eng_defs (filePath):
	print "Updating Redist Engine version defines: " + filePath
	regx_replace("  <VERSION_NUMBER>(\d+).(\d+).(\d+)</VERSION_NUMBER>", "  <VERSION_NUMBER>" + str(VERSION_MAJOR) + "." + str(VERSION_MINOR) + "." + str(VERSION_MAINTENANCE) + "</VERSION_NUMBER>", filePath)
	
def update_assembly_info (filePath):
	print "Updating assembly info: " + filePath
	regx_replace(r"^\[assembly: AssemblyVersion\(\"(.*)\"\)\]", "[assembly: AssemblyVersion(\"" + str(VERSION_MAJOR) + "." + str(VERSION_MINOR) + "." + str(VERSION_MAINTENANCE) + "." + str(VERSION_BUILD) + "\")]", filePath)
	regx_replace(r"^\[assembly: AssemblyFileVersion\(\"(.*)\"\)\]", "[assembly: AssemblyFileVersion(\"" + str(VERSION_MAJOR) + "." + str(VERSION_MINOR) + "." + str(VERSION_MAINTENANCE) + "." + str(VERSION_BUILD) + "\")]", filePath)

def update_wix (filePath):
	print "Updating wix: " + filePath
	regx_replace("define MajorVersion=(.*)", "define MajorVersion=" + str(VERSION_MAJOR) + "?>", filePath)
	regx_replace("define MinorVersion=(.*)", "define MinorVersion=" + str(VERSION_MINOR) + "?>", filePath)
	regx_replace("define MaintenanceVersion=(.*)", "define MaintenanceVersion=" + str(VERSION_MAINTENANCE) + "?>", filePath)
	regx_replace("define BuildVersion=(.*)", "define BuildVersion=" + str(VERSION_BUILD) + "?>", filePath)
		
def update_publisher_policy (filePath):
	print "Updating publisher policy: " + filePath
	regx_replace("oldVersion=\"1.1.0.31 - (.*)\"", "oldVersion=\"1.1.0.31 - " + strVersion + "\"", filePath)
	regx_replace("newVersion=\"(.*)\"", "newVersion=\"" + strVersion + "\"", filePath)
		
def update_doxygen (filePath):
	print "Updating doxygen: " + filePath
	regx_replace("PROJECT_NAME\s*=\s*\"OpenNI (\d+)\.(\d+)\.(\d+)\"", "PROJECT_NAME = \"OpenNI " + str(VERSION_MAJOR) + "." + str(VERSION_MINOR) + "." + str(VERSION_MAINTENANCE) + "\"", filePath)

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

strVersion = str(VERSION_MAJOR) + "." + str(VERSION_MINOR) + "." + str(VERSION_MAINTENANCE) + "." + str(VERSION_BUILD)	
	
print "Going to update files to version: %d.%d.%d.%d" % (VERSION_MAJOR, VERSION_MINOR, VERSION_MAINTENANCE, VERSION_BUILD)

update_self_defs("./UpdateVersion.py")
update_redist_defs("./RedistMaker.bat")
update_redist_script("./Redist_OpenNI.py")
update_redist_eng_defs("./OpenNi_Config.xml")
update_src_ver_defs("../../../Include/XnVersion.h")
update_assembly_info("../Build/Res/AssemblyInfo-OpenNI.cs")
update_assembly_info("../../Linux-x86/Build/Res/AssemblyInfo-OpenNI.cs")
update_wix("../Install/OpenNI/Includes/OpenNIVariables.wxi")
update_publisher_policy("../../../Wrappers/OpenNI.Net/PublisherPolicy.config")
update_doxygen("../../../Source/DoxyGen/Doxyfile")

try:
	update_redist_defs("../../../ExportOpenSource.bat")
except:
	print "Skipping the export script..."
 
print "\n*** Done ***"
