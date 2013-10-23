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
import uuid
path_to_base = os.path.dirname(os.path.abspath('../../../Externals/PSCommon/Windows/CreateRedist/redist_base.py'))
if path_to_base not in sys.path:
    sys.path.insert(0, path_to_base)
import redist_base

class RedistOpenNI(redist_base.RedistBase):
    def __init__(self):
        #RedistBase.__init__(self)
        super(RedistOpenNI,self).__init__()
        self.VC_version = 10
        self.project_is_2010 = True
        self.vc_build_bits = "32"
        self.config_xml_filename = "OpenNi_Config.xml"
        self.redist_name = "OpenNI"
        self.redist_internal_name = 'OpenNI'
        self.product_name = 'OpenNI'
        self.doxy_file_name = 'Doxyfile'
        self.write_2010_sample_dependency = True
        self.internal_conf_name = 'Dev'
        self.SCRIPT_DIR = os.getcwd()

    def init_win_sdk_vars(self):
        """find Windows SDK install dir"""
        WIN_SDK_KEY = (win32con.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\Microsoft SDKs\Windows")
        WIN_SDK_VALUES = [("CurrentInstallFolder", win32con.REG_SZ)]
        self.WIN_SDK_INST_DIR = redist_base.get_reg_values(WIN_SDK_KEY, WIN_SDK_VALUES)[0]

    def init_version_vars(self):
        """
        Init Verison vars.
        currently not called in script. Taken from Redist_OpenNi.
        """
        version_file = open(self.WORK_DIR + "/Include/XnVersion.h").read()
        version_major = re.search(r"define XN_MAJOR_VERSION (\d+)", version_file).groups()[0]
        self.version_minor = re.search(r"define XN_MINOR_VERSION (\d+)", version_file).groups()[0]
        version_maintenance = re.search(r"define XN_MAINTENANCE_VERSION (\d+)", version_file).groups()[0]
        version_build = re.search(r"define XN_BUILD_VERSION (\d+)", version_file).groups()[0]
        self.version_string = version_major + "." + self.version_minor + "." + version_maintenance + "." + version_build

    def build_project(self):
        #--------------Build Project---------------------------------------------------#
        self.build_proj_solution()
        self.build_java_wrapper()
        self.build_other_samples()
        self.build_dotnet_policy()
        self.make_doxy()
        self.create_redist_dir()
        self.copy_files_to_redist()
        self.creating_samples()
        self.remove_read_only_attributes()

    def build_dotnet_policy(self):
        # Build the .NET publisher policy assembly (unfortunately, this should be done manually):
        for minor in range(1, int(self.version_minor)+1):
            policy_name = 'Policy.1.' + str(minor) + '.OpenNI.Net.dll'
            cmd = self.WIN_SDK_INST_DIR + '\\Bin\\al.exe'
            cmd += ' /link:' + self.WORK_DIR + '\\Wrappers\\OpenNI.Net\\PublisherPolicy.config'
            cmd += ' /out:' + self.WORK_DIR + '\\Platform\\Win32\\' + self.bin_dir + '\\Release\\' + policy_name
            cmd += ' /keyFile:' + self.WORK_DIR + '\\Platform\\Win32\\Build\\Wrappers\\OpenNI.Net\\OpenNI.snk'
            cmd += ' /platform:'
            if self.vc_build_bits=="32":
                cmd += 'x86'
            else:
                cmd += 'x64'
            cmd += ' /version:' + self.version_string

            if subprocess.call(cmd) != 0:
                print("Building Publisher Policy Failed!!")
                self.logger.critical("Building Publisher Policy Failed!")
                self.finish_script(1)

    def build_java_wrapper(self):
        "Builds the Java wrapper"
        self.build_other_proj(self.WORK_DIR + '\\Platform\\Win32\\Build\\Wrappers\\OpenNI.java')

    def update_installer_clr_policy(self):
        updater = UpdateInstallerCLRPolicy(os.path.join(self.WORK_DIR,self.inst_proj_path))
        for minor in range(1, int(self.version_minor)+1):
            [missing,broken] = updater.is_missing(minor)
            if broken:
                self.logger.error('WXS file seems to be broken on definitions of dotnet policy for version 1.%d.'%minor)
            elif missing:
                self.logger.info('WXS file missing definitions of dotnet policy for version 1.%d.'%minor)
                updater.add_policy_clauses(minor)
                self.logger.info('WXS was succesfully updated to contain definitions of dotnet policy for version 1.%d.'%minor)
            else:
                self.logger.info('WXS contains definitions of dotnet policy for version 1.%d.'%minor)

    def copy_doxy_files(self):
        os.system("""copy "html\*.chm" ..\..\Documentation""")

    def copy_files_to_redist(self):
        #-------------Copy files to redist---------------------------------------------#
        print("* Copying files to redist dir...")
        self.logger.info("Copying files to redist dir...")

        os.chdir(self.WORK_DIR + "\\Platform\\Win32")

        #license
        os.system ("copy \"..\\..\\LICENSE\" Redist")
        os.system ("copy \"..\\..\\NOTICE\" Redist")

        #bin
        os.system ("copy " + self.bin_dir + "\\Release\\OpenNI*.dll Redist\\" + self.bin_dir)
        os.system ("copy " + self.bin_dir + "\\Release\\OpenNI.Net.dll Redist\\" + self.bin_dir)
        os.system ("copy " + self.bin_dir + "\\Release\\Policy.1.*.OpenNI.Net.dll Redist\\" + self.bin_dir)
        #os.system ("copy " + "..\\..\\Wrappers\\OpenNI.Net\\PublisherPolicy.config Redist\\" + self.bin_dir)
        os.system ("copy " + self.bin_dir + "\\Release\\nimCodecs*.dll Redist\\" + self.bin_dir)
        os.system ("copy " + self.bin_dir + "\\Release\\nimMockNodes*.dll Redist\\" + self.bin_dir)
        os.system ("copy " + self.bin_dir + "\\Release\\nimRecorder*.dll Redist\\" + self.bin_dir)
        os.system ("copy " + self.bin_dir + "\\Release\\niReg*.exe Redist\\" + self.bin_dir)
        os.system ("copy " + self.bin_dir + "\\Release\\niLicense*.exe Redist\\" + self.bin_dir)
        os.system ("copy " + self.bin_dir + "\\Release\\OpenNI.jni.dll Redist\\" + self.bin_dir)
        os.system ("copy " + self.bin_dir + "\\Release\\org.openni.jar Redist\\" + self.bin_dir)

        for minor in range(1, int(self.version_minor)+1):
            policy_congfig_name = 'PublisherPolicy1.' + str(minor) + '.config'
            os.system ("copy " + "..\\..\\Wrappers\\OpenNI.Net\\PublisherPolicy.config Redist\\" + self.bin_dir + "\\" + policy_congfig_name)

        #lib
        os.system ("copy " + self.lib_dir + "\\Release\\*.* Redist\\" + self.lib_dir)

        #docs
        os.system ("copy \"..\\..\\Documentation\\*.pdf\" Redist\\Documentation")
        os.system ("copy \"..\\..\\Documentation\\*.chi\" Redist\\Documentation")
        os.system ("copy \"..\\..\\Documentation\\*.chm\" Redist\\Documentation")

        #include
        os.system ("xcopy /S ..\\..\\Include\\* Redist\\Include\\")

        #driver
        os.system ("xcopy /S Driver\\Bin\\*.* Redist\\Driver\\")

        #samples
        os.system ("copy Redist\\Samples\\" + self.bin_dir + "\\Release\\*.* Redist\\Samples\\" + self.bin_dir + "\\Debug")
        os.system ("xcopy /S Build\\Res\\*.* Redist\\Samples\\Res\\")

        #data
        os.system ("copy ..\\..\\Data\\SamplesConfig.xml Redist\\Data\\SamplesConfig.xml")

        # Copy the glut dll to the samples
        os.system ("xcopy /S .\\" + self.bin_dir + "\\Release\\glut*.dll Redist\\Samples\\" + self.bin_dir + "\\Release\\")
        os.system ("xcopy /S .\\" + self.bin_dir + "\\Release\\glut*.dll Redist\\Samples\\" + self.bin_dir + "\\Debug\\")

        # Copy the wrappers to the samples
        shutil.copy(os.path.join(self.bin_dir, "Release", "OpenNI.net.dll"), os.path.join("Redist", "Samples", self.bin_dir, "Release"))
        shutil.copy(os.path.join(self.bin_dir, "Release", "OpenNI.net.dll"), os.path.join("Redist", "Samples", self.bin_dir, "Debug"))
        shutil.copy(os.path.join(self.bin_dir, "Release", "org.openni.jar"), os.path.join("Redist", "Samples", self.bin_dir, "Release"))
        shutil.copy(os.path.join(self.bin_dir, "Release", "org.openni.jar"), os.path.join("Redist", "Samples", self.bin_dir, "Debug"))

        # Copy the release notes
        os.system ("copy .\\ReleaseNotes.txt Redist\\Documentation\\")

        os.chdir(self.WORK_DIR)

    def fix_file(self,arg,dirname,fname):
        "Fixes paths for all the files in fname"
        for filename in fname:
            filePath = dirname + "\\" + filename
            if os.path.isdir(filePath):
                continue

            ext = ['icproj','vcproj','vcxproj','csproj','cpp','h','c','ini','cs','py','bat','java']
            file_ext = os.path.splitext(filename)[1][1:]
            if file_ext in ext:
                file = open(filePath, "r+")
                s = file.read()
                file.seek(0)

                olds = s
                # Fix include paths
                s = re.sub(r"../../../../../Include",r"$(OPEN_NI_INCLUDE)",s)
                s = re.sub(r"..\\..\\..\\..\\..\\Include",r"$(OPEN_NI_INCLUDE)",s)
                
                sample_name = os.path.basename(dirname)
                s = re.sub(r"..\\..\\..\\..\\..\\Samples\\" + sample_name, r".", s)
                s = re.sub(r"../../../../../Samples/" + sample_name, r".", s)
                
                # fix path to "Data" folder
                s = re.sub(r"..\\\\..\\\\..\\\\..\\\\Data\\",r"..\\\\..\\\\..\\\\Data\\",s)
                s = re.sub(r"..\\..\\..\\..\\Data\\",r"..\\..\\..\\Data\\",s)
                s = re.sub("../../../../Data/",r"../../../Data/",s)

                # fix path to res files
                s = re.sub(r"..\\..\\Res\\",r"..\\Res\\",s)

                # fix path to java build script
                s = re.sub(r"..\\..\\BuildJava.py", r"..\\Build\\BuildJava.py", s)
                
                # fix bin and lib path for 32 bit configuration
                s = re.sub(r"../../../Bin/",r"../Bin/",s)
                s = re.sub(r"..\\..\\..\\Bin\\",r"../Bin/",s)
                s = re.sub(r"../../../Lib/\$\(Configuration\)",r"$(OPEN_NI_LIB)",s)
                s = re.sub(r"..\\..\\..\\Lib\\\$\(Configuration\)",r"$(OPEN_NI_LIB)",s)
                
                # fix bin and lib path for 64 bit configuration
                s = re.sub(r"../../../Bin64/",r"../Bin64/",s)
                s = re.sub(r"..\\..\\..\\Bin64\\",r"../Bin64/",s)
                s = re.sub(r"../../../Lib64/\$\(Configuration\)",r"$(OPEN_NI_LIB64)",s)
                s = re.sub(r"..\\..\\..\\Lib64\\\$\(Configuration\)",r"$(OPEN_NI_LIB64)",s)

                # fix general path problems
                s = re.sub(r"..\\..\\..\\..\\..\\",r"..\\..\\",s)
                s = re.sub(r"../../../../../",r"../../",s)
                
                # remove source control integraion (if exists)
                s = re.sub(r".*SccProjectName=\".*", r"", s);
                s = re.sub(r".*SSccAuxPath=\".*", r"", s);
                s = re.sub(r".*SccLocalPath=\".*", r"", s);

                # fix csproj link problem
                if file_ext == "csproj":
                    sample_name = os.path.basename(dirname)
                    link_re = r"<Compile Include=\"\.\\(?P<file>.*)\">\n"
                    link_re += r"\s*<Link>(?P=file)</Link>"
                    compiled_re = re.compile(link_re, re.MULTILINE)
                    s = compiled_re.sub("<Compile Include=\"\g<file>\">", s)
                    
                # remove dependencies from samples vcxproj to non-sample projects
                if file_ext == "vcxproj":
                    # dependency to another sample will be to ..\<sample>
                    compiled_start = re.compile(r"<ProjectReference Include=\"\.\.\\\.\.\\")
                    compiled_end = re.compile(r"</ProjectReference>")
                    while True:
                        match_start = compiled_start.search(s)
                        if match_start is None:
                            break
                        # look for end
                        match_end = compiled_end.search(s, match_start.end())
                        s = s[:match_start.start()] + s[match_end.end():]
                    
                file.truncate()
                file.write(s)
                file.close()

    def redist_openni(self):
        """
        Main Script.
        """
        self.init_logger(self.redist_name)
        self.check_args(sys.argv)
        self.init_vars()
        self.init_vs_vars()
        print (("\n*** Build target is:" + self.vc_build_bits + " Doxy:" + str(self.Make_Doxy) + " Rebuild:" + sys.argv[3] + " VC Version:" + str(self.VC_version) + " ***"))
        self.init_win_sdk_vars()
        self.init_version_vars()
        self.print_message()
        self.logger.info("PrimeSense OpenNI Redist Started")
        self.build_project()
        self.fixing_files()
        
        # set OpenNI environment variables to point to the redist folder (so that samples can compile correctly)
        REDIST_DIR = os.path.join(self.WORK_DIR,"Platform","Win32","Redist")
        os.environ['OPEN_NI_INCLUDE'] = REDIST_DIR + "\\Include"
        os.environ['OPEN_NI_LIB'] = REDIST_DIR + "\\Lib"
        os.environ['OPEN_NI_LIB64'] = REDIST_DIR + "\\Lib64"
        self.build_samples()
        self.update_installer_clr_policy()
        [dev,redist] = self.make_installer(self.SCRIPT_DIR)
        if not dev:
            return 1
        else:
            if not redist:
                return 1
        self.clean_up()
        return 0


class UpdateInstallerCLRPolicy:
    """
    Covers up for missing parts in the installer
    that are responsible for dotnet-publisher-policy stuff.
    """
    def __init__(self,inst_proj_full_path):
        self.wxs_file = os.path.join(inst_proj_full_path,'OpenNI.wxs')
    def is_missing(self, minor):
        """
        Returns [missing,broken] if this version is missing,
        or broken if only one of the clauses exist.
        """
        #checks the WXS file if 2 patterns are found
        #<Component Id="OpenNIPolicy1.%d"
        #<ComponentRef Id="OpenNIPolicy1.%d"/>
        comp_id_clause = False
        rev_re = re.compile('<Component Id=\"OpenNIPolicy1\.%d\"'%minor)
        all_lines = open(self.wxs_file).readlines()
        re_res = rev_re.search(str(all_lines))
        if re_res:
            comp_id_clause = True
        comp_ref_clause = False
        comp_ref_str = '<ComponentRef Id=\"OpenNIPolicy1\.%d\"/>'%minor
        rev_re = re.compile(comp_ref_str)
        re_res = rev_re.search(str(all_lines))
        if re_res:
            comp_ref_clause = True
        missing = not comp_ref_clause and not comp_id_clause
        broken = (comp_id_clause and not comp_ref_clause) or (comp_ref_clause and not comp_id_clause)
        return [missing,broken]
    def add_policy_clauses(self,minor):
        """
        Adds the two clauses into the file.
        """
        comp_ref_str = '<ComponentRef Id=\"OpenNIPolicy1.%d\"/>'%minor
        comp_ref_anchor = '<ComponentRef Id="OpenNINET"/>'
        redist_base.regx_replace(comp_ref_anchor,'%s\n%s'%(comp_ref_anchor,comp_ref_str),self.wxs_file)

        guid = uuid.uuid1()
        comp_id_str = r'<Component Id="OpenNIPolicy1.%d" Guid="%s" DiskId="1">'%(minor,str(guid)) + '\n' + \
        r'<File Id="OpenNIPolicy1.%d" Name="Policy.1.%d.OpenNI.Net.dll" Assembly=".net" KeyPath="yes" '%(minor,minor) +  \
        r'Source="$(var.OpenNIFilesDir)\\bin$(var.PlatformSuffix)\Policy.1.%d.OpenNI.Net.dll" />'%(minor) + '\n' + \
        r'<File Id="OpenNIPolicyConfig1.%d" Name="PublisherPolicy1.%d.config" KeyPath="no" '%(minor,minor) + \
        r'Source="$(var.OpenNIFilesDir)\\bin$(var.PlatformSuffix)\PublisherPolicy1.%d.config" />'%(minor) + '\n' + \
        r'</Component>'
        comp_id_anchor = r'<Component Id="OpenNINET"'
        redist_base.regx_replace(comp_id_anchor,'%s\n%s'%(comp_id_str,comp_id_anchor),self.wxs_file)

if __name__ == "__main__":
    redist = RedistOpenNI()
    rc =  redist.redist_openni()
    redist.finish_script(rc)
