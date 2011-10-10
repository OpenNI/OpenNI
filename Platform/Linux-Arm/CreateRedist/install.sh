#!/bin/bash -e


STAGING_DIR=$2

INSTALL_LIB=$STAGING_DIR/usr/lib
INSTALL_BIN=$STAGING_DIR/usr/bin
INSTALL_INC=$STAGING_DIR/usr/include/ni
INSTALL_VAR=$STAGING_DIR/var/lib/ni

# Create staging dirs if needed
if [ "$STAGING_DIR" != "" ]; then

	mkdir -p $STAGING_DIR
	mkdir -p $INSTALL_BIN
	mkdir -p $INSTALL_LIB
fi 

printf $INSTALL_BIN

MODULES="libnimMockNodes.so libnimCodecs.so libnimRecorder.so"

SCRIPT_DIR=`pwd`/`dirname $0`

# read script args
if [ "$1" == "-u" ]; then
	INSTALL="0"
else
	if [ "$1" == "-i" ]; then
		INSTALL="1"
	else
		echo "Usage: $0 [options]"
		echo "Available options:"
		printf "\t-i\tInstall (default)\n"
		printf "\t-u\tUninstall\n"
		exit 1
	fi
fi

# create file list
LIB_FILES=`ls $SCRIPT_DIR/Lib/*`
BIN_FILES=`ls $SCRIPT_DIR/Bin/*`

if [ "$INSTALL" == "1" ]; then

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


	# If we used this script for staging, do not register modules
	if [ "$STAGING_DIR" == "" ]; then 
		# register modules
		for module in $MODULES; do
			printf "registering module '$module'..."
			niReg -r $INSTALL_LIB/$module
			printf "OK\n"
		done
	fi

else
	if [ "$STAGING_DIR" == "" ]; then
		# unregister modules
		for module in $MODULES; do
	    	printf "unregistering module '$module'..."
		if niReg -u $INSTALL_LIB/$module; then
		    printf "OK\n"
		fi
		done
	fi

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
fi

printf "\n*** DONE ***\n\n"

