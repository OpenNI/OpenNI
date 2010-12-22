#/****************************************************************************
#*                                                                           *
#*  OpenNI 1.0 Alpha                                                         *
#*  Copyright (C) 2010 PrimeSense Ltd.                                       *
#*                                                                           *
#*  This file is part of OpenNI.                                             *
#*                                                                           *
#*  OpenNI is free software: you can redistribute it and/or modify           *
#*  it under the terms of the GNU Lesser General Public License as published *
#*  by the Free Software Foundation, either version 3 of the License, or     *
#*  (at your option) any later version.                                      *
#*                                                                           *
#*  OpenNI is distributed in the hope that it will be useful,                *
#*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
#*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
#*  GNU Lesser General Public License for more details.                      *
#*                                                                           *
#*  You should have received a copy of the GNU Lesser General Public License *
#*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
#*                                                                           *
#****************************************************************************/
#


#-------------Imports----------------------------------------------------------#
from xml.dom.minidom import parse, parseString
import win32con,pywintypes,win32api
from time import strftime
import logging
import glob
import os
import re
import sys

#-------------Functions--------------------------------------------------------#

def finish_script(exit_code):
    os.chdir(SCRIPT_DIR)
    #logging.shutdown()
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
    if os.path.exists(sample_dir + "\\.redist"):
        redistFile = open(sample_dir + "\\.redist")
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
        filePath = dirname + "\\" + filename
        ext = ['icproj','vcproj','csproj','cpp','h','ini','cs']
        if os.path.splitext(filename)[1][1:] in ext:
            print("Fixing: " + filePath)
            tempName=filePath+'~~~'
            input = open(filePath)
            output = open(tempName,'w')
            for s in input:
                olds = s
                s = re.sub(r"..\\..\\..\\..\\..\\",r"..\\..\\",s)
                s = re.sub(r"../../../../../",r"../../",s)
                s = re.sub(r"../../../Bin/",r"../Bin/",s)
                s = re.sub(r"..\\..\\..\\Bin\\",r"../Bin/",s)				
                s = re.sub(r"..\\\\..\\\\..\\\\..\\\\Data\\",r"..\\\\..\\\\..\\\\Data\\",s)
                s = re.sub(r"..\\..\\..\\..\\Data\\",r"..\\..\\..\\Data\\",s)
                s = re.sub("../../../../Data/",r"../../../Data/",s)
                s = re.sub(r"..\\..\\Res\\",r"..\\Res\\",s)
                s = re.sub(r"../../../Lib/\$\(ConfigurationName\)",r"../../Lib/$(ConfigurationName);../../Lib/",s)
                s = re.sub(r"..\\..\\..\\Lib\\\$\(ConfigurationName\)",r"../../Lib/$(ConfigurationName);../../Lib/",s)

                if (re.search("SccProjectName=\"",s)!=None) | (re.search("SSccAuxPath=\"",s)!=None) | \
                                             (re.search("SccLocalPath=\"",s)!=None):
                    s=""
                output.write(s)
                
                if s != olds:
                    print("Changed : " + olds.strip("\n"))
                    print("To      : " + s.strip("\n"))
            output.close()
            input.close()
            os.remove(filePath)
            os.rename(tempName,filePath)

def find_proj(arg,dirname,fname):
    # gives the list of all the vcproj files in fname
    for filename in fname:
        ext = os.path.splitext(filename)[1]
        if ext == '.vcproj' or ext == '.csproj':
            filePath = dirname + "\\" + filename
            samples_proj_list.append(filePath)

def get_reg_values(reg_key, value_list):
    # open the reg key
    try:
        reg_key = win32api.RegOpenKeyEx(*reg_key)
    except pywintypes.error as e:
        raise Exception("Failed to open registry key!")
    # Get the values
    try:
        values = [(win32api.RegQueryValueEx(reg_key, name), data_type) for name, data_type in value_list]
        # values list of ((value, type), expected_type)
        for (value, data_type), expected in values:
            if data_type != expected:
                raise Exception("Bad registry value type! Expected %d, got %d instead." % (expected, data_type))
        # values okay, leave only values
        values = [value for ((value, data_type), expected) in values]
    except pywintypes.error as e:
        raise Exception("Failed to get registry value!")
    finally:
        try:
            win32api.RegCloseKey(reg_key)
        except pywintypes.error as e:
            # We don't care if reg key close failed...
            pass
    return tuple(values)

#------------Constants and globals---------------------------------------------#
try:
    MSVC_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\VisualStudio\9.0")
    MSVC_VALUES = [("InstallDir", win32con.REG_SZ)]
    VS_INST_DIR = get_reg_values(MSVC_KEY, MSVC_VALUES)[0]
    VS_NEED_UPGRADE = 0
except Exception as e:
    MSVC_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\VisualStudio\10.0")
    MSVC_VALUES = [("InstallDir", win32con.REG_SZ)]
    VS_INST_DIR = get_reg_values(MSVC_KEY, MSVC_VALUES)[0]
    VS_NEED_UPGRADE = 1

NSIS_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\NSIS")
NSIS_VALUES = [("InstallLocation", win32con.REG_EXPAND_SZ)]
NSIS_INST_DIR = get_reg_values(NSIS_KEY, NSIS_VALUES)[0]
DateTimeSTR = strftime("%Y-%m-%d %H:%M:%S")
DateSTR = strftime("%Y-%m-%d")
SCRIPT_DIR = os.getcwd()
CONFIG_XML = parse("OpenNi_Config.xml")
WORK_DIR = str(CONFIG_XML.getElementsByTagName("WORK_DIR")[0].firstChild.data)
# Fix to allow reletive path
os.chdir(WORK_DIR)
WORK_DIR = os.getcwd() + "\\"
os.chdir(SCRIPT_DIR)
OPENNI_VER = str(CONFIG_XML.getElementsByTagName("VERSION_NUMBER")[0].firstChild.data)
PLATFORM = str(CONFIG_XML.getElementsByTagName("PLATFORM")[0].firstChild.data)
PROJECT_SLN = str(CONFIG_XML.getElementsByTagName("PROJECT_SLN")[0].firstChild.data)
SAMPLES_SLN = str(CONFIG_XML.getElementsByTagName("SAMPLES_SLN")[0].firstChild.data)
PROJECT_NAME = str(CONFIG_XML.getElementsByTagName("PROJECT")[0].firstChild.data)
ver_regx = re.compile("SDK \d.*\s")
global samples_proj_list
samples_proj_list = []

#-------------Log--------------------------------------------------------------#
if not(os.path.exists(SCRIPT_DIR + "\\Output\\")):
    os.mkdir(SCRIPT_DIR + "\\Output\\")
logger = logging.getLogger('myapp')
hdlr = logging.FileHandler('Output\\Nightly.log')
formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
hdlr.setFormatter(formatter)
logger.addHandler(hdlr)
logger.setLevel(logging.INFO)

#------------Welcome Messege--------=------------------------------------------#
print("\n");
print("*********************************")
print("*   PrimeSense OpenNI Redist    *")
print("*     " + DateTimeSTR + "       *")
print("*********************************")
logger.info("PrimeSense OpenNI Redist Started")
# Create output dir


#--------------Build Project---------------------------------------------------#
print("* Building " + PROJECT_NAME + "...")
logger.info("Building " + PROJECT_NAME + "...")
# Set Intel Env
os.system("set INTEL_LICENSE_FILE=C:\\Program Files\\Common Files\\Intel\\Licenses")
# Build project solution
os.chdir(WORK_DIR + PROJECT_SLN.rpartition("\\")[0])
if VS_NEED_UPGRADE == 1:
    os.system("\""+VS_INST_DIR + "devenv\" " + PROJECT_SLN.rpartition("\\")[2]+\
                 " /upgrade > ..\\CreateRedist\\Output\\Build"+PROJECT_NAME+".txt")

#print "\""+VS_INST_DIR + "devenv\" " + PROJECT_SLN.rpartition("\\")[2]+" /build release > \""+SCRIPT_DIR+"\\Output\\Build"+PROJECT_NAME+".txt\""
    
os.system('\"'+VS_INST_DIR + 'devenv\" ' + PROJECT_SLN.rpartition("\\")[2]+\
             " /build release > ..\\CreateRedist\\Output\\Build"+PROJECT_NAME+".txt")

# Get the build output
lines = open(SCRIPT_DIR+"\\Output\\Build"+PROJECT_NAME+".txt").readlines()
build_result = lines[-1]
print(build_result)
logger.info(build_result)
# Check for failed build
failed_builds = int(re.search("(\d*) failed",build_result).group(1))
if failed_builds != 0:
    print("Building Failed!!")
    logger.critical("Building Failed!")
    finish_script(1)



if len(sys.argv) != 2:
    #--------------Doxygen---------------------------------------------------------#
    print("* Creating Doxygen...")
    logger.info("Creating Doxygen...")
    os.chdir(WORK_DIR + "Source\\Doxygen");
    # Replacing version number in the doxygen setup file
    os.system("attrib -r Doxyfile")
    regx_replace("OpenNI \d*.\d*.\d*\s",PROJECT_NAME +" " + OPENNI_VER + " ",'Doxyfile')
    if os.path.exists(WORK_DIR + "\\Source\\Doxygen\\html\\"):
        os.system("rmdir /S /Q html")
    # Running doxygen
    os.system("mkdir html")
    os.system("copy PSSmallLogo.jpg html")
    os.system("doxygen.exe Doxyfile > ..\\..\\Platform\Win32\\CreateRedist\\Output\\EngineDoxy.txt")
    os.system("""copy "html\*.chm" ..\..\Documentation""")  
else:
    print("Skipping Doxygen...")

#-------------Create Redist Dir------------------------------------------------#
print("* Creating Redist Dir...")
logger.info("Creating Redist Dir...")
os.chdir(WORK_DIR + "\\Platform\\Win32")
# Removing the old directory
os.system("rmdir /S /Q Redist")
# Creating new directory tree
os.system("mkdir Redist")
os.system("mkdir Redist\\Bin")
os.system("mkdir Redist\\Lib")
os.system("mkdir Redist\\Include")
os.system("mkdir Redist\\Documentation")
os.system("mkdir Redist\\Samples")
os.system("mkdir Redist\\Samples\\Bin")
os.system("mkdir Redist\\Samples\\Bin\\Debug")
os.system("mkdir Redist\\Samples\\Bin\\Release")
os.system("mkdir Redist\\Samples\\Build")
os.system("mkdir Redist\\Samples\\Res")
os.system("mkdir Redist\\Data")
os.system("mkdir Redist\\Tools")
#os.system("mkdir Redist\\Wrappers")

#os.system("mkdir Redist\\Driver")

#-------------Copy files to redist---------------------------------------------#
print("* Copying files to redist dir...")
logger.info("Copying files to redist dir...")

#license
os.system ("copy \"..\\..\\GPL.txt\" Redist")
os.system ("copy \"..\\..\\LGPL.txt\" Redist")

#bin
os.system ("copy Bin\\Release\\OpenNI.dll Redist\\Bin")
os.system ("copy Bin\\Release\\OpenNI.net.dll Redist\\Bin")
os.system ("copy Bin\\Release\\nimCodecs.dll Redist\\Bin")
os.system ("copy Bin\\Release\\nimMockNodes.dll Redist\\Bin")
os.system ("copy Bin\\Release\\nimRecorder.dll Redist\\Bin")
os.system ("copy Bin\\Release\\niReg.exe Redist\\Bin")
os.system ("copy Bin\\Release\\niLicense.exe Redist\\Bin")

#lib
os.system ("copy Lib\\Release\\*.* Redist\\Lib")

#docs
os.system ("copy \"..\\..\\Documentation\\*.pdf\" Redist\\Documentation")
os.system ("copy \"..\\..\\Documentation\\*.chi\" Redist\\Documentation")
os.system ("copy \"..\\..\\Documentation\\*.chm\" Redist\\Documentation")

#include
os.system ("xcopy /S ..\\..\\Include\\* Redist\\Include\\")

#driver
os.system ("xcopy /S Driver\\Bin\\*.* Redist\\Driver\\")
#tools
os.system ("copy Tools\\*.* Redist\\Tools")

samples_list = os.listdir("..\\..\\Samples")
print(samples_list)
if '.svn' in samples_list:
    samples_list.remove('.svn')

for sample in samples_list:
    # Check if the sample is a tool or should'nt be distributed in this conf
    #if sample.startswith("Sample-") == False:
        #continue;
    #rc = check_sample(WORK_DIR + "Samples\\" + sample)
    # Remove the checking, samples should allways be compiles
    #rc = 0
    #if rc==0:
    os.system ("mkdir Redist\\Samples\\"+sample)
    #os.system ("xcopy /S Samples\\" + sample + "\\*.h Redist\\Samples\\"+ sample)
    os.system ("xcopy /S ..\\..\\Samples\\" + sample + " Redist\\Samples\\"+ sample)

    #os.system ("xcopy /S Samples\\" + sample + "\\*.cpp Redist\\Samples\\"+ sample)

    #os.system ("xcopy /S Samples\\"+ sample + "\\*.vcproj Redist\\Samples\\"+ sample + "\\")
    #os.system ("xcopy /S Samples\\"+ sample + "\\*.lib Redist\\Samples\\"+ sample + "\\")

    os.system ("xcopy /S Build\\Samples\\"+ sample + "\\*.icproj Redist\\Samples\\"+ sample + "\\")
    os.system ("xcopy /S Build\\Samples\\"+ sample + "\\*.vcproj Redist\\Samples\\"+ sample + "\\")
    os.system ("xcopy /S Build\\Samples\\"+ sample + "\\*.csproj Redist\\Samples\\"+ sample + "\\")

    #if rc==2:
    #    system("copy Bin\\Release\\Sample-$Sample.exe Redist\\Samples\\Bin\\Release\\" + sample + ".exe")
        #break

os.system ("copy Redist\\Samples\\Bin\\Release\\*.* Redist\\Samples\\Bin\\Debug")
os.system ("xcopy /S Build\\Res\\*.* Redist\\Samples\\Res\\")

#data
os.system ("copy ..\\..\\Data\\SamplesConfig.xml Redist\\Data\\SamplesConfig.xml")

# Copy the glut dll to the samples
os.system ("xcopy /S .\\Bin\\Release\\glut32.dll Redist\\Samples\\Bin\\Release\\")
os.system ("xcopy /S .\\Bin\\Release\\glut32.dll Redist\\Samples\\Bin\\Debug\\")
os.system ("xcopy /S .\\Bin\\Release\\OpenNI.net.dll Redist\\Samples\\Bin\\Release\\")
os.system ("xcopy /S .\\Bin\\Release\\OpenNI.net.dll Redist\\Samples\\Bin\\Debug\\")
# Copy the release notes
os.system ("copy .\\ReleaseNotes.txt Redist\\Documentation\\")


#-----Remove Read Only Attrib--------------------------------------------------#
print("* Removing Read Only Attributes...")
logger.info("Removing Read Only Attributes...")
os.system ("attrib -r -h -s /S Redist\\*.*")


#--------Fixing Files----------------------------------------------------------#
print("* Fixing Files...")
logger.info("Fixing Files...")
for dirpath, dirnames, filenames in os.walk('Redist\\'):
    fix_file('', dirpath, dirnames + filenames)

#-------Creating project and solutions-----------------------------------------#
print("* Creating project and solutions...")
logger.info("Creating project and solutions...")
# find vcproj files
for dirpath, dirnames, filenames in os.walk('Redist\\'):
    find_proj('', dirpath, dirnames + filenames)

# open all solution files
OUTFILESLN2008 = open("Redist\\Samples\\Build\\All_2008.sln",'w')
OUTFILESLN2008.write("Microsoft Visual Studio Solution File, Format Version 10.00\n")
OUTFILESLN2008.write("# Visual Studio 2008\n")
OUTFILESLN2010 = open("Redist\\Samples\\Build\\All_2010.sln",'w')
OUTFILESLN2010.write("Microsoft Visual Studio Solution File, Format Version 11.00\n")
OUTFILESLN2010.write("# Visual Studio 2010\n")
# Do this for each project
for prj2008_filename in samples_proj_list:
    ProjName = ""
    ProjGUID = ""
    print(prj2008_filename)
    prj_name_partitioned = os.path.splitext(prj2008_filename);
    prj2010_filename = prj_name_partitioned[0] + "_2010" + prj_name_partitioned[1]
    prj2008 = open(prj2008_filename,'r')
    prj2010 = open(prj2010_filename,'w')
    lines = prj2008.readlines()
    for line in lines:
        # Search for name and guid in the project file
        ProjNametmp = re.search(r"Name=\"(.*)\"",line)
        if (ProjNametmp!=None):
            if (ProjName==""):
                ProjName = ProjNametmp.group(1)
        ProjGUIDtmp = re.search(r"ProjectGUID=\"(.*)\"",line)
        if (ProjGUIDtmp!=None):
            if (ProjGUID==""):
                ProjGUID = ProjGUIDtmp.group(1)
        ProjNametmp = re.search(r"<AssemblyName>(.*)</AssemblyName>",line)
        if (ProjNametmp!=None):
            if (ProjName==""):
                ProjName = ProjNametmp.group(1)
        ProjGUIDtmp = re.search(r"<ProjectGuid>(.*)</ProjectGuid>",line)
        if (ProjGUIDtmp!=None):
            if (ProjGUID==""):
                ProjGUID = ProjGUIDtmp.group(1)
        prj2010.write(line)
    prj2008.close()
    prj2010.close()
    # rename prj file to _2008.vcproj
    origname = prj2008_filename
    prj2008_filename = prj_name_partitioned[0] + "_2008" + prj_name_partitioned[1]
    os.rename(origname,prj2008_filename)
    # create reletive path to samples
    prj2008_path = "..\\" + prj2008_filename.partition("\\")[2].partition("\\")[2]
    prj2010_path = "..\\" + prj2010_filename.partition("\\")[2].partition("\\")[2]
    # add project to all samples project
    OUTFILESLN2008.write("Project(\"{19091980-2008-4CFA-1491-04CC20D8BCF9}\") = \""+\
                                ProjName+"\", \""+prj2008_path+"\", \""+ProjGUID+"\"\n")
    OUTFILESLN2008.write("EndProject\n")
    OUTFILESLN2010.write("Project(\"{19091980-2008-4CFA-1491-04CC20D8BCF9}\") = \""+\
                                ProjName+"\", \""+prj2010_path+"\", \""+ProjGUID+"\"\n")
    OUTFILESLN2010.write("EndProject\n")    
# Close files
OUTFILESLN2008.close()
OUTFILESLN2010.close()

#-------------Build Samples---------------------------------------------------#
print("* Building Samples in release configuration......")
logger.info("Building Samples in release configuration...")
# Build project solution
os.chdir(WORK_DIR + SAMPLES_SLN.rpartition("\\")[0])

if VS_NEED_UPGRADE == 1:
    os.system("\""+VS_INST_DIR + "devenv\" " + SAMPLES_SLN.rpartition("\\")[2]+\
                           " /upgrade > ..\\..\\..\\CreateRedist\\Output\\EngineSmpRelease.txt")

os.system("\""+VS_INST_DIR + "devenv\" " + SAMPLES_SLN.rpartition("\\")[2]+\
                 " /build release > ..\\..\\..\\CreateRedist\\Output\\EngineSmpRelease.txt")

# Get the build output
lines = open(SCRIPT_DIR+"\\Output\\EngineSmpRelease.txt").readlines()
build_result = lines[-1]
print(build_result)
logger.info(build_result)
# Check for failed build
failed_builds = int(re.search("(\d*) failed",build_result).group(1))
if failed_builds != 0:
    print("Samples Building In Release Failed!!")
    logger.critical("Samples Building Failed!")
#    finish_script(1)

print("* Building Samples in debug configuration......")
logger.info("Building Samples in debug configuration...")
# Build project solution
os.chdir(WORK_DIR + SAMPLES_SLN.rpartition("\\")[0])

if VS_NEED_UPGRADE == 1:
    os.system("\""+VS_INST_DIR + "devenv\" " + SAMPLES_SLN.rpartition("\\")[2]+\
                     " /upgrade > ..\\..\\..\\CreateRedist\\Output\\EngineSmpDebug.txt")

os.system("\""+VS_INST_DIR + "devenv\" " + SAMPLES_SLN.rpartition("\\")[2]+\
                 " /build debug > ..\\..\\..\\CreateRedist\\Output\\EngineSmpDebug.txt")

# Get the build output
lines = open(SCRIPT_DIR+"\\Output\\EngineSmpDebug.txt").readlines()
build_result = lines[-1]
print(build_result)
logger.info(build_result)
# Check for failed build
failed_builds = int(re.search("(\d*) failed",build_result).group(1))
if failed_builds != 0:
    print("Samples Building In Debug Failed!!")
    logger.critical("Samples Building Failed!")
#    finish_script(1)

# --------------------Delete stuff
os.chdir(WORK_DIR + "\\Platform\\Win32\\Redist\\Samples\\Bin\\Release\\")
os.system("del *.pdb")
os.chdir(WORK_DIR + "\\Platform\\Win32\\Redist\\Samples\\Bin\\Debug\\")
os.system("del *.pdb")
os.system("del *.ilk")
os.chdir(WORK_DIR + "\\Platform\\Win32\\Redist\\Lib\\")
os.system("del nim*.*")

#-------------Make installer---------------------------------------------------#
print("* Making Installer...")
logger.info("Making Installer...")
os.chdir(WORK_DIR + "Platform\\Win32\\Install\\")
# Replace version in the NSI script
os.system("attrib -r OpenNi.nsi")
regx_replace("OPENNI_VER \"\d*.\d*.\d*\"","OPENNI_VER \"" + OPENNI_VER + "\"",'OpenNi.nsi')
# make installer
os.system("\""+NSIS_INST_DIR + "\\makensis.exe\" OpenNi.nsi > ..\\CreateRedist\\Output\\OpenNiNSIS.txt")
os.system("move .\\Output\\*.exe ..\\CreateRedist\\Output")

#-------------CleanUP----------------------------------------------------------#
print("* Redist OpenNi Ended.   !!")
logger.info("Redist OpenNi Ended.")
finish_script(0)


