#!/bin/sh -e

INSTALL_LIB=/usr/lib
INSTALL_BIN=/usr/bin
INSTALL_INC=/usr/include/ni
INSTALL_VAR=/var/lib/ni

OS_NAME=`uname -s`

case $OS_NAME in
Darwin)
	MODULES="libnimMockNodes.dylib libnimCodecs.dylib libnimRecorder.dylib"
	;;
*)
	MODULES="libnimMockNodes.so libnimCodecs.so libnimRecorder.so"
	;;
esac

SCRIPT_DIR=`pwd`/`dirname $0`

# create file list
LIB_FILES=`ls $SCRIPT_DIR/Lib/*`
BIN_FILES=`ls $SCRIPT_DIR/Bin/ni*`

case $1 in
-i | "")
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
		niReg -r $INSTALL_LIB/$module
		printf "OK\n"
	done

        # mono
	if [ -f /usr/bin/gmcs ]
	then
		gacutil -i Bin/OpenNI.net.dll -package 2.0
	fi
	;;
	
-u)
	printf "Uninstalling OpenNI\n"
	printf "****************************\n\n"

	# unregister modules
	for module in $MODULES; do
    	printf "unregistering module '$module'..."
        if niReg -u $INSTALL_LIB/$module; then
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
	if [ -f /usr/bin/gmcs ]
	then
		printf "Removing OpenNI.net: "
		gacutil -u OpenNI.net
	fi
	;;
*) 
	echo "Usage: $0 [options]"
	echo "Available options:"
	printf "\t-i\tInstall (default)\n"
	printf "\t-u\tUninstall\n"
	exit 1
	;;
esac

printf "\n*** DONE ***\n\n"

