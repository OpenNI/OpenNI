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
from commands import getoutput as gop

#-------------Functions--------------------------------------------------------#

def calc_jobs_number():
    cores = 1
	
    try:
        if ostype == "Darwin":
            txt = gop('sysctl -n hw.physicalcpu')
        else:		
            txt = gop('grep "processor\W:" /proc/cpuinfo | wc -l')
			
        cores = int(txt)
    except:
        pass
       
    return str(cores * 2)

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

        ext = ['c','cpp','h','ini','cs','java']
        if filename == "Makefile" or filename.partition(".")[2] in ext:
            #print "Fixing: " + filePath
            tempName=filePath+'~~~'
            input = open(filePath)
            output = open(tempName,'w')
            for s in input:
                olds = s
                s = re.sub(r"../../../Bin",r"../Bin",s)
                s = re.sub(r"../../../../../Include",r"../../Include =/usr/include/ni",s)
                s = re.sub(r"../../../../../Samples/[\w.]+/?",r"./",s)
                s = re.sub(r"../../../../Data/SamplesConfig.xml",r"../../Config/SamplesConfig.xml",s)
                s = re.sub(r"../../Res/",r"../Res/",s)
                s = re.sub(r"include ../../Common/CommonDefs.mak",r"include ../Build/Common/CommonDefs.mak",s)
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
DateTimeSTR = strftime("%Y-%m-%d %H:%M:%S")
DateSTR = strftime("%Y-%m-%d")
SCRIPT_DIR = os.getcwd()
# Fix to allow reletive path
WORK_DIR = os.getcwd() + "/"
os.chdir(SCRIPT_DIR)
PROJECT_NAME = "OpenNI"
ostype = os.popen('uname -s').read().rstrip()
machinetype = os.popen('uname -m').read().rstrip()

if len(sys.argv) >= 2:
    PLATFORM = sys.argv[1]
    MAKE_ARGS = 'PLATFORM=' + PLATFORM
else:
    if machinetype == "x86_64":
        PLATFORM = "x64"
    elif machinetype == "x86" or machinetype == "i386" or machinetype == "i686":
        PLATFORM = "x86"
    elif machinetype[:3] == "arm":
        PLATFORM = "Arm"
    else:
        print "Unknown platform:", machinetype
        finish_script(1)

    MAKE_ARGS = ''

MAKE_ARGS += ' -j' + calc_jobs_number()

if ostype == "Darwin":
    TARGET = "MacOSX"
else:
    TARGET = "Linux-" + PLATFORM

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
print
logger.info("PrimeSense OpenNI Redist Started")

print "Target:", TARGET

#--------------Take Version----------------------------------------------------#
version_file = open("../../../Include/XnVersion.h").read()
major = re.search(r"define XN_MAJOR_VERSION (\d+)", version_file).groups()[0]
minor = re.search(r"define XN_MINOR_VERSION (\d+)", version_file).groups()[0]
maintenance = re.search(r"define XN_MAINTENANCE_VERSION (\d+)", version_file).groups()[0]
build = re.search(r"define XN_BUILD_VERSION (\d+)", version_file).groups()[0]

version = major + "." + minor + "." + maintenance + "." + build
print "Version:", version

print "Num of compile jobs:", calc_jobs_number()

print

#--------------Build Project---------------------------------------------------#
print "* Building OpenNI..."
logger.info("Building OpenNI...")

# Build
#execute_check("gacutil -u OpenNI.net > " + SCRIPT_DIR + "/Output/gacutil.txt", "Remove from GAC")
execute_check('make ' + MAKE_ARGS + ' -C ' + SCRIPT_DIR + '/../Build clean > ' + SCRIPT_DIR + '/Output/Build' + PROJECT_NAME + '_clean.txt', 'Cleaning')
execute_check('make ' + MAKE_ARGS + ' -C ' + SCRIPT_DIR + '/../Build > ' + SCRIPT_DIR + '/Output/Build' + PROJECT_NAME + '.txt', 'Building')

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

REDIST_NAME = "OpenNI-Bin-Dev-" + TARGET + "-v" + version

REDIST_DIR = "Redist/" + REDIST_NAME

# Removing the old directory
if (os.path.exists(REDIST_DIR)):
    shutil.rmtree(REDIST_DIR)

# Creating new directory tree
os.makedirs(REDIST_DIR)
os.makedirs(REDIST_DIR + "/Bin")
os.makedirs(REDIST_DIR + "/Lib")
os.makedirs(REDIST_DIR + "/Jar")
os.makedirs(REDIST_DIR + "/Include")
os.makedirs(REDIST_DIR + "/Documentation")
os.makedirs(REDIST_DIR + "/Samples")
os.makedirs(REDIST_DIR + "/Samples/Bin")
os.makedirs(REDIST_DIR + "/Samples/Bin/" + PLATFORM + "-Debug")
os.makedirs(REDIST_DIR + "/Samples/Bin/" + PLATFORM + "-Release")
os.makedirs(REDIST_DIR + "/Samples/Build")
os.makedirs(REDIST_DIR + "/Samples/Config")
os.makedirs(REDIST_DIR + "/Samples/Res")

#-------------Copy files to redist---------------------------------------------#
print "* Copying files to redist dir..."
logger.info("Copying files to redist dir...")

#license
shutil.copy("../../LICENSE", REDIST_DIR)
shutil.copy("../../NOTICE", REDIST_DIR)

#lib
if ostype == "Darwin":
    LIBS_TYPE = ".dylib"
else:
    LIBS_TYPE = ".so"

shutil.copy("Bin/" + PLATFORM + "-Release/libnimCodecs"+LIBS_TYPE, REDIST_DIR + "/Lib")
shutil.copy("Bin/" + PLATFORM + "-Release/libnimMockNodes"+LIBS_TYPE, REDIST_DIR + "/Lib")
shutil.copy("Bin/" + PLATFORM + "-Release/libnimRecorder"+LIBS_TYPE, REDIST_DIR + "/Lib")
shutil.copy("Bin/" + PLATFORM + "-Release/libOpenNI"+LIBS_TYPE, REDIST_DIR + "/Lib")
shutil.copy("Bin/" + PLATFORM + "-Release/libOpenNI.jni"+LIBS_TYPE, REDIST_DIR + "/Lib")

#bin
MonoDetected = 0
shutil.copy("Bin/" + PLATFORM + "-Release/niReg", REDIST_DIR + "/Bin")
shutil.copy("Bin/" + PLATFORM + "-Release/niLicense", REDIST_DIR + "/Bin")
if PLATFORM == 'x86' or PLATFORM == 'x64':
    if (os.path.exists("/usr/bin/gmcs")):
        shutil.copy("Bin/" + PLATFORM + "-Release/OpenNI.net.dll", REDIST_DIR + "/Bin")
        shutil.copy("Bin/" + PLATFORM + "-Release/OpenNI.net.dll", REDIST_DIR + "/Samples/Bin/" + PLATFORM + "-Debug")
        shutil.copy("Bin/" + PLATFORM + "-Release/OpenNI.net.dll", REDIST_DIR + "/Samples/Bin/" + PLATFORM + "-Release")
        MonoDetected = 1
        
# java wrapper
shutil.copy("Bin/" + PLATFORM + "-Release/org.openni.jar", REDIST_DIR + "/Jar")
shutil.copy("Bin/" + PLATFORM + "-Release/org.openni.jar", REDIST_DIR + "/Samples/Bin/" + PLATFORM + "-Debug")
shutil.copy("Bin/" + PLATFORM + "-Release/org.openni.jar", REDIST_DIR + "/Samples/Bin/" + PLATFORM + "-Release")

#docs
shutil.copytree("../../Source/DoxyGen/html", REDIST_DIR + "/Documentation/html")

#include
for includeFile in os.listdir("../../Include"):
    if not os.path.isdir("../../Include/" + includeFile):
        shutil.copy("../../Include/" + includeFile, REDIST_DIR + "/Include")

shutil.copytree("../../Include/Linux-x86", REDIST_DIR + "/Include/Linux-x86")
shutil.copytree("../../Include/Linux-Arm", REDIST_DIR + "/Include/Linux-Arm")
shutil.copytree("../../Include/MacOSX", REDIST_DIR + "/Include/MacOSX")
shutil.copytree("Build/Common", REDIST_DIR + "/Samples/Build/Common")

# samples
samples_list = os.listdir("Build/Samples")
if '.svn' in samples_list:
    samples_list.remove('.svn')

if PLATFORM == 'CE4100':
    samples_list.remove('NiViewer')
    samples_list.remove('NiSimpleViewer')

if PLATFORM == 'Arm':
    samples_list.remove('NiUserTracker')
    samples_list.remove('NiViewer')
    samples_list.remove('NiSimpleViewer')
    samples_list.remove('NiHandTracker')
    samples_list.remove('NiUserSelection')

if (MonoDetected == 0):
    samples_list.remove("SimpleRead.net")
    samples_list.remove("SimpleViewer.net")
    samples_list.remove("UserTracker.net")

print "Samples:", samples_list

for sample in samples_list:
    shutil.copytree("../../Samples/" + sample, REDIST_DIR + "/Samples/" + sample)
    shutil.copy("Build/Samples/"+ sample + "/Makefile", REDIST_DIR + "/Samples/"+ sample)

#data
shutil.copy("../../Data/SamplesConfig.xml", REDIST_DIR + "/Samples/Config/SamplesConfig.xml")

#res
res_files = os.listdir("Build/Res")
if '.svn' in res_files:
    res_files.remove('.svn')
for res_file in res_files:
    shutil.copy("Build/Res/" + res_file, REDIST_DIR + "/Samples/Res")

# remove all .svn files
os.system("find " + REDIST_DIR + "/. | grep .svn | xargs rm -rf")

# remove all .svn files
os.system("find " + REDIST_DIR + "/Samples/. | grep .svn | xargs rm -rf")

#-----Remove Read Only Attrib--------------------------------------------------#
print "* Removing Read Only Attributes..."
logger.info("Removing Read Only Attributes...")
os.system ("chmod -R +r " + REDIST_DIR + "/*")

#--------Fixing Files----------------------------------------------------------#
print "* Fixing Files..."
logger.info("Fixing Files...")
os.path.walk(REDIST_DIR + "/Samples",fix_file,'')

#-------Creating project and solutions-----------------------------------------#
print "* Creating Makefile..."
logger.info("Creating Makefile...")

MAKEFILE = open(REDIST_DIR + "/Samples/Build/Makefile", 'w')
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

copy_install_script(PLATFORM, "CreateRedist/install.sh", REDIST_DIR)

#-------------Build Samples---------------------------------------------------#
print "* Building Samples in release configuration......"
logger.info("Building Samples in release configuration...")

# Build project solution
execute_check("make " + MAKE_ARGS + " -C " + REDIST_DIR + "/Samples/Build " + " > "+SCRIPT_DIR+"/Output/BuildSmpRelease.txt", "Build samples in release")

print "* Building Samples in debug configuration......"
logger.info("Building Samples in debug configuration...")

# Build project solution
execute_check("make " + MAKE_ARGS + " CFG=Debug -C " + REDIST_DIR + "/Samples/Build > "+SCRIPT_DIR+"/Output/BuildSmpDebug.txt", "Build samples in debug")

# delete intermidiate files
for sample in samples_list:
   os.system("rm -rf " + REDIST_DIR + "/Samples/"+sample+"/" + PLATFORM + "/Debug")
   os.system("rm -rf " + REDIST_DIR + "/Samples/"+sample+"/" + PLATFORM + "/Release")

#-------------Create TAR-------------------------------------------------------#
print "* Creating tar......"
logger.info("Creating tar...")

os.makedirs(SCRIPT_DIR+"/Final")
os.chdir(SCRIPT_DIR + "/../Redist")

execute_check("tar -cjf " +SCRIPT_DIR+"/Final/" + REDIST_NAME + ".tar.bz2 " + REDIST_NAME, "Tar")

os.chdir(SCRIPT_DIR)

#-------------CleanUP----------------------------------------------------------#
print "* Redist OpenNi Ended.   !!"
logger.info("Redist OpenNi Ended.")
finish_script(0)


