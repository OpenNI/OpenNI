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
import os
import subprocess
import sys
import shutil

# take script dir
SCRIPT_DIR = os.path.abspath(os.path.dirname(sys.argv[0]))

# parse command line
if len(sys.argv) < 5:
    print (("usage: ", sys.argv[0], " <32|64> <ProjDir> <SourceDir> <Name> [NeededJarFiles] [MainClass]"))
    exit(1)

if sys.argv[1] == "" or sys.argv[1] == "32":
    bin_dir = "Bin"
elif sys.argv[1] == "64":
    bin_dir = "Bin64"
else:
    print ('First argument must be "32", "64" or empty (32)')
    exit(1)
    
proj_dir = os.path.abspath(sys.argv[2])
source_dir = os.path.abspath(os.path.join(proj_dir, sys.argv[3]))
proj_name = sys.argv[4]
needed_jar_files = ""
main_class = ""
if len(sys.argv) > 5:
    needed_jar_files = sys.argv[5]
if len(sys.argv) > 6:
    main_class = sys.argv[6]

RELEASE_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, "..", bin_dir, "Release"))
DEBUG_DIR = os.path.abspath(os.path.join(SCRIPT_DIR, "..", bin_dir, "Debug"))

JAR_FILE = os.path.join(RELEASE_DIR, proj_name + '.jar')
BATCH_FILE = os.path.join(RELEASE_DIR, proj_name + '.bat')

# make sure JAVA_HOME is set
JAVA_HOME = os.path.expandvars("$JAVA_HOME")
if JAVA_HOME == "":
    print ("JAVA_HOME is not set!")
    exit(1)
    
CLASS_PATH = os.path.expandvars("$CLASSPATH")

TEMP_BUILD_DIR = os.path.join(proj_dir, "bin")

# create bin dir if needed
if not os.path.exists(TEMP_BUILD_DIR):
    os.mkdir(TEMP_BUILD_DIR)
 
# build
cmd = '"' + os.path.join(JAVA_HOME, 'bin\javac.exe') + '" '
if needed_jar_files != "":
    # add class path
    cmd += '-cp "' + CLASS_PATH + ';'
    needed_list = needed_jar_files.split(';')
    for needed in needed_list:
        cmd += os.path.join(RELEASE_DIR, needed) + ';'
    cmd += '" '

cmd += '-d ' + TEMP_BUILD_DIR + ' ' + os.path.join(source_dir, '*.java')
res = subprocess.call(cmd)
if res != 0:
    print ("Failed to build!")
    exit(1)

# create JAR file
cmd = '"' + os.path.join(JAVA_HOME, 'bin\jar.exe') + '" -cf'
manifest = main_class != "" or needed_jar_files != ""
if manifest:
    # add manifest
    TEMP_MANIFEST_FILE = os.path.join(proj_dir, "Manifest.txt")
    manifest = open(TEMP_MANIFEST_FILE, 'w')
    if needed_jar_files != "":
        manifest.write("Class-Path:")
        needed_list = needed_jar_files.split(';')
        for needed in needed_list:
            manifest.write(" " + needed)
        manifest.write('\n')
    if main_class != "":
        manifest.write("Main-Class: " + main_class + "\n")
    manifest.close()
    cmd += 'm'
cmd += ' ' + JAR_FILE + ' '
if manifest:
    cmd += TEMP_MANIFEST_FILE + ' '
cmd += '-C ' + TEMP_BUILD_DIR + ' .'
res = subprocess.call(cmd)
if res != 0:
    print ("Failed to jar!")
    exit(1)

# copy jar to Bin/Debug
shutil.copy(JAR_FILE, DEBUG_DIR)
    
# create batch file (by default, windows does not open a console when double-clicking jar files)
if main_class != "":
    batch = open(BATCH_FILE, 'w')
    batch.write('java -jar ' + proj_name + '.jar\n')
    batch.close()

    # copy batch to Bin/Debug
    shutil.copy(BATCH_FILE, DEBUG_DIR)
