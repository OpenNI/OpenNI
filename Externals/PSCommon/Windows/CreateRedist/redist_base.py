#/****************************************************************************
#*      THIS file should be DUPLICATED IN BOTH                               *
#*      Engine\Platform\Win32\CreateRedist                                   *
#*  AND OpenNI\Platform\Win32\CreateRedist                                   *
#*                                                                           *
#****************************************************************************/

#/****************************************************************************
#*                                                                           *
#*  PrimeSense OpenNI & Sensor                                               *
#*  Copyright (C) 2010 PrimeSense Ltd.                                       *
#*                                                                           *
#*  This file is part of PrimeSense Common.                                  *
#*                                                                           *
#*  PrimeSense OpenNI & Sensor are free software: you can redistribute       *
#*  it and/or modify                                                         *
#*  it under the terms of the GNU Lesser General Public License as published *
#*  by the Free Software Foundation, either version 3 of the License, or     *
#*  (at your option) any later version.                                      *
#*                                                                           *
#*  PrimeSense OpenNI & Sensor are distributed in the hope that they will    *
#*  be useful,                                                               *
#*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
#*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
#*  GNU Lesser General Public License for more details.                      *
#*                                                                           *
#*  You should have received a copy of the GNU Lesser General Public License *
#*  along with PrimeSense OpenNI & Sensor.                                   *
#*  If not, see <http://www.gnu.org/licenses/>.                              *
#*                                                                           *
#****************************************************************************/

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
import threading

#-------------Functions--------------------------------------------------------#

def is_64_bit_platform():
    result = False
    import platform
    (bits,linkage) = platform.architecture()
    matchObject = re.search('64',bits)
    result = matchObject is not None
    return result

def write_dependencides(sln_file, all_samples,sample):
    sln_file.write("\tProjectSection(ProjectDependencies) = postProject\n")
    for depend in sample.dependencies:
        sln_file.write("\t\t" + all_samples[depend].project_guid + " = " + all_samples[depend].project_guid + "\n")
    sln_file.write("\tEndProjectSection\n")

def remove_readonly(path):
    for root, dirs, files in os.walk(path):
        for fname in files:
            full_path = os.path.join(root, fname)
            os.chmod(full_path ,stat.S_IWRITE)

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

#-------------Classes----------------------------------------------------------#
class SampleData(object):
    def __init__(self):
        self.name = ''
        self.project_dir = ''
        self.source_dir = ''
        self.project_guid = ''
        self.project_file = ''
        self.is_net = False
        self.is_other = False
        self.project_name = ''
        self.redist_dir = ''
        self.dependencies = []

    def print_():
        print((self.name))

    def __str__(self):
        return self.name

    def __eval__(self):
        return self.name

class RedistBase(object):
    def __init__(self):
        self.SCRIPT_DIR = ''
        self.VC_version = 0
        self.vc_build_bits = ""
        self.output_dir = ""
        self.final_dir = ""
        self.config_xml_filename = ""
        self.redist_name = ""
        self.redist_internal_name = ''
        self.product_name = ''
        self.doxy_file_name = ""
        self.write_2010_sample_dependency = False
        self.all_samples = None
        self.internal_conf_name = ''
        self.TIMEOUT_UPGRADE_VS10_SEC = 180
        self.project_is_2010 = False

    def finish_script(self,exit_code):
        os.chdir(self.SCRIPT_DIR)
        #logging.shutdown()
        exit(exit_code)

    def check_args(self,args):
        """
        Parse cmdline args; along them are doxy,bitness,VC.
        """
        if len(sys.argv) not in [4,5]:
            print ("Args: <Doxygen:y/n> <BuildTarget:32/64> <FullRebuild:y/n> [<VCVersion:9/10>]")
            exit(1)
        if sys.argv[1] == 'y' or sys.argv[1] == 'Yes':
            self.Make_Doxy=1
        elif sys.argv[1] == 'n' or sys.argv[1] == 'No':
            self.Make_Doxy=0
        else:
            print("Args: <Doxygen:y/n> <BuildTarget:32/64> <FullRebuild:y/n>")
            print("Doxygen param must be y or n!")
            exit(1)

        if sys.argv[2] == '32':
            self.vc_build_bits = "32"
        elif sys.argv[2] == '64':
            self.vc_build_bits = "64"
        else:
            print("Args: <Doxygen:y/n> <BuildTarget:32/64> <FullRebuild:y/n>")
            print("BuildTarget param must be 32 or 64!")
            exit(1)

        if sys.argv[3] == 'y' or sys.argv[3] == 'Yes':
            self.vc_build_type = "/Rebuild"
        elif sys.argv[3] == 'n' or sys.argv[3] == 'No':
            self.vc_build_type = "/Build"
        else:
            print("Args: <Doxygen:y/n> <BuildTarget:32/64> <FullRebuild:y/n>")
            print("FullRebuild param must be y or n!")
            exit(1)

        if self.project_is_2010:
            self.VC_version = 10
            if len(sys.argv) > 4:
                if sys.argv[4] == '9':
                    print("Project does not support VS2008!")
                    exit(1)
        else:
            self.VC_version = 9
            if len(sys.argv) > 4:
                if sys.argv[4] == '10':
                    self.VC_version = 10

    def init_vs_vars(self):
        """
        Checks for the availablity of Visual Studio to compile with.
        Currently supports VS2008 (vc9) and VS2010(vc10).
        If 64-bit platform, registry key under Software/Wow6432Node/...
        """
        self.VS_NEED_UPGRADE = 0
        try:
            if self.is_64_bit_platform:
                MSVC_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Wow6432Node\Microsoft\VisualStudio\9.0")
            else:
                MSVC_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\VisualStudio\9.0")
            MSVC_VALUES = [("InstallDir", win32con.REG_SZ)]
            self.VS_INST_DIR = get_reg_values(MSVC_KEY, MSVC_VALUES)[0]
        except Exception as e:
            self.VC_version = 10
            
        if self.VC_version == 10:
            if not self.project_is_2010:
                self.VS_NEED_UPGRADE = 1
                
            if self.is_64_bit_platform:
                MSVC_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Wow6432Node\Microsoft\VisualStudio\10.0")
            else:
                MSVC_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\VisualStudio\10.0")
            MSVC_VALUES = [("InstallDir", win32con.REG_SZ)]
            self.VS_INST_DIR = get_reg_values(MSVC_KEY, MSVC_VALUES)[0]

    def init_vars(self):
        """
        Initializae misc variables.
        Among them: is_64_bit_platform
        """
        self.inst_proj_path = 'Platform\\Win32\\Install\\%s\\'%self.redist_internal_name
        self.is_64_bit_platform = is_64_bit_platform()
        self.DateTimeSTR = strftime("%Y-%m-%d %H:%M:%S")
        DateSTR = strftime("%Y-%m-%d")
        CONFIG_XML = parse(self.config_xml_filename)
        self.WORK_DIR = str(CONFIG_XML.getElementsByTagName("WORK_DIR")[0].firstChild.data)
        # Fix to allow reletive path
        os.chdir(self.WORK_DIR)
        self.WORK_DIR = os.getcwd() + "\\"
        os.chdir(self.SCRIPT_DIR)
        self.VER = str(CONFIG_XML.getElementsByTagName("VERSION_NUMBER")[0].firstChild.data)
        PLATFORM = str(CONFIG_XML.getElementsByTagName("PLATFORM")[0].firstChild.data)
        self.PROJECT_SLN = str(CONFIG_XML.getElementsByTagName("PROJECT_SLN")[0].firstChild.data)
        self.SAMPLES_SLN = str(CONFIG_XML.getElementsByTagName("SAMPLES_SLN")[0].firstChild.data)
        self.PROJECT_NAME = str(CONFIG_XML.getElementsByTagName("PROJECT")[0].firstChild.data)
        self.BUILD_DIR = os.path.join(self.WORK_DIR, "Platform", "Win32", "Build")

        ver_regx = re.compile("SDK \d.*\s")
        global samples_proj_list
        global samples_guid_list
        global samples_guid_list_net
        samples_proj_list = []
        samples_guid_list = []
        samples_guid_list_net = []

        if self.vc_build_bits=="32":
            self.bin_dir = "Bin"
            self.lib_dir = "Lib"
            self.vc_build_platform = "Win32"
            self.output_dir = "Output32"
            self.final_dir = "Final32"
        else:
            self.bin_dir = "Bin64"
            self.lib_dir = "Lib64"
            self.vc_build_platform = "x64"
            self.output_dir = "Output64"
            self.final_dir = "Final64"

    def init_logger(self,name):
        """
        Initilizes the logger.
        """
        #-------------Log--------------------------------------------------------------#
        out_path = os.path.join(self.SCRIPT_DIR,self.output_dir)
        if not(os.path.exists(out_path)):
            os.mkdir(out_path)
        self.logger = logging.getLogger(name)
        hdlr = logging.FileHandler(os.path.join(self.output_dir, '%s_redist_maker.log'%name))
        formatter = logging.Formatter('%(asctime)s %(levelname)s %(message)s')
        hdlr.setFormatter(formatter)
        self.logger.addHandler(hdlr)
        self.logger.setLevel(logging.INFO)

    def print_message(self):
        """
        TO DO: fix that function in Redist_OpenNi to resemble this (better) version.
        """
        #------------Welcome Messege--------=------------------------------------------#
        print("\n");
        print("*********************************")
        print(("*   PrimeSense " + self.redist_name +  " Redist    *"))
        print(("*     " + self.DateTimeSTR + "       *"))
        print("*********************************")

    def build_proj_solution(self):
        #--------------Build Project---------------------------------------------------#
        print(("* Building " + self.PROJECT_NAME + "..."))
        path2output = os.path.join(self.SCRIPT_DIR,self.output_dir)
        if not os.path.exists(path2output):
            os.makedirs(path2output)
        path2final = os.path.join(self.SCRIPT_DIR,self.final_dir)
        if not os.path.exists(path2final):
            os.makedirs(path2final)

        self.logger.info(("Building " + self.PROJECT_NAME + "..."))
        # Set Intel Env
        os.system("set INTEL_LICENSE_FILE=C:\\Program Files\\Common Files\\Intel\\Licenses")
        # Build project solution
        os.chdir(self.WORK_DIR + self.PROJECT_SLN.rpartition("\\")[0])
        print((os.getcwd()))

        out_file = os.path.join(self.SCRIPT_DIR, self.output_dir, "Build"+self.PROJECT_NAME+".txt")
        upg_out_file = os.path.join(self.SCRIPT_DIR, self.output_dir, "Build"+self.PROJECT_NAME+"UPG"+".txt")
        if self.VS_NEED_UPGRADE == 1:
            os.system("attrib -r * /s")
            devenv_upg_cmd_str = "\""+self.VS_INST_DIR + "devenv\" " + self.PROJECT_SLN.rpartition("\\")[2]+\
                         " /upgrade > " + upg_out_file
            print(('upgrading. out put in %s'%out_file))
            print(('command is %s'%devenv_upg_cmd_str))
            my_rc = os.system(devenv_upg_cmd_str)
            print(('upgrading finished w result %d'%my_rc))
            #devenv_upg_cmd = Command(devenv_upg_cmd_str)
            #devenv_upg_cmd.run(self.TIMEOUT_UPGRADE_VS10_SEC)

        devenv_cmd = '\"'+self.VS_INST_DIR + 'devenv\" ' + self.PROJECT_SLN.rpartition("\\")[2]+\
                     " " + self.vc_build_type + " \"release|" + self.vc_build_platform + "\" /out " + \
                     out_file
        self.logger.debug('Calling vs : %s'%devenv_cmd)
        rc = subprocess.call(devenv_cmd, close_fds=True)
        self.logger.debug('Calling vs, RC: %d'%rc)
        # Get the build output
        lines = open(out_file).readlines()
        build_result = lines[-2]
        print(build_result)
        self.logger.info(build_result)
        # Check for failed build
        failed_builds = 0
        fail_to_open = 0

        temp = re.search("(\d*) failed",build_result)
        if temp != None :
            failed_builds = int(temp.group(1))

        temp2 = re.search('cannot be opened',str(lines))
        if temp2 != None :
            fail_to_open = 1

        if failed_builds != 0 or fail_to_open !=0:
            print("Building Failed!!")
            self.logger.critical("Building Failed!")
            self.finish_script(1)

        # return to work dir
        os.chdir(self.WORK_DIR)

    def make_doxy(self):
        """
        Make Doxy.
        """
        if self.Make_Doxy==1:
            print("* Creating Doxygen...")
            self.logger.info("Creating Doxygen...")
            os.chdir(os.path.join(self.WORK_DIR,"Source\\Doxygen"));
            # Replacing version number in the doxygen setup file
            res = os.system("attrib -r " + self.doxy_file_name)
            print(('removing readonly attribute for Doxyfile: ' + str(res)))
            regx_replace(self.redist_name + " \d*.\d*.\d*\s",self.PROJECT_NAME +" " + self.VER + " ",self.doxy_file_name)
            if os.path.exists(self.WORK_DIR + "\\Source\\Doxygen\\html\\"):
                os.system("rmdir /S /Q html")
            # Running doxygen
            os.system("mkdir html > null")
            #os.system("copy PSSmallLogo.jpg html > null") // where is this file ?
            doxy_out_file = os.path.join(self.SCRIPT_DIR,self.output_dir,self.PROJECT_NAME + "Doxy.txt")
            doxygen_cmd = "doxygen.exe %s > " % (self.doxy_file_name) + doxy_out_file
            os.system(doxygen_cmd)
            self.copy_doxy_files()
            os.chdir(self.WORK_DIR)
        else:
            print("Skipping Doxygen...")

    def create_redist_dir(self):
        """
        Creates directory structure for Redist.
        """
        #-------------Create Redist Dir------------------------------------------------#
        print("* Creating Redist Dir...")
        self.logger.info("Creating Redist Dir...")
        os.chdir(os.path.join(self.WORK_DIR,"Platform","Win32"))
        # Removing the old directory
        os.system("rmdir /S /Q Redist")
        # Creating new directory tree
        os.system("mkdir Redist")
        os.system("mkdir Redist\\" + self.bin_dir)
        os.system("mkdir Redist\\" + self.lib_dir)
        os.system("mkdir Redist\\Include")
        os.system("mkdir Redist\\Documentation")
        os.system("mkdir Redist\\Samples")
        os.system("mkdir Redist\\Samples\\" + self.bin_dir)
        os.system("mkdir Redist\\Samples\\" + self.bin_dir + "\\Debug")
        os.system("mkdir Redist\\Samples\\" + self.bin_dir + "\\Release")
        os.system("mkdir Redist\\Samples\\Build")
        os.system("mkdir Redist\\Samples\\Res")
        os.system("mkdir Redist\\Data")
        os.chdir(self.WORK_DIR)

    def find_samples(self):
        # returns a dictionary of all samples
        all_samples = dict()
        os.chdir(self.WORK_DIR)
        samples_list = os.listdir(os.path.join(self.BUILD_DIR,"Samples"))
        if '.svn' in samples_list:
            samples_list.remove('.svn')
        for sample in samples_list:
            sample_data = SampleData()
            sample_data.name = sample
            sample_data.source_dir = os.path.join(self.WORK_DIR, "Samples", sample)
            sample_data.project_dir = os.path.join(self.BUILD_DIR, "Samples", sample)
            sample_data.is_other = False;

            vc_proj_name = sample_data.project_dir + "\\" + sample + ".vcproj"
            cs_proj_name = sample_data.project_dir + "\\" + sample + ".csproj"
            vcx_proj_name = sample_data.project_dir + "\\" + sample + ".vcxproj"
            other_proj_name = os.path.join(sample_data.project_dir, "Build.bat")

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

                prj.close()

            elif os.path.exists(vcx_proj_name):
                # a VC project for VS 2010
                sample_data.project_file = vcx_proj_name
                sample_data.project_name = sample

                # open it
                prj = open(vcx_proj_name, 'r')
                lines = prj.readlines()
                for line in lines:
                    # Search for name
                    if sample_data.project_name == "":
                        ProjNametmp = re.search(r"Name=\"(.*)\"",line)
                        if (ProjNametmp != None):
                            sample_data.project_name = ProjNametmp.group(1)

                    # Search for GUID
                    if sample_data.project_guid == "":
                        ProjGUIDtmp = re.search(r"<ProjectGuid>(.*)</ProjectGuid>", line)
                        if (ProjGUIDtmp != None):
                            sample_data.project_guid = ProjGUIDtmp.group(1)

                prj.close()
            
            elif os.path.exists(cs_proj_name):
                # a .NET project
                sample_data.project_file = cs_proj_name
                sample_data.is_net = True

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

            elif os.path.exists(other_proj_name):
                # some other type of project (java?)
                sample_data.project_file = other_proj_name
                sample_data.is_other = True
                sample_data.project_name = sample

            else:
                print(('Sample ' + sample + ' does not have a valid project file'))
                self.finish_script(1)

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

    def get_samples(self):
        # returns a dictionary of all samples
        if self.all_samples == None:
            self.all_samples = self.find_samples()

        return self.all_samples

    def build_other_proj(self, build_dir):
        # build other (not Visual Studio) project
        ret = subprocess.call(os.path.join(build_dir, "Build.bat") + " " + self.vc_build_bits)
        if ret != 0:
            print(("Building project " + build_dir + " failed!"))
            self.logger.critical("Building project " + build_dir + " failed!")
            self.finish_script(1)

    def build_other_samples(self):
        "Builds other samples (java?)"
        all_samples = self.get_samples()
        for sample in list(all_samples.values()):
            if not sample.is_other:
                continue
            build_dir = os.path.join(self.BUILD_DIR, "Samples", sample.name)
            self.build_other_proj(build_dir)

    def creating_samples(self):
        """
        not exactly the same some further work needed.
        """
        #-------Creating samples-------------------------------------------------------#
        print("* Creating samples...")
        self.logger.info("Creating samples...")

        all_samples = self.get_samples()

        os.chdir(os.path.join(self.WORK_DIR, "Platform", "Win32"))

        # open all solution files
        if not self.project_is_2010:
            OUTFILESLN2008 = open("Redist\\Samples\\Build\\All_2008.sln",'w')
            OUTFILESLN2008.write("Microsoft Visual Studio Solution File, Format Version 10.00\n")
            OUTFILESLN2008.write("# Visual Studio 2008\n")

        if not self.project_is_2010:
            OUTFILESLN2010 = open("Redist\\Samples\\Build\\All_2010.sln",'w')
        else:
            OUTFILESLN2010 = open("Redist\\Samples\\Build\\All.sln",'w')
        
        OUTFILESLN2010.write("Microsoft Visual Studio Solution File, Format Version 11.00\n")
        OUTFILESLN2010.write("# Visual Studio 2010\n")

        # copy java build script
        try:
            shutil.copy(os.path.join("Build", "BuildJava.py"), os.path.join("Redist", "Samples", "Build"))
        except:
            pass

        # add projects
        for sample in list(all_samples.values()):
            # make dir
            os.system ("mkdir " + sample.redist_dir)
            # copy source
            os.system ("xcopy /S " + sample.source_dir + " " + sample.redist_dir)

            if sample.is_other:
                shutil.copy(sample.project_file, sample.redist_dir)
            else:
                # copy the project file to 2008 and 2010:
                prj_name_partitioned = os.path.splitext(sample.project_file);
                prj2008_filename = sample.redist_dir + "\\" + sample.name + "_2008" + prj_name_partitioned[1]

                if self.project_is_2010:
                    prj2010_filename = sample.redist_dir + "\\" + sample.name + prj_name_partitioned[1]
                else:
                    prj2010_filename = sample.redist_dir + "\\" + sample.name + "_2010" + prj_name_partitioned[1]

                if not self.project_is_2010:
                    shutil.copy(sample.project_file, prj2008_filename)
                shutil.copy(sample.project_file, prj2010_filename)

                # create reletive path to samples
                prj2008_path = "..\\" + prj2008_filename.partition("\\")[2].partition("\\")[2]
                prj2010_path = "..\\" + prj2010_filename.partition("\\")[2].partition("\\")[2]

                # add project to solution
                if not self.project_is_2010:
                    OUTFILESLN2008.write("Project(\"{19091980-2008-4CFA-1491-04CC20D8BCF9}\") = \""+\
                                            sample.project_name + "\", \"" + prj2008_path + "\", \"" + sample.project_guid + "\"\n")
                OUTFILESLN2010.write("Project(\"{19091980-2008-4CFA-1491-04CC20D8BCF9}\") = \""+\
                                            sample.project_name + "\", \"" + prj2010_path + "\", \"" + sample.project_guid + "\"\n")

                # write down dependencies
                if len(sample.dependencies) > 0:
                    if not self.project_is_2010:
                        OUTFILESLN2008.write("\tProjectSection(ProjectDependencies) = postProject\n")
                        for depend in sample.dependencies:
                            OUTFILESLN2008.write("\t\t" + all_samples[depend].project_guid + " = " + all_samples[depend].project_guid + "\n")
                        OUTFILESLN2008.write("\tEndProjectSection\n")
                        if self.write_2010_sample_dependency == True:
                            write_dependencides(OUTFILESLN2010,all_samples,sample)
                            
                if not self.project_is_2010:
                    OUTFILESLN2008.write("EndProject\n")
                OUTFILESLN2010.write("EndProject\n")

        # Close files
        if not self.project_is_2010:
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

        for sample in list(all_samples.values()):
            conf_32_name = "Win32"
            if sample.is_net:
                conf_32_name = "x86"

            if not self.project_is_2010:
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

        if not self.project_is_2010:
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

        if not self.project_is_2010:
            OUTFILESLN2008.close()
        OUTFILESLN2010.close()

        os.chdir(self.WORK_DIR)

    def remove_read_only_attributes(self):
        #-----Remove Read Only Attrib--------------------------------------------------#
        print("* Removing Read Only Attributes...")
        full_path = os.path.join(self.WORK_DIR,"Platform", "Win32" , "Redist")
        self.logger.info("Removing Read Only Attributes... (%s)" % (full_path))
        #os.system ("attrib -r -h -s /S Redist\\*.*")
        remove_readonly(full_path)

    def make_installer(self,msi_dest_path):
        """
        [dev_success,redist_success]
        """
        wix_var_file = '%sVariables.wxi'%self.redist_internal_name.replace('_','')

        [dev_success,redist_success] = [False,False]
        print("* Making Installer...")
        self.logger.info("Making Installer...")
        self.wix_inst_primitive_check()
        os.chdir(os.path.join(self.WORK_DIR,self.inst_proj_path))
        # Replace version in the WIX
        self.wix_dev_var_set()
        self.check_upgrade_install_sln()
        print("calling WIX")
        conf_name = self.internal_conf_name
        dev_success = self.build_installer(conf_name)
        print(("moving %s Msi"%conf_name))
        src = ('.\\bin\Release\en-US\\%s.msi'%self.redist_internal_name)
        dst = os.path.join( msi_dest_path ,self.final_dir,(self.product_name + '-Win' + self.vc_build_bits + "-" + self.VER + '-%s.msi'%conf_name))
        os.system(("move %s %s"%(src,dst)))
        self.dev_to_redist_hack()
        self.wix_redist_var_set()
        print("calling WIX")
        conf_name = 'Redist'
        redist_success = self.build_installer(conf_name)
        print(("moving %s Msi"%conf_name))
        os.system("move .\\bin\Release\en-US\\%s.msi %s"%(self.redist_internal_name,\
                                                          os.path.join( msi_dest_path ,self.final_dir,self.product_name + '-Win' + self.vc_build_bits + "-" + self.VER + '-%s.msi'%conf_name)))

        os.chdir(self.WORK_DIR)
        return [dev_success,redist_success]

    def build_installer(self,conf_name):
        success = False
        wix_log = 'Build%sWIX%s'%(self.redist_internal_name,conf_name)
        wix_log.replace('_','') #because outside code expects EENI in the name instead of EE_NI
        out_file = os.path.join('..\\..\\CreateRedist',self.output_dir,"%s.txt"%wix_log)
        wix_rc = subprocess.call("\"" + self.VS_INST_DIR \
            + "devenv\" %s.wixproj /Build \"release|%s"%(self.redist_internal_name, 'x86' if self.vc_build_bits=='32' else 'x64')  \
            + "\" /out " + out_file, close_fds=True)
        failed_builds = self.check_vs_report_failed(out_file)
        if failed_builds > 0 or wix_rc != 0:
            self.logger.info('Fail to build installer for %s version'%conf_name)
        else:
            success = True
        return success

    def wix_redist_var_set(self):
        """preconsdition: CWD is where wix-variables-file is stored"""
        temp = self.redist_internal_name.replace('_','')
        os.system("attrib -r Includes\\%sVariables.wxi"%temp)
        #print("setting WIX BuildPlatform")
        #regx_replace("BuildPlatform=(.*)", "BuildPlatform=" + str(vc_build_bits) + "?>", "Includes\\OpenNIVariables.wxi")
        print("setting WIX BinaryOnlyRedist=True")
        regx_replace("BinaryOnlyRedist=(.*)", "BinaryOnlyRedist=True?>", "Includes\\%sVariables.wxi"%temp)

    def wix_dev_var_set(self):
        """preconsdition: CWD is where wix-variables-file is stored"""
        print("setting WIX BinaryOnlyRedist=False")
        temp = self.redist_internal_name.replace('_','')
        os.system("attrib -r Includes\\%sVariables.wxi"%temp)
        regx_replace("BinaryOnlyRedist=(.*)", "BinaryOnlyRedist=False?>", "Includes\\%sVariables.wxi"%temp)

    def check_upgrade_install_sln(self):
        """preconsdition: CWD is where wix-variables-file is stored"""
        up_wix_file = os.path.join('..\\..\\CreateRedist',self.output_dir,"Upgrade%sWIX.txt"%(self.redist_internal_name))
        if self.VS_NEED_UPGRADE == 1:
            subprocess.call("\"" + self.VS_INST_DIR +  \
            "devenv\" %s.sln /upgrade /out "%(self.redist_internal_name) + up_wix_file, close_fds=True)

    def wix_inst_primitive_check(self):
        wixPath = os.environ.get('WIX')
        if wixPath == None:
            print('*** no WIX env. var. defined ! use set WIX=C:\Program Files\Windows Installer XML v3.5\ or similar to set the path ***')
            print('make installer is SERIOUSLY expected to fail')
            self.logger.info('It seems that WIX is not installed and therefore teh installer cannot be built.')
        else:
            print(('WIX='+wixPath))

    def dev_to_redist_hack(self):
        pass

    def check_vs_report_failed(self,file):
        lines = open(file).readlines()
        build_result = lines[-2]
        failed_builds = 0
        temp = re.search("(\d*) failed",build_result)
        if temp != None :
            failed_builds = int(temp.group(1))
        return failed_builds

    def fixing_files(self):
        """
        fixing files
        """
        #--------Fixing Files----------------------------------------------------------#
        print("* Fixing Files...")
        self.logger.info("Fixing Files...")
        for dirpath, dirnames, filenames in os.walk(os.path.join(self.WORK_DIR, "Platform", "Win32", "Redist")):
            self.fix_file('', dirpath, dirnames + filenames)

    def build_samples(self):
        """
        Build Samples.
        """
        #-------------Build Samples---------------------------------------------------#
        print("* Building Samples in release configuration......")
        self.logger.info("Building Samples in release configuration...")
        # Build project solution
        os.chdir(self.WORK_DIR + self.SAMPLES_SLN.rpartition("\\")[0])

        output_file = os.path.join(self.SCRIPT_DIR ,self.output_dir ,self.PROJECT_NAME + "SmpRelease.txt")

        if self.VS_NEED_UPGRADE == 1:
            os.system("\""+self.VS_INST_DIR + "devenv\" " +self.SAMPLES_SLN.rpartition("\\")[2]+\
                       " /upgrade > " + output_file)

        subprocess.call("\""+self.VS_INST_DIR + "devenv\" " +self.SAMPLES_SLN.rpartition("\\")[2]+\
                 " " + self.vc_build_type + " \"release|" + self.vc_build_platform + "\" /out " + output_file)

        # Get the build output
        lines = open(output_file).readlines()
        build_result = lines[-2]
        print(build_result)
        self.logger.info(build_result)
        # Check for failed build

        failed_builds = 0
        temp = re.search("(\d*) failed",build_result)

        if temp != None :
            failed_builds = int(temp.group(1))
        if failed_builds != 0:
            print("Samples Building In Release Failed!!")
            self.logger.critical("Samples Building Failed!")
            self.finish_script(1)

        print("* Building Samples in debug configuration......")
        self.logger.info("Building Samples in debug configuration...")
        # Build project solution
        os.chdir(self.WORK_DIR +self.SAMPLES_SLN.rpartition("\\")[0])

        output_file = os.path.join(self.SCRIPT_DIR ,self.output_dir ,self.PROJECT_NAME + "SmpDebug.txt")

        if self.VS_NEED_UPGRADE == 1:
            os.system("\""+self.VS_INST_DIR + "devenv\" " +self.SAMPLES_SLN.rpartition("\\")[2]+\
                     " /upgrade > " + output_file)

        subprocess.call("\""+self.VS_INST_DIR + "devenv\" " +self.SAMPLES_SLN.rpartition("\\")[2]+\
                 " " + self.vc_build_type + " \"debug|" + self.vc_build_platform + "\" /out " + output_file)

        # Get the build output
        lines = open(output_file).readlines()
        build_result = lines[-2]
        print(build_result)
        self.logger.info(build_result)
        # Check for failed build

        failed_builds = 0
        tempReResult = re.search("(\d*) failed",build_result)

        if tempReResult != None :
            failed_builds = int(tempReResult.group(1))
        if failed_builds != 0:
            print("Samples Building In Debug Failed!!")
            self.logger.critical("Samples Building Failed!")
            self.finish_script(1)

        # Build other samples
        all_samples = self.get_samples()
        for sample in list(all_samples.values()):
            if sample.is_other:
                self.build_other_proj(os.path.join(self.WORK_DIR, "Platform", "Win32", "Redist", "Samples", sample.name))

        # --------------------Delete stuff
        os.chdir(self.WORK_DIR + "\\Platform\\Win32\\Redist\\Samples\\" + self.bin_dir +"\\Release\\")
        os.system("del *.pdb")
        os.chdir(self.WORK_DIR + "\\Platform\\Win32\\Redist\\Samples\\" + self.bin_dir +"\\Debug\\")
        os.system("del *.pdb")
        os.system("del *.ilk")
        os.chdir(self.WORK_DIR + "\\Platform\\Win32\\Redist\\" + self.lib_dir + "\\")
        os.system("del nim*.*")

        os.chdir(self.WORK_DIR)

    def clean_up(self):
        temp_str = "Redist "+ self.redist_name +" Ended."
        print(temp_str)
        self.logger.info(temp_str)
        #self.finish_script(0)
class Command(object):
    def __init__(self, cmd):
        self.cmd = cmd
        self.process = None

    def run(self, timeout):
        def target():
            print('Thread started')
            self.process = subprocess.Popen(self.cmd, shell=True)
            self.process.communicate()
            print('Thread finished')

        thread = threading.Thread(target=target)
        thread.start()

        thread.join(timeout)
        if thread.is_alive():
            print('Terminating process')
            self.process.terminate()
            thread.join()
        print((self.process.returncode))
