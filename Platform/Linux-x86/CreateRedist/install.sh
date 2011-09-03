#!/bin/sh -e

usage="
Usage: $0 [OPTIONS]
Installs OpenNI to current machine.

-i,--install
	Installs OpenNI (default mode)
-u,--uninstall
	Uninstalls OpenNI.
-c,--cross-compile-rootfs <path>
	Used for cross-compiling. Installs OpenNI to <path> instead of '/'.
-h,--help
	Shows this help screen.
"

OS_NAME=`uname -s`

case $OS_NAME in
Darwin)
	MODULES="libnimMockNodes.dylib libnimCodecs.dylib libnimRecorder.dylib"
	GACDIR=""
	;;
*)
	MODULES="libnimMockNodes.so libnimCodecs.so libnimRecorder.so"
	GACDIR="-gacdir $rootfs/usr"	
	;;
esac

SCRIPT_DIR=`pwd`/`dirname $0`

# create file list
LIB_FILES=`ls $SCRIPT_DIR/Lib/*`
BIN_FILES=`ls $SCRIPT_DIR/Bin/ni*`
JAR_FILES=`ls $SCRIPT_DIR/Jar/*.jar`

rootfs=

# parse command line
while [ "$1" ]; do
	case $1 in
	-i|--install)
		install=yes
		;;
	-u|--uninstall)
		uninstall=yes
		;;
	-c|--cross-staging-dir)
		shift
		rootfs=$1
		;;
	-h|--help) 
		echo "$usage"
		exit 0
		;;
	*)
		echo "Unrecognized option $1"
		exit 1
	esac
	shift
done

# default mode is install
if [ ! "$install" = yes ] && [ ! "$uninstall" = yes ]; then
	install=yes
fi

# validity check
if [ "$install" = yes ] && [ "$uninstall" = yes ]; then
	echo "-i and -u flags cannot be used together!"
	exit 1
fi

INSTALL_LIB=$rootfs/usr/lib
INSTALL_BIN=$rootfs/usr/bin
INSTALL_INC=$rootfs/usr/include/ni
INSTALL_VAR=$rootfs/var/lib/ni
INSTALL_JAR=$rootfs/usr/share/java

# make all calls into OpenNI run in this filesystem
export OPEN_NI_INSTALL_PATH=$rootfs
# make sure the staging dir OpenNI is the one being run
export LD_LIBRARY_PATH=$INSTALL_LIB

if [ "$install" = yes ]; then
	printf "Installing OpenNI\n"
	printf "****************************\n\n"
	
	# copy libraries
	printf "copying shared libraries..."
	cp $LIB_FILES $INSTALL_LIB
	printf "OK\n"

	# utilities
	printf "copying executables..."
	cp $BIN_FILES $INSTALL_BIN
	printf "OK\n"

	# include files
	printf "copying include files..."
	mkdir -p $INSTALL_INC
	cp -r Include/* $INSTALL_INC
	printf "OK\n"

	# create database dir
	printf "creating database directory..."
	mkdir -p $INSTALL_VAR
	printf "OK\n"

	# register modules
	for module in $MODULES; do
		printf "registering module '$module'..."
		$INSTALL_BIN/niReg -r $INSTALL_LIB/$module
		printf "OK\n"
	done

	# mono
	if [ -f $rootfs/usr/bin/gmcs -a -f Bin/OpenNI.net.dll ]; then
		printf "Installing .Net wrappers...\n"
		gacutil -i Bin/OpenNI.net.dll -package 2.0 $GACDIR
	fi

	# java wrappers
	printf "creating java bindings directory..."
	mkdir -p $INSTALL_JAR
	printf "OK\n"

	printf "Installing java bindings..."
	cp $JAR_FILES $INSTALL_JAR
	printf "OK\n"

	printf "\n*** DONE ***\n\n"

elif [ "$uninstall" = yes ]; then
	printf "Uninstalling OpenNI\n"
	printf "****************************\n\n"

	# unregister modules
	for module in $MODULES; do
    	printf "unregistering module '$module'..."
        if $INSTALL_BIN/niReg -u $INSTALL_LIB/$module; then
            printf "OK\n"
        fi
	done

	# include files
	printf "removing include files..."
	rm -rf $INSTALL_INC
	printf "OK\n"

	# binaries
	printf "removing executables..."
	for filename in $BIN_FILES; do
		rm -f $INSTALL_BIN/`basename $filename`
	done
	printf "OK\n"

	# libraries
	printf "removing shared libraries..."
	for filename in $LIB_FILES; do
		rm -f $INSTALL_LIB/`basename $filename`
	done
	printf "OK\n"

	# mono
	if [ -f $rootfs/usr/bin/gmcs -a -f Bin/OpenNI.net.dll ]; then
		printf "Removing .Net wrappers...\n"
		gacutil -u OpenNI.net $GACDIR
	fi

	# java
	printf "removing java bindings..."
	for filename in $JAR_FILES; do
		rm -f $INSTALL_JAR/`basename $filename`
	done
	printf "OK\n"
	
	printf "\n*** DONE ***\n\n"

fi

