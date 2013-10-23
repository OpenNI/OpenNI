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

cmd += '-d "' + TEMP_BUILD_DIR + '" "' + os.path.join(source_dir, '*.java') + '"'
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
cmd += ' "' + JAR_FILE + '" '
if manifest:
    cmd += '"' + TEMP_MANIFEST_FILE + '" '
cmd += '-C "' + TEMP_BUILD_DIR + '" .'
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
