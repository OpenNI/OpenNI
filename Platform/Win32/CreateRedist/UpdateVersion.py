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

VERSION_MAJOR = 1
VERSION_MINOR = 4
VERSION_MAINTENANCE = 0
VERSION_BUILD = 2

class UpdateVersion:
    def main(self):
        self.version_major = VERSION_MAJOR
        self.version_minor = VERSION_MINOR
        self.version_maintenance = VERSION_MAINTENANCE
        self.version_build = VERSION_BUILD
        # ----------------------- MAIN -------------------------
        if len(sys.argv) == 5:
            self.version_major=int(sys.argv[1])
            self.version_minor=int(sys.argv[2])
            self.version_maintenance=int(sys.argv[3])
            self.version_build=int(sys.argv[4])

        if (self.version_major > 9):
            print( "Illegal major version")
            sys.exit()

        if (self.version_minor > 99):
            print ("Illegal minor version")
            sys.exit()

        if (self.version_maintenance > 99):
            print ("Illegal maintenance version")
            sys.exit()

        if (self.version_build > 9999):
            print ("Illegal build version")
            sys.exit()

        self.strVersion = str(self.version_major) + "." + str(self.version_minor) + "." + str(self.version_maintenance) + "." + str(self.version_build)

        print (("Going to update files to version: %d.%d.%d.%d" % (self.version_major, self.version_minor, self.version_maintenance, self.version_build)))

        self.update_self_defs("./UpdateVersion.py")
        self.update_redist_defs("./RedistMaker.bat")
        self.update_redist_script("./Redist_OpenNI.py")
        self.update_redist_eng_defs("./OpenNi_Config.xml")
        self.update_src_ver_defs("../../../Include/XnVersion.h")
        self.update_assembly_info("../Build/Res/AssemblyInfo-OpenNI.cs")
        self.update_assembly_info("../../Linux/Build/Res/AssemblyInfo-OpenNI.cs")
        self.update_wix("../Install/OpenNI/Includes/OpenNIVariables.wxi")
        self.update_publisher_policy("../../../Wrappers/OpenNI.Net/PublisherPolicy.config")
        self.update_doxygen("../../../Source/DoxyGen/Doxyfile")

        try:
            self.update_redist_defs("../../../ExportOpenSource.bat")
        except:
            print ("Skipping the export script...")

        print ("\n*** Done ***")
    def regx_replace(self,findStr,repStr,filePath):
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

    def update_self_defs (self,filePath):
        print(( "Updating self version defines: " + filePath))
        self.regx_replace("VERSION_MAJOR = \d+\n", "VERSION_MAJOR = " + str(self.version_major) + "\n", filePath)
        self.regx_replace("VERSION_MINOR = \d+\n", "VERSION_MINOR = " + str(self.version_minor) + "\n", filePath)
        self.regx_replace("VERSION_MAINTENANCE = \d+\n", "VERSION_MAINTENANCE = " + str(self.version_maintenance) + "\n", filePath)
        self.regx_replace("VERSION_BUILD = \d+\n", "VERSION_BUILD = " + str(self.version_build) + "\n", filePath)

    def update_src_ver_defs (self,filePath):
        print(( "Updating source version defines: " + filePath))
        self.regx_replace("#define XN_MAJOR_VERSION (.*)", "#define XN_MAJOR_VERSION " + str(self.version_major), filePath)
        self.regx_replace("#define XN_MINOR_VERSION (.*)", "#define XN_MINOR_VERSION " + str(self.version_minor), filePath)
        self.regx_replace("#define XN_MAINTENANCE_VERSION (.*)", "#define XN_MAINTENANCE_VERSION " + str(self.version_maintenance), filePath)
        self.regx_replace("#define XN_BUILD_VERSION (.*)", "#define XN_BUILD_VERSION " + str(self.version_build), filePath)

    def update_redist_defs (self,filePath):
        print(( "Updating RedistMaker version defines: " + filePath))
        self.regx_replace("set MAJOR_VERSION=(.*)", "set MAJOR_VERSION=" + str(self.version_major), filePath)
        self.regx_replace("set MINOR_VERSION=(.*)", "set MINOR_VERSION=" + str(self.version_minor), filePath)
        self.regx_replace("set MAINTENANCE_VERSION=(.*)", "set MAINTENANCE_VERSION=" + str(self.version_maintenance), filePath)
        self.regx_replace("set BUILD_VERSION=(.*)", "set BUILD_VERSION=" + str(self.version_build), filePath)

    def update_redist_script (self,filePath):
        print(("Updating Redist script version defines: " + filePath))
        self.regx_replace("set MAJOR_VERSION=(.*)", "set MAJOR_VERSION=" + str(self.version_major), filePath)
        self.regx_replace("set MINOR_VERSION=(.*)", "set MINOR_VERSION=" + str(self.version_minor), filePath)
        self.regx_replace("set MAINTENANCE_VERSION=(.*)", "set MAINTENANCE_VERSION=" + str(self.version_maintenance), filePath)
        self.regx_replace("set BUILD_VERSION=(.*)", "set BUILD_VERSION=" + str(self.version_build), filePath)

    def update_redist_eng_defs (self,filePath):
        print (("Updating Redist Engine version defines: " + filePath))
        self.regx_replace("  <VERSION_NUMBER>(\d+).(\d+).(\d+)</VERSION_NUMBER>", "  <VERSION_NUMBER>" + str(self.version_major) + "." + str(self.version_minor) + "." + str(self.version_maintenance) + "</VERSION_NUMBER>", filePath)

    def update_assembly_info (self,filePath):
        print (("Updating assembly info: " + filePath))
        self.regx_replace(r"^\[assembly: AssemblyVersion\(\"(.*)\"\)\]", "[assembly: AssemblyVersion(\"" + str(self.version_major) + "." + str(self.version_minor) + "." + str(self.version_maintenance) + "." + str(self.version_build) + "\")]", filePath)
        self.regx_replace(r"^\[assembly: AssemblyFileVersion\(\"(.*)\"\)\]", "[assembly: AssemblyFileVersion(\"" + str(self.version_major) + "." + str(self.version_minor) + "." + str(self.version_maintenance) + "." + str(self.version_build) + "\")]", filePath)

    def update_wix (self,filePath):
        print (("Updating wix: " + filePath))
        self.regx_replace("define MajorVersion=(.*)", "define MajorVersion=" + str(self.version_major) + "?>", filePath)
        self.regx_replace("define MinorVersion=(.*)", "define MinorVersion=" + str(self.version_minor) + "?>", filePath)
        self.regx_replace("define MaintenanceVersion=(.*)", "define MaintenanceVersion=" + str(self.version_maintenance) + "?>", filePath)
        self.regx_replace("define BuildVersion=(.*)", "define BuildVersion=" + str(self.version_build) + "?>", filePath)

    def update_publisher_policy (self,filePath):
        print(( "Updating publisher policy: " + filePath))
        self.regx_replace("oldVersion=\"1.1.0.31 - (.*)\"", "oldVersion=\"1.1.0.31 - " + self.strVersion + "\"", filePath)
        self.regx_replace("newVersion=\"(.*)\"", "newVersion=\"" + self.strVersion + "\"", filePath)

    def update_doxygen (self,filePath):
        print (("Updating doxygen: " + filePath))
        self.regx_replace("PROJECT_NAME\s*=\s*\"OpenNI (\d+)\.(\d+)\.(\d+)\"", "PROJECT_NAME = \"OpenNI " + str(self.version_major) + "." + str(self.version_minor) + "." + str(self.version_maintenance) + "\"", filePath)
if __name__ == '__main__':
    UpdateVersion().main()
