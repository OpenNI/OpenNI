#/***************************************************************************
#*                                                                          *
#*  OpenNI 1.1 Alpha                                                        *
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
import win32con,pywintypes,win32api
from time import strftime
import logging
import glob
import os
import re
import sys
import subprocess
import shutil
import stat

#-------------Classes----------------------------------------------------------#
class SampleData(object):
    def __init__(self):
        self.name = ''
        self.project_dir = ''
        self.source_dir = ''
        self.project_guid = ''
        self.project_file = ''
        self.is_net = False
        self.project_name = ''
        self.redist_dir = ''
        self.dependencies = []

    def print_():
        print self.name
    def __str__(self):
        return self.name
    def __eval__(self):
        return self.name

#-------------Functions--------------------------------------------------------#
def find_samples():
    # returns a dictionary of all samples
    all_samples = dict()
    samples_list = os.listdir("..\\..\\Samples")
    if '.svn' in samples_list:
        samples_list.remove('.svn')
    print(samples_list)
    for sample in samples_list:
        sample_data = SampleData()
        sample_data.name = sample
        sample_data.source_dir = "..\\..\\Samples\\" + sample
        sample_data.project_dir = "Build\\Samples\\" + sample

        vc_proj_name = sample_data.project_dir + "\\" + sample + ".vcproj"
        cs_proj_name = sample_data.project_dir + "\\" + sample + ".csproj"

        # check if this is a VC project
        if os.path.exists(vc_proj_name):
            sample_data.project_file = vc_proj_name

            # open it
            prj = open(vc_proj_name, 'r')
            lines = prj.readlines()
            for line in lines:
                # Search for name
                if sample_data.project_name == "":
                    ProjNametmp = re.search(r"Name=\"(.*)\"",line)
                    if (ProjNametmp != None):
                        sample_data.project_name = ProjNametmp.group(1)

                # Search for GUID
                if sample_data.project_guid == "":
                    ProjGUIDtmp = re.search(r"ProjectGUID=\"(.*)\"",line)
                    if (ProjGUIDtmp != None):
                        sample_data.project_guid = ProjGUIDtmp.group(1)

                ProjNametmp = re.search(r"<AssemblyName>(.*)</AssemblyName>",line)
                if (ProjNametmp!=None):
                    if (ProjName==""):
                        ProjName = ProjNametmp.group(1)
                        ProjIsNET = 1
                ProjGUIDtmp = re.search(r"<ProjectGuid>(.*)</ProjectGuid>",line)
                if (ProjGUIDtmp!=None):
                    if (ProjGUID==""):
                        ProjGUID = ProjGUIDtmp.group(1)

            prj.close()

        # or a .NET project
        elif os.path.exists(cs_proj_name):
            sample_data.project_file = cs_proj_name
            sample_data.is_net = True
            # open it, read GUID
            prj = open(cs_proj_name, 'r')

            # open it
            prj = open(cs_proj_name, 'r')
            lines = prj.readlines()
            for line in lines:
                # Search for name
                if sample_data.project_name == "":
                    ProjNametmp = re.search(r"<AssemblyName>(.*)</AssemblyName>",line)
                    if (ProjNametmp != None):
                        sample_data.project_name = ProjNametmp.group(1)

                # Search for GUID
                if sample_data.project_guid == "":
                    ProjGUIDtmp = re.search(r"<ProjectGuid>(.*)</ProjectGuid>",line)
                    if (ProjGUIDtmp != None):
                        sample_data.project_guid = ProjGUIDtmp.group(1)

            prj.close()

        else:
            print('Sample ' + sample + ' does not have a valid project file')
            logger.critical('Sample ' + sample + ' does not have a valid project file')
            finish_script(1)

        # check if it has a special configuration
        redist_file_name = sample_data.source_dir + "\\.redist"
        if os.path.exists(redist_file_name):
            redist_file = open(redist_file_name, 'r')

            for line in redist_file.readlines():
                # seach for dependencies
                match = re.search("^DEPENDS=(.*)$", line)
                if match != None:
                    sample_data.dependencies.append(match.group(1))

            redist_file.close()

        sample_data.redist_dir = "Redist\\Samples\\" + sample_data.name
        all_samples[sample_data.name] = sample_data

    return all_samples

def finish_script(exit_code):
    os.chdir(SCRIPT_DIR)
    #logging.shutdown()
    exit(exit_code)

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

def fix_file(arg,dirname,fname):
    "Fixes paths for all the files in fname"
    for filename in fname:
        filePath = dirname + "\\" + filename
        ext = ['icproj','vcproj','csproj','cpp','h','ini','cs']
        file_ext = os.path.splitext(filename)[1][1:]
        if file_ext in ext:
            #print("Fixing: " + filePath)
            file = open(filePath, "r+")
            s = file.read()
            file.seek(0)

            olds = s
            s = re.sub(r"..\\..\\..\\..\\..\\",r"..\\..\\",s)
            s = re.sub(r"../../../../../",r"../../",s)
            s = re.sub(r"..\\\\..\\\\..\\\\..\\\\Data\\",r"..\\\\..\\\\..\\\\Data\\",s)
            s = re.sub(r"..\\..\\..\\..\\Data\\",r"..\\..\\..\\Data\\",s)
            s = re.sub("../../../../Data/",r"../../../Data/",s)
            s = re.sub(r"..\\..\\Res\\",r"..\\Res\\",s)
            if vc_build_bits=="32":
                s = re.sub(r"../../../Bin/",r"../Bin/",s)
                s = re.sub(r"..\\..\\..\\Bin\\",r"../Bin/",s)
                s = re.sub(r"../../../Lib/\$\(ConfigurationName\)",r"../../Lib/",s)
                s = re.sub(r"..\\..\\..\\Lib\\\$\(ConfigurationName\)",r"../../Lib/",s)
            else:
                s = re.sub(r"../../../Bin64/",r"../Bin64/",s)
                s = re.sub(r"..\\..\\..\\Bin64\\",r"../Bin64/",s)
                s = re.sub(r"../../../Lib64/\$\(ConfigurationName\)",r"../../Lib64/",s)
                s = re.sub(r"..\\..\\..\\Lib64\\\$\(ConfigurationName\)",r";../../Lib64/",s)

            s = re.sub(r".*SccProjectName=\".*", r"", s);
            s = re.sub(r".*SSccAuxPath=\".*", r"", s);
            s = re.sub(r".*SccLocalPath=\".*", r"", s);
            
            # fix csproj link problem
            if file_ext == "csproj":
                sample_name = os.path.basename(dirname)
                link_re = r"<Compile Include=\"..\\..\\Samples\\" + sample_name + r"\\.*\">\n"
                link_re += r"\s*<Link>(.*)</Link>"
                compiled_re = re.compile(link_re, re.MULTILINE)
                s = compiled_re.sub("<Compile Include=\"\\1\">", s)
            
            file.truncate()
            file.write(s)
            file.close()

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

#------------Check args---------------------------------------------#

if len(sys.argv) not in [4,5]:
    print "Args: <Doxygen:y/n> <BuildTarget:32/64> <FullRebuild:y/n> [<VCVersion:9/10>]"
    exit(1)

if sys.argv[1] == 'y':
    Make_Doxy=1
elif sys.argv[1] == 'n':
    Make_Doxy=0
else:
    print "Args: <Doxygen:y/n> <BuildTarget:32/64> <FullRebuild:y/n>"
    print "Doxygen param must be y or n!"
    exit(1)

if sys.argv[2] == '32':
    vc_build_bits = "32"
elif sys.argv[2] == '64':
    vc_build_bits = "64"
else:
    print "Args: <Doxygen:y/n> <BuildTarget:32/64> <FullRebuild:y/n>"
    print "BuildTarget param must be 32 or 64!"
    exit(1)

if sys.argv[3] == 'y':
    vc_build_type = "/Rebuild"
elif sys.argv[3] == 'n':
    vc_build_type = "/Build"
else:
    print "Args: <Doxygen:y/n> <BuildTarget:32/64> <FullRebuild:y/n>"
    print "Doxygen param must be y or n!"
    exit(1)

VC_version = 9
if len(sys.argv) > 4:
    if sys.argv[4] == '10':
        VC_version = 10

try:
    VS_NEED_UPGRADE = 0
    MSVC_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\VisualStudio\9.0")
    MSVC_VALUES = [("InstallDir", win32con.REG_SZ)]
    VS_INST_DIR = get_reg_values(MSVC_KEY, MSVC_VALUES)[0]
except Exception as e:
    VC_version = 10

print ("\n*** Build target is:" + vc_build_bits + " Doxy:" + str(Make_Doxy) + " Rebuild:" + sys.argv[3] + " VC Version:" + str(VC_version) + " ***");	
	
#------------Constants and globals---------------------------------------------#
if VC_version == 10:
    VS_NEED_UPGRADE = 1
    MSVC_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\VisualStudio\10.0")    
    MSVC_VALUES = [("InstallDir", win32con.REG_SZ)]
    VS_INST_DIR = get_reg_values(MSVC_KEY, MSVC_VALUES)[0]    

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
global samples_guid_list
global samples_guid_list_net
samples_proj_list = []
samples_guid_list = []
samples_guid_list_net = []

if vc_build_bits=="32":
	bin_dir = "Bin"
	lib_dir = "Lib"
	vc_build_platform = "Win32"
else:
	bin_dir = "Bin64"
	lib_dir = "Lib64"
	vc_build_platform = "x64"

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

#--------------Build Project---------------------------------------------------#
print("* Building " + PROJECT_NAME + "...")
logger.info("Building " + PROJECT_NAME + "...")

# Build project solution
os.chdir(WORK_DIR + PROJECT_SLN.rpartition("\\")[0])
if VS_NEED_UPGRADE == 1:
    os.system("attrib -r * /s")
    os.system("\""+VS_INST_DIR + "devenv\" " + PROJECT_SLN.rpartition("\\")[2]+\
                 " /upgrade > ..\\CreateRedist\\Output\\Build"+PROJECT_NAME+".txt")

subprocess.call('\"'+VS_INST_DIR + 'devenv\" ' + PROJECT_SLN.rpartition("\\")[2]+\
             " " + vc_build_type + " \"release|" + vc_build_platform + "\" /out ..\\CreateRedist\\Output\\Build"+PROJECT_NAME+".txt", close_fds=True)

# Get the build output
lines = open(SCRIPT_DIR+"\\Output\\Build"+PROJECT_NAME+".txt").readlines()
build_result = lines[-2]
print(build_result)
logger.info(build_result)
# Check for failed build

temp = re.search("(\d*) failed",build_result)
if temp != None :
	failed_builds = int(temp.group(1))
else:
	failed_builds = 0
if failed_builds != 0:
    print("Building Failed!!")
    logger.critical("Building Failed!")
    finish_script(1)



if Make_Doxy==1:
    #--------------Doxygen---------------------------------------------------------#
    print("* Creating Doxygen...")
    logger.info("Creating Doxygen...")
    os.chdir(WORK_DIR + "Source\\Doxygen");
    # Replacing version number in the doxygen setup file
    res = os.system("attrib -r Doxyfile")
    print 'removing readonly attribute for Doxyfile: ' + str(res)
    regx_replace("OpenNI \d*.\d*.\d*\s",PROJECT_NAME +" " + OPENNI_VER + " ",'Doxyfile')
    if os.path.exists(WORK_DIR + "\\Source\\Doxygen\\html\\"):
        os.system("rmdir /S /Q html")
    # Running doxygen
    os.system("mkdir html")
    #os.system("copy PSSmallLogo.jpg html") // where is this file ? 
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
os.system("mkdir Redist\\" + bin_dir)
os.system("mkdir Redist\\" + lib_dir)
os.system("mkdir Redist\\Include")
os.system("mkdir Redist\\Documentation")
os.system("mkdir Redist\\Samples")
os.system("mkdir Redist\\Samples\\" + bin_dir)
os.system("mkdir Redist\\Samples\\" + bin_dir + "\\Debug")
os.system("mkdir Redist\\Samples\\" + bin_dir + "\\Release")
os.system("mkdir Redist\\Samples\\Build")
os.system("mkdir Redist\\Samples\\Res")
os.system("mkdir Redist\\Data")

#-------------Copy files to redist---------------------------------------------#
print("* Copying files to redist dir...")
logger.info("Copying files to redist dir...")

#license
os.system ("copy \"..\\..\\GPL.txt\" Redist")
os.system ("copy \"..\\..\\LGPL.txt\" Redist")

#bin
os.system ("copy " + bin_dir + "\\Release\\OpenNI*.dll Redist\\" + bin_dir)
os.system ("copy " + bin_dir + "\\Release\\OpenNI.net.dll Redist\\" + bin_dir)
os.system ("copy " + bin_dir + "\\Release\\nimCodecs*.dll Redist\\" + bin_dir)
os.system ("copy " + bin_dir + "\\Release\\nimMockNodes*.dll Redist\\" + bin_dir)
os.system ("copy " + bin_dir + "\\Release\\nimRecorder*.dll Redist\\" + bin_dir)
os.system ("copy " + bin_dir + "\\Release\\niReg*.exe Redist\\" + bin_dir)
os.system ("copy " + bin_dir + "\\Release\\niLicense*.exe Redist\\" + bin_dir)

#lib
os.system ("copy " + lib_dir + "\\Release\\*.* Redist\\" + lib_dir)

#docs
os.system ("copy \"..\\..\\Documentation\\*.pdf\" Redist\\Documentation")
os.system ("copy \"..\\..\\Documentation\\*.chi\" Redist\\Documentation")
os.system ("copy \"..\\..\\Documentation\\*.chm\" Redist\\Documentation")

#include
os.system ("xcopy /S ..\\..\\Include\\* Redist\\Include\\")

#driver
os.system ("xcopy /S Driver\\Bin\\*.* Redist\\Driver\\")

#samples
os.system ("copy Redist\\Samples\\" + bin_dir + "\\Release\\*.* Redist\\Samples\\" + bin_dir + "\\Debug")
os.system ("xcopy /S Build\\Res\\*.* Redist\\Samples\\Res\\")

#data
os.system ("copy ..\\..\\Data\\SamplesConfig.xml Redist\\Data\\SamplesConfig.xml")

# Copy the glut dll to the samples
os.system ("xcopy /S .\\" + bin_dir + "\\Release\\glut*.dll Redist\\Samples\\" + bin_dir + "\\Release\\")
os.system ("xcopy /S .\\" + bin_dir + "\\Release\\glut*.dll Redist\\Samples\\" + bin_dir + "\\Debug\\")

# Copy the OpenNI.net dll to the samples
os.system ("xcopy /S .\\" + bin_dir + "\\Release\\OpenNI.net.dll Redist\\Samples\\" + bin_dir + "\\Release\\")
os.system ("xcopy /S .\\" + bin_dir + "\\Release\\OpenNI.net.dll Redist\\Samples\\" + bin_dir + "\\Debug\\")

# Copy the release notes
os.system ("copy .\\ReleaseNotes.txt Redist\\Documentation\\")

#-------Creating samples-------------------------------------------------------#
print("* Creating samples...")
logger.info("Creating samples...")

# open all solution files
OUTFILESLN2008 = open("Redist\\Samples\\Build\\All_2008.sln",'w')
OUTFILESLN2008.write("Microsoft Visual Studio Solution File, Format Version 10.00\n")
OUTFILESLN2008.write("# Visual Studio 2008\n")
OUTFILESLN2010 = open("Redist\\Samples\\Build\\All_2010.sln",'w')
OUTFILESLN2010.write("Microsoft Visual Studio Solution File, Format Version 11.00\n")
OUTFILESLN2010.write("# Visual Studio 2010\n")

all_samples = find_samples()

# add projects
for sample in all_samples.values():
    print(sample.name)

    # make dir
    os.system ("mkdir " + sample.redist_dir)
    # copy source
    os.system ("xcopy /S " + sample.source_dir + " " + sample.redist_dir)

    # copy the project file to 2008 and 2010:
    prj_name_partitioned = os.path.splitext(sample.project_file);
    prj2008_filename = sample.redist_dir + "\\" + sample.name + "_2008" + prj_name_partitioned[1]
    prj2010_filename = sample.redist_dir + "\\" + sample.name + "_2010" + prj_name_partitioned[1]

    shutil.copy(sample.project_file, prj2008_filename)
    shutil.copy(sample.project_file, prj2010_filename)

    # create reletive path to samples
    prj2008_path = "..\\" + prj2008_filename.partition("\\")[2].partition("\\")[2]
    prj2010_path = "..\\" + prj2010_filename.partition("\\")[2].partition("\\")[2]

    # add project to solution
    OUTFILESLN2008.write("Project(\"{19091980-2008-4CFA-1491-04CC20D8BCF9}\") = \""+\
                                sample.project_name + "\", \"" + prj2008_path + "\", \"" + sample.project_guid + "\"\n")
    OUTFILESLN2010.write("Project(\"{19091980-2008-4CFA-1491-04CC20D8BCF9}\") = \""+\
                                sample.project_name + "\", \"" + prj2010_path + "\", \"" + sample.project_guid + "\"\n")

    # write down dependencies
    if len(sample.dependencies) > 0:
        OUTFILESLN2008.write("\tProjectSection(ProjectDependencies) = postProject\n")
        for depend in sample.dependencies:
            OUTFILESLN2008.write("\t\t" + all_samples[depend].project_guid + " = " + all_samples[depend].project_guid + "\n")
        OUTFILESLN2008.write("\tEndProjectSection\n")
        OUTFILESLN2010.write("\tProjectSection(ProjectDependencies) = postProject\n")
        for depend in sample.dependencies:
            OUTFILESLN2010.write("\t\t" + all_samples[depend].project_guid + " = " + all_samples[depend].project_guid + "\n")
        OUTFILESLN2010.write("\tEndProjectSection\n")		

    OUTFILESLN2008.write("EndProject\n")
    OUTFILESLN2010.write("EndProject\n")

# Close files
OUTFILESLN2008.write("Global\n")
OUTFILESLN2008.write("	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n")
OUTFILESLN2008.write("		Debug|Win32 = Debug|Win32\n")
OUTFILESLN2008.write("		Debug|x64 = Debug|x64\n")
OUTFILESLN2008.write("		Release|Win32 = Release|Win32\n")
OUTFILESLN2008.write("		Release|x64 = Release|x64\n")
OUTFILESLN2008.write("	EndGlobalSection\n")
OUTFILESLN2008.write("	GlobalSection(ProjectConfigurationPlatforms) = postSolution\n")
OUTFILESLN2010.write("Global\n")
OUTFILESLN2010.write("	GlobalSection(SolutionConfigurationPlatforms) = preSolution\n")
OUTFILESLN2010.write("		Debug|Win32 = Debug|Win32\n")
OUTFILESLN2010.write("		Debug|x64 = Debug|x64\n")
OUTFILESLN2010.write("		Release|Win32 = Release|Win32\n")
OUTFILESLN2010.write("		Release|x64 = Release|x64\n")
OUTFILESLN2010.write("	EndGlobalSection\n")
OUTFILESLN2010.write("	GlobalSection(ProjectConfigurationPlatforms) = postSolution\n")

for sample in all_samples.values():
    conf_32_name = "Win32"
    if sample.is_net:
        conf_32_name = "x86"

    OUTFILESLN2008.write("		" + sample.project_guid + ".Debug|Win32.ActiveCfg = Debug|" + conf_32_name + "\n")
    OUTFILESLN2008.write("		" + sample.project_guid + ".Debug|Win32.Build.0 = Debug|" + conf_32_name + "\n")
    OUTFILESLN2008.write("		" + sample.project_guid + ".Debug|x64.ActiveCfg = Debug|x64\n")
    OUTFILESLN2008.write("		" + sample.project_guid + ".Debug|x64.Build.0 = Debug|x64\n")
    OUTFILESLN2008.write("		" + sample.project_guid + ".Release|Win32.ActiveCfg = Release|" + conf_32_name + "\n")
    OUTFILESLN2008.write("		" + sample.project_guid + ".Release|Win32.Build.0 = Release|" + conf_32_name + "\n")
    OUTFILESLN2008.write("		" + sample.project_guid + ".Release|x64.ActiveCfg = Release|x64\n")
    OUTFILESLN2008.write("		" + sample.project_guid + ".Release|x64.Build.0 = Release|x64\n")
    OUTFILESLN2010.write("		" + sample.project_guid + ".Debug|Win32.ActiveCfg = Debug|" + conf_32_name + "\n")
    OUTFILESLN2010.write("		" + sample.project_guid + ".Debug|Win32.Build.0 = Debug|" + conf_32_name + "\n")
    OUTFILESLN2010.write("		" + sample.project_guid + ".Debug|x64.ActiveCfg = Debug|x64\n")
    OUTFILESLN2010.write("		" + sample.project_guid + ".Debug|x64.Build.0 = Debug|x64\n")
    OUTFILESLN2010.write("		" + sample.project_guid + ".Release|Win32.ActiveCfg = Release|" + conf_32_name + "\n")
    OUTFILESLN2010.write("		" + sample.project_guid + ".Release|Win32.Build.0 = Release|" + conf_32_name + "\n")
    OUTFILESLN2010.write("		" + sample.project_guid + ".Release|x64.ActiveCfg = Release|x64\n")
    OUTFILESLN2010.write("		" + sample.project_guid + ".Release|x64.Build.0 = Release|x64\n")

OUTFILESLN2008.write("	EndGlobalSection\n")
OUTFILESLN2008.write("	GlobalSection(SolutionProperties) = preSolution\n")
OUTFILESLN2008.write("		HideSolutionNode = FALSE\n")
OUTFILESLN2008.write("	EndGlobalSection\n")
OUTFILESLN2008.write("EndGlobal	\n")
OUTFILESLN2010.write("	EndGlobalSection\n")
OUTFILESLN2010.write("	GlobalSection(SolutionProperties) = preSolution\n")
OUTFILESLN2010.write("		HideSolutionNode = FALSE\n")
OUTFILESLN2010.write("	EndGlobalSection\n")
OUTFILESLN2010.write("EndGlobal	\n")

OUTFILESLN2008.close()
OUTFILESLN2010.close()

#-----Remove Read Only Attrib--------------------------------------------------#
print("* Removing Read Only Attributes...")
full_path = os.path.join(os.getcwd(), "Redist")
logger.info("Removing Read Only Attributes... (%s)" % (full_path))
#os.system ("attrib -r -h -s /S Redist\\*.*")

def remove_readonly(path):
	for root, dirs, files in os.walk(path):
		for fname in files: 
			full_path = os.path.join(root, fname)
			os.chmod(full_path ,stat.S_IWRITE)
			
remove_readonly(full_path)



#--------Fixing Files----------------------------------------------------------#
print("* Fixing Files...")
logger.info("Fixing Files...")
for dirpath, dirnames, filenames in os.walk('Redist\\'):
    fix_file('', dirpath, dirnames + filenames)

#-------------Build Samples---------------------------------------------------#
print("* Building Samples in release configuration......")
logger.info("Building Samples in release configuration...")
# Build project solution
os.chdir(WORK_DIR + SAMPLES_SLN.rpartition("\\")[0])

if VS_NEED_UPGRADE == 1:
    os.system("\""+VS_INST_DIR + "devenv\" " + SAMPLES_SLN.rpartition("\\")[2]+\
                           " /upgrade > ..\\..\\..\\CreateRedist\\Output\\EngineSmpRelease.txt")

subprocess.call("\""+VS_INST_DIR + "devenv\" " + SAMPLES_SLN.rpartition("\\")[2]+\
                 " " + vc_build_type + " \"release|" + vc_build_platform + "\" /out ..\\..\\..\\CreateRedist\\Output\\EngineSmpRelease.txt")

# Get the build output
lines = open(SCRIPT_DIR+"\\Output\\EngineSmpRelease.txt").readlines()
build_result = lines[-2]
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

subprocess.call("\""+VS_INST_DIR + "devenv\" " + SAMPLES_SLN.rpartition("\\")[2]+\
                 " " + vc_build_type + " \"debug|" + vc_build_platform + "\" /out ..\\..\\..\\CreateRedist\\Output\\EngineSmpDebug.txt")

# Get the build output
lines = open(SCRIPT_DIR+"\\Output\\EngineSmpDebug.txt").readlines()
build_result = lines[-2]
print(build_result)
logger.info(build_result)
# Check for failed build
failed_builds = int(re.search("(\d*) failed",build_result).group(1))
if failed_builds != 0:
    print("Samples Building In Debug Failed!!")
    logger.critical("Samples Building Failed!")
#    finish_script(1)

# --------------------Delete stuff
os.chdir(WORK_DIR + "\\Platform\\Win32\\Redist\\Samples\\" + bin_dir +"\\Release\\")
os.system("del *.pdb")
os.chdir(WORK_DIR + "\\Platform\\Win32\\Redist\\Samples\\" + bin_dir +"\\Debug\\")
os.system("del *.pdb")
os.system("del *.ilk")
os.chdir(WORK_DIR + "\\Platform\\Win32\\Redist\\" + lib_dir + "\\")
os.system("del nim*.*")

#-------------Make installer---------------------------------------------------#
print("* Making Installer...")
logger.info("Making Installer...")
os.chdir(WORK_DIR + "Platform\\Win32\\Install\\OpenNI\\")
# Replace version in the WIX
os.system("attrib -r Includes\\OpenNIVariables.wxi")

print("setting WIX BuildPlatform")
#regx_replace("BuildPlatform=(.*)", "BuildPlatform=" + str(vc_build_bits) + "?>", "Includes\\OpenNIVariables.wxi")

print("setting WIX BinaryOnlyRedist=True")
regx_replace("BinaryOnlyRedist=(.*)", "BinaryOnlyRedist=True?>", "Includes\\OpenNIVariables.wxi")

# make installer
wixPath = os.environ.get('WIX')
if wixPath == None:
    print '*** no WIX env. var. defined ! use set WIX=C:\Program Files\Windows Installer XML v3.5\ or similar to set the path ***'
    print 'make installer is SERIOUSLY expected to fail'
else:
    print 'WIX='+wixPath

if VS_NEED_UPGRADE == 1:
	subprocess.call("\"" + VS_INST_DIR + "devenv\" OpenNI.sln /upgrade /out ..\\..\\CreateRedist\\Output\\UpgradeOpenNIWIX.txt", close_fds=True)
	
print("calling WIX")
if vc_build_bits=="32":
	subprocess.call("\"" + VS_INST_DIR + "devenv\" OpenNI.wixproj /Build \"release|x86"\
                + "\" /out ..\\..\\CreateRedist\\Output\\BuildOpenNIWIXRedist.txt", close_fds=True)
else:
	subprocess.call("\"" + VS_INST_DIR + "devenv\" OpenNI.wixproj /Build \"release|x64"\
                + "\" /out ..\\..\\CreateRedist\\Output\\BuildOpenNIWIXRedist.txt", close_fds=True)

print("moving Redist Msi")
os.system("move .\\bin\Release\en-US\\OpenNI.msi ..\\..\\CreateRedist\\Output\\OpenNI-Win" + vc_build_bits + "-" + OPENNI_VER + "-Redist.msi")

print("setting WIX BinaryOnlyRedist=False")
regx_replace("BinaryOnlyRedist=(.*)", "BinaryOnlyRedist=False?>", "Includes\\OpenNIVariables.wxi")

# make installer
print("calling WIX")
if vc_build_bits=="32":
	subprocess.call("\"" + VS_INST_DIR + "devenv\" OpenNI.wixproj /Build \"release|x86"\
                + "\" /out ..\\..\\CreateRedist\\Output\\BuildOpenNIWIXDev.txt", close_fds=True)
else:
	subprocess.call("\"" + VS_INST_DIR + "devenv\" OpenNI.wixproj /Build \"release|x64"\
                + "\" /out ..\\..\\CreateRedist\\Output\\BuildOpenNIWIXDev.txt", close_fds=True)
print("moving Dev Msi")	

os.system("move .\\bin\Release\en-US\\OpenNI.msi ..\\..\\CreateRedist\\Output\\OpenNI-Win" + vc_build_bits + "-" + OPENNI_VER + "-Dev.msi")

#-------------CleanUP----------------------------------------------------------#
print("* Redist OpenNi Ended.   !!")
logger.info("Redist OpenNi Ended.")
finish_script(0)


