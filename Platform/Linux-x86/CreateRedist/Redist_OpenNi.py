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


#-------------Imports----------------------------------------------------------#
from xml.dom.minidom import parse, parseString
from time import strftime
import logging
import glob
import os
import re
import sys
import shutil
import stat

#-------------Functions--------------------------------------------------------#

def finish_script(exit_code):
    os.chdir(SCRIPT_DIR)
    exit(exit_code)

def replace_string_in_file(findStr,repStr,filePath):
    "replaces all findStr by repStr in file filePath"
    tempName=filePath+'~~~'
    input = open(filePath)
    output = open(tempName,'w')
    for s in input:
        output.write(s.replace(findStr,repStr))
    output.close()
    input.close()
    os.remove(filePath)
    os.rename(tempName,filePath)

def regx_replace(findStr,repStr,filePath):
    "replaces all findStr by repStr in file filePath using regualr expression"
    findStrRegx = re.compile(findStr)
    tempName=filePath+'~~~'
    input = open(filePath)
    output = open(tempName,'w')
    for s in input:
        output.write(findStrRegx.sub(repStr,s))
    output.close()
    input.close()
    os.remove(filePath)
    os.rename(tempName,filePath)

def check_sample(sample_dir):
    "Checks if a sample is a tool or should be skipped, returns: 0 - Regular, 1 - Skip, 2 - Tool"
    rc = 0
    if os.path.exists(sample_dir + "/.redist"):
        redistFile = open(sample_dir + "/.redist")
    else:
        rc=0
        return rc
    redist_lines =redistFile.readlines()
    skip_re = re.compile("^SKIP=([^\|]*\|)*(" + PLATFORM + "|ALL)(\|[^\|]*)*$")
    tool_re = re.compile("^TOOL=([^\|]*\|)*(" + PLATFORM + "|ALL)(\|[^\|]*)*$")
    for line in redist_lines:
        if skip_re.search(line):
            rc = 1
            redistFile.close()
            return rc
        if tool_re.search(line):
            rc = 2
            redistFile.close()
            return rc
    redistFile.close()
    return rc

def fix_file(arg,dirname,fname):
    "Fixes paths for all the files in fname"
    for filename in fname:
        filePath = dirname + "/" + filename
        if os.path.isdir(filePath):
            continue

        ext = ['cpp','h','ini','cs','java']
        if filename == "Makefile" or filename.partition(".")[2] in ext:
            #print "Fixing: " + filePath
            tempName=filePath+'~~~'
            input = open(filePath)
            output = open(tempName,'w')
            for s in input:
                olds = s
                s = re.sub(r"../../../Bin",r"../Bin",s)
                s = re.sub(r"../../../../../Include",r"../../Include /usr/include/ni",s)
                s = re.sub(r"../../../../../Samples/[\w.]+/",r"",s)
                s = re.sub(r"../../../../Data/SamplesConfig.xml",r"../../Config/SamplesConfig.xml",s)
                s = re.sub(r"../../Res/",r"../Res/",s)
                s = re.sub(r"include ../../Common/CommonCppMakefile",r"LIB_DIRS += ../../Lib\ninclude ../Build/Common/CommonCppMakefile",s)
                s = re.sub(r"include ../../Common/CommonCSMakefile",r"LIB_DIRS += ../../Lib\ninclude ../Build/Common/CommonCSMakefile",s)
                s = re.sub(r"include ../../Common/CommonJavaMakefile",r"LIB_DIRS += ../../Lib\ninclude ../Build/Common/CommonJavaMakefile",s)

                output.write(s)
                
                #if s != olds:
                    #print "Changed : " + olds.strip("\n")
                    #print "To      : " + s.strip("\n")

            output.close()
            input.close()
            os.remove(filePath)
            os.rename(tempName,filePath)

def fix_platform_specific_files(arg,dirname,fname):
    "Fixes Platform-specific file"
    for filename in fname:
        filePath = dirname + "/" + filename
        if os.path.isdir(filePath):
            continue

        if filename.find("Platform") != -1:
            tempName=filePath+'~~~'
            input = open(filePath)
            output = open(tempName,'w')
            for s in input:
                if  -1 == s.find("export CXX") and -1 == s.find("export TARGET_SYS_ROOT"):
                    output.write(s)
            output.close();
            os.remove(filePath);
            os.rename(tempName, filePath)
            
def copy_install_script(platform, filePath, dest):
    "Copies the install script and fixing it if needed"
    input = open(filePath)
    dest_name = os.path.join(dest, os.path.basename(filePath))
    output = open(dest_name, 'w')
    
    for line in input:
        if platform == 'CE4100':
            line = re.sub(r"/var/lib/ni", r"/usr/etc/ni", line)
            
        output.write(line)
        
    input.close()
    output.close()
    os.chmod(dest_name, stat.S_IRUSR | stat.S_IXUSR | stat.S_IRGRP | stat.S_IXGRP | stat.S_IROTH | stat.S_IXOTH)
    
def execute_check(cmd, name):
    "Executes command and checks the return code. If it's not 0, stops redist."
    ret = os.system(cmd)
    if ret != 0:
        print "failed to execute: " + cmd
        print name + " Failed!"
        logger.critical(name + " Failed!")
        finish_script(1)
        

#------------Constants and globals---------------------------------------------#
platform="x86"
if len(sys.argv) >= 2:
    platform = sys.argv[1]
    if not platform == 'CE4100' and not platform == 'Arm' and not platform == 'x86':
        print "Unknown platform!!"
        finish_script(1)

DateTimeSTR = strftime("%Y-%m-%d %H:%M:%S")
DateSTR = strftime("%Y-%m-%d")
SCRIPT_DIR = os.getcwd()
# Fix to allow reletive path
WORK_DIR = os.getcwd() + "/"
os.chdir(SCRIPT_DIR)
PROJECT_NAME = "OpenNI"
ostype = os.popen('uname -s').read().rstrip()
machinetype = os.popen('uname -m').read().rstrip()

#-------------Log--------------------------------------------------------------#

# Create output dir
if (os.path.exists(SCRIPT_DIR + "/Output")):
    shutil.rmtree(SCRIPT_DIR + "/Output")
if (os.path.exists(SCRIPT_DIR + "/Final")):
    shutil.rmtree(SCRIPT_DIR + "/Final")
os.makedirs(SCRIPT_DIR + "/Output")
logger = logging.getLogger('myapp')
hdlr = logging.FileHandler('Output/Nightly.log')
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
hdlr.setFormatter(formatter)
logger.addHandler(hdlr)
logger.setLevel(logging.INFO)

#------------Welcome Messege--------=------------------------------------------#
print "\n";
print "*********************************"
print "*   PrimeSense OpenNI Redist    *"
print "*     " + DateTimeSTR + "       *"
print "*********************************"
logger.info("PrimeSense OpenNI Redist Started")

#--------------Take Version----------------------------------------------------#
print "* Taking version..."
logger.info("Taking version...")

version_file = open("../../../Include/XnVersion.h").read()
major = re.search(r"define XN_MAJOR_VERSION (\d+)", version_file).groups()[0]
minor = re.search(r"define XN_MINOR_VERSION (\d+)", version_file).groups()[0]
maintenance = re.search(r"define XN_MAINTENANCE_VERSION (\d+)", version_file).groups()[0]
build = re.search(r"define XN_BUILD_VERSION (\d+)", version_file).groups()[0]

version = major + "." + minor + "." + maintenance + "." + build
print "version is", version

#--------------Build Project---------------------------------------------------#
print "* Building OpenNI..."
logger.info("Building OpenNI...")

# Build
#execute_check("gacutil -u OpenNI.net > " + SCRIPT_DIR + "/Output/gacutil.txt", "Remove from GAC")
result = os.system('make PLATFORM=' + platform + ' -C ../Build clean > ' + SCRIPT_DIR + '/Output/Build' + PROJECT_NAME + '_clean.txt')
execute_check('make PLATFORM=' + platform + ' -C ../Build > ' + SCRIPT_DIR + '/Output/Build' + PROJECT_NAME + '.txt', "Building")

# Get the build output
lines = open(SCRIPT_DIR+"/Output/Build" + PROJECT_NAME + ".txt").readlines()
build_result = lines[-1]
print(build_result)
logger.info(build_result)

# Check for failed build
if result != 0:
    print "Building Failed!!"
    logger.critical("Building Failed!")
    finish_script(1)

#--------------Doxygen---------------------------------------------------------#
print "* Creating Doxygen..."
logger.info("Creating DoxyGen...")
os.chdir("../../../Source/DoxyGen");
if os.path.exists("html"):
    os.system("rm -rf html")
# Running doxygen
os.makedirs("html")
execute_check("doxygen Doxyfile > "+ SCRIPT_DIR + "/Output/EngineDoxy.txt", "Creating Documentation")

# remove unneeded files
os.system("rm -rf html/*.map html/*.md5 html/*.hhc html/*.hhk html/*.hhp")

#-------------Create Redist Dir------------------------------------------------#
print "* Creating Redist Dir..."
logger.info("Creating Redist Dir...")
os.chdir(SCRIPT_DIR + "/..")

# Removing the old directory
if (os.path.exists("Redist")):
    shutil.rmtree("Redist")

# Creating new directory tree
os.makedirs("Redist")
os.makedirs("Redist/Bin")
os.makedirs("Redist/Lib")
os.makedirs("Redist/Jar")
os.makedirs("Redist/Include")
os.makedirs("Redist/Documentation")
os.makedirs("Redist/Samples")
os.makedirs("Redist/Samples/Bin")
os.makedirs("Redist/Samples/Bin/Debug")
os.makedirs("Redist/Samples/Bin/Release")
os.makedirs("Redist/Samples/Build")
os.makedirs("Redist/Samples/Config")
os.makedirs("Redist/Samples/Res")

#-------------Copy files to redist---------------------------------------------#
print "* Copying files to redist dir..."
logger.info("Copying files to redist dir...")

#license
shutil.copy("../../GPL.txt", "Redist")
shutil.copy("../../LGPL.txt", "Redist")

#lib
if ostype == "Darwin":
    LIBS_TYPE = ".dylib"
else:
    LIBS_TYPE = ".so"

shutil.copy("Bin/Release/libnimCodecs"+LIBS_TYPE, "Redist/Lib")
shutil.copy("Bin/Release/libnimMockNodes"+LIBS_TYPE, "Redist/Lib")
shutil.copy("Bin/Release/libnimRecorder"+LIBS_TYPE, "Redist/Lib")
shutil.copy("Bin/Release/libOpenNI"+LIBS_TYPE, "Redist/Lib")
shutil.copy("Bin/Release/libOpenNI.jni"+LIBS_TYPE, "Redist/Lib")

#bin
MonoDetected = 0
shutil.copy("Bin/Release/niReg", "Redist/Bin")
shutil.copy("Bin/Release/niLicense", "Redist/Bin")
if platform == 'x86':
    if (os.path.exists("/usr/bin/gmcs")):
        shutil.copy("Bin/Release/OpenNI.net.dll", "Redist/Bin")
        shutil.copy("Bin/Release/OpenNI.net.dll", "Redist/Samples/Bin/Debug")
        shutil.copy("Bin/Release/OpenNI.net.dll", "Redist/Samples/Bin/Release")
        MonoDetected = 1
        
# java wrapper
shutil.copy("Bin/Release/org.OpenNI.jar", "Redist/Jar")
shutil.copy("Bin/Release/org.OpenNI.jar", "Redist/Samples/Bin/Debug")
shutil.copy("Bin/Release/org.OpenNI.jar", "Redist/Samples/Bin/Release")

#docs
shutil.copytree("../../Source/DoxyGen/html", "Redist/Documentation/html")

#include
for includeFile in os.listdir("../../Include"):
    if not os.path.isdir("../../Include/" + includeFile):
        shutil.copy("../../Include/" + includeFile, "Redist/Include")

shutil.copytree("../../Include/Linux-x86", "Redist/Include/Linux-x86")
shutil.copytree("../../Include/Linux-Arm", "Redist/Include/Linux-Arm")
shutil.copytree("../../Include/MacOSX", "Redist/Include/MacOSX")
shutil.copytree("Build/Common", "Redist/Samples/Build/Common")

# samples
samples_list = os.listdir("Build/Samples")
if '.svn' in samples_list:
    samples_list.remove('.svn')

if platform == 'CE4100':
    samples_list.remove('NiViewer')
    samples_list.remove('NiSimpleViewer')

if platform == 'Arm':
    samples_list.remove('NiUserTracker')
    samples_list.remove('NiViewer')
    samples_list.remove('NiSimpleViewer')

if (MonoDetected == 0):
    samples_list.remove("SimpleRead.net")
    samples_list.remove("SimpleViewer.net")
    samples_list.remove("UserTracker.net")

print "Samples:", samples_list

for sample in samples_list:
    shutil.copytree("../../Samples/" + sample, "Redist/Samples/" + sample)
    shutil.copy("Build/Samples/"+ sample + "/Makefile", "Redist/Samples/"+ sample)

#data
shutil.copy("../../Data/SamplesConfig.xml", "Redist/Samples/Config/SamplesConfig.xml")

#res
res_files = os.listdir("Build/Res")
if '.svn' in res_files:
    res_files.remove('.svn')
for res_file in res_files:
    shutil.copy("Build/Res/" + res_file, "Redist/Samples/Res")

# remove all .svn files
os.system("find Redist/. | grep .svn | xargs rm -rf")

# remove all .svn files
os.system("find Redist/Samples/. | grep .svn | xargs rm -rf")

#-----Remove Read Only Attrib--------------------------------------------------#
print "* Removing Read Only Attributes..."
logger.info("Removing Read Only Attributes...")
os.system ("chmod -R +r Redist/*")

#--------Fixing Files----------------------------------------------------------#
print "* Fixing Files..."
logger.info("Fixing Files...")
os.path.walk("Redist/Samples",fix_file,'')

#-------Creating project and solutions-----------------------------------------#
print "* Creating Makefile..."
logger.info("Creating Makefile...")

MAKEFILE = open("Redist/Samples/Build/Makefile", 'w')
MAKEFILE.write("include Common/CommonDefs.mak\n\n")
MAKEFILE.write(".PHONY: all\n\n")
MAKEFILE.write("NETPROJ = \n")

MAKEFILE.write("ifneq \"$(realpath /usr/bin/gmcs)\" \"\"\n");
for sample in samples_list:
    if sample.find(".net") >0:
        MAKEFILE.write("\tNETPROJ += " + sample + "\n")
MAKEFILE.write("endif\n\n");

MAKEFILE.write("all: $(NETPROJ) ")
for sample in samples_list:
    if sample.find(".net") == -1:
        MAKEFILE.write(sample + " ")
MAKEFILE.write("\n\n")

for sample in samples_list:
    MAKEFILE.write("\n")
    MAKEFILE.write(".PHONY: "+sample+"\n")
    MAKEFILE.write(sample+":\n")
    MAKEFILE.write("\t$(MAKE) -C ../"+sample+"\n")
    
# Close files
MAKEFILE.close()

#-------Copy install script---------------------------------------------------#
print "* Copying install script..."
logger.info("Copying install script...")

copy_install_script(platform, "CreateRedist/install.sh", "Redist")

#-------------Build Samples---------------------------------------------------#
print "* Building Samples in release configuration......"
logger.info("Building Samples in release configuration...")

# Build project solution
result = os.system("make PLATFORM=" + platform + " -C Redist/Samples/Build " + " > "+SCRIPT_DIR+"/Output/BuildSmpRelease.txt")

# Get the build output
lines = open(SCRIPT_DIR+"/Output/BuildSmpRelease.txt").readlines()
build_result = lines[-1]
print(build_result)
logger.info(build_result)

# Check for failed build
if result != 0:
    print "Samples Building In Release Failed!!"
    logger.critical("Samples Building Failed!")
    finish_script(1)

print "* Building Samples in debug configuration......"
logger.info("Building Samples in debug configuration...")

# Build project solution
result = os.system("make CFG=Debug -C Redist/Samples/Build > "+SCRIPT_DIR+"/Output/BuildSmpDebug.txt")

# Get the build output
lines = open(SCRIPT_DIR+"/Output/BuildSmpDebug.txt").readlines()
build_result = lines[-1]
print(build_result)
logger.info(build_result)

# Check for failed build
if result != 0:
    print "Samples Building In Debug Failed!!"
    logger.critical("Samples Building Failed!")
    finish_script(1)

# delete intermidiate files
for sample in samples_list:
   os.system("rm -rf Redist/Samples/"+sample+"/Debug")
   os.system("rm -rf Redist/Samples/"+sample+"/Release")

#--------Fixing Platform Specific Files----------------------------------------#
print "* Fixing Platform-Specific Files..."
logger.info("Fixing Platform-Specific Files...")
os.path.walk("Redist/Samples/Build",fix_platform_specific_files,'')

#-------------Create TAR-------------------------------------------------------#
print "* Creating tar......"
logger.info("Creating tar...")

os.chdir("Redist")
os.makedirs(SCRIPT_DIR+"/Final")

if ostype == "Darwin":
    TAR_TARGET = "MacOSX"
elif platform == 'CE4100':
    TAR_TARGET = "CE4100"
elif platform == 'Arm':
    TAR_TARGET = "Arm"
elif machinetype == "i686":
    TAR_TARGET = "Linux32"
elif machinetype == "x86_64":
    TAR_TARGET = "Linux64"
else:
    TAR_TARGET = "Linux"

result = os.system("tar -cjf " +SCRIPT_DIR+"/Final/OpenNI-Bin-" + TAR_TARGET + "-v" + version + ".tar.bz2 *")

if result != 0:
    print "Tar failed!!"
    logger.critical("Creating Tar Failed!")
    finish_script(1)

#-------------CleanUP----------------------------------------------------------#
print "* Redist OpenNi Ended.   !!"
logger.info("Redist OpenNi Ended.")
finish_script(0)


