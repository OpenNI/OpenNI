#!/bin/bash -e

if [ -z "$INSTALL_LIB" ]; then
  INSTALL_LIB=/usr/lib
fi
if [ -z "$INSTALL_BIN" ]; then
  INSTALL_BIN=/usr/bin
fi
if [ -z "$INSTALL_INC" ]; then
  INSTALL_INC=/usr/include/ni
fi
if [ -z "$INSTALL_VAR" ]; then
  INSTALL_VAR=/var/lib/ni
fi

if [ "`uname -s`" == "Darwin" ]; then
	MODULES="libnimMockNodes.dylib libnimCodecs.dylib libnimRecorder.dylib"
else
	MODULES="libnimMockNodes.so libnimCodecs.so libnimRecorder.so"
fi

SCRIPT_DIR=`pwd`/`dirname $0`

# read script args
INSTALL="1"
REGISTER="1"

while (( "$#" )); do
	case "$1" in
	"-i")
		INSTALL="1"
		;;
	"-u")
		INSTALL="0"
		;;
	"-n")
		REGISTER="0"
		;;
	*)
		echo "Usage: $0 [options]"
		echo "Available options:"
		printf "\t-i\tInstall (default)\n"
		printf "\t-u\tUninstall\n"
		printf "\t-n\tDo not (un)register the modules\n"
		exit 1
		;;
	esac
	shift
done

# create file list
LIB_FILES=`ls $SCRIPT_DIR/Lib/*`
BIN_FILES=`ls $SCRIPT_DIR/Bin/ni*`

if [ "$INSTALL" == "1" ]; then

	mkdir -p $INSTALL_LIB
	mkdir -p $INSTALL_BIN
	mkdir -p $INSTALL_INC
	mkdir -p $INSTALL_VAR

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

	if [ "$REGISTER" == "1" ]; then
		# register modules
		for module in $MODULES; do
			printf "registering module '$module'..."
			niReg -r $INSTALL_LIB/$module
			printf "OK\n"
		done
	fi

	if [ "$REGISTER" == "1" ]; then
            # mono
	    if [ -f /usr/bin/gmcs ]
	    then
		gacutil -i Bin/OpenNI.net.dll -package 2.0
	    fi
        fi

else
	if [ "$REGISTER" == "1" ]; then
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

    if [ "$REGISTER" == "1" ]; then
	# mono
	if [ -f /usr/bin/gmcs ]
	then
		printf "Removing OpenNI.net: "
		gacutil -u OpenNI.net
	fi
    fi
fi

printf "\n*** DONE ***\n\n"

