import os
import sys
import re
import time
import traceback

packageFullPath = "..\..\..\..\..\PrimeSenseVersions.nsh"

def find_package_number(findStr, text):
    for line in text:
        temp = re.search(findStr, line)
        if temp != None:
            packageNumber = temp.group(1)
            return packageNumber

def copy_files_to_repository(SourcePath,RepositoryPath, BuildDate, PackageVersion, Bits, ProjectName,
                             Major_version, Minor_version, Maintenance_version, Build_version):
    fullVersion = Major_version + "." + Minor_version + "." + Maintenance_version + "." + Build_version
    destPath = os.path.join(RepositoryPath, BuildDate + "__" + PackageVersion, "Win" + Bits,
                            ProjectName + "-" + fullVersion)
    os.system("rmdir /S /q " + destPath)
    os.system("mkdir " + destPath)
    os.system("xcopy /E /I " + SourcePath + " " + destPath)

def copy_zip_to_repository(SourcePath,RepositoryPath, BuildDate, PackageVersion, Bits, ProjectName,
                             Major_version, Minor_version, Maintenance_version, Build_version):
    fullVersion = Major_version + "." + Minor_version + "." + Maintenance_version + "." + Build_version
    destPath = os.path.join(RepositoryPath, BuildDate + "__" + PackageVersion, "Win" + Bits,
                            ProjectName + "-" + fullVersion)
    os.system("rmdir /S /q " + destPath)
    os.system("mkdir " + destPath)
    os.system("xcopy /I " + SourcePath + " " + destPath)

def open_package_file(path):
    files = open(path).readlines()
    packageNumber = find_package_number("!define PACKAGE_VER\s+\"(\S+)\"", files)
    return packageNumber

if __name__ == "__main__":
    try:
        if len(sys.argv) != 10:
            print (("Usage: copyToRepository.py <FinalPath> <RepositoryPath> <BuildDate> <bits> " \
            + "<ProjectName> <Major_version> <Minor_version> <Maintenance_version> <Build_version>"))
            sys.exit(1)

        finalPath = sys.argv[1]
        repositoryPath = sys.argv[2]
        buildDate = sys.argv[3]
        bits = sys.argv[4]
        projectName = sys.argv[5]
        major_version = sys.argv[6]
        minor_version = sys.argv[7]
        maintenance_version = sys.argv[8]
        build_version = sys.argv[9]

        packageNumber = ''
        if not(os.path.exists(packageFullPath)):
            # Redist of OpenNI openSource
            packageFullPath = "..\..\..\..\..\..\PrimeSenseVersions.nsh"
            packageNumber = open_package_file(packageFullPath)
            if packageNumber == '':
                sys.exit(1)
            copy_zip_to_repository("..\..\..\..\*.zip",repositoryPath, buildDate, packageNumber, bits,
                                 projectName, major_version, minor_version, maintenance_version, build_version)
        else:
            # Redist of OpenNI
            packageNumber = open_package_file(packageFullPath)
            if packageNumber == '':
                sys.exit(1)
            copy_files_to_repository(finalPath,repositoryPath, buildDate, packageNumber, bits,
                                 projectName, major_version, minor_version, maintenance_version, build_version)

        sys.exit(0)
    except SystemExit as e:
        sys.exit(e)
    except:
        print ((traceback.print_exc()))
        sys.exit(1)
