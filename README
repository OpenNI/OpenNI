OpenNI (Version 1.5.4.0 - May 7th 2012)
---------------------------------------

Website: http://www.primesense.com
Forum: http://groups.google.com/group/openni-dev
Wiki: http://wiki.openni.org

Binaries are available at:
http://www.openni.org/Downloads/OpenNIModules.aspx
(The "OpenNI Binaries" section)

Sources are available at:
https://github.com/OpenNI/OpenNI
or
https://github.com/OpenNI/OpenNI/tree/unstable
for unstable branch

Release Notes:
--------------
* At the moment, the default is to compile the code with SSE3 support (this is also true for the supplied binaries). 
  If you have a CPU without such support, please remove the sse compiler flags from the make files.
  (A good hint for this error is that you encounter an "illegal instructions" messages)
* MacOSX: Only OSX 10.6 (Snow Leopard) and above with an Intel based CPU is currently supported.
* MacOSX: Drawing the depth/image maps via the mono .NET wrapper can be slow and will cause the FPS to drop.
  
Build Notes:
------------
Windows:
	Requirements:
		1) Microsoft Visual Studio 2010
		   From: http://msdn.microsoft.com/en-us/vstudio/bb984878.aspx
		2) Python 2.6+/3.x
		   From: http://www.python.org/download/
		3) PyWin32
		   From: http://sourceforge.net/projects/pywin32/files/pywin32/
		   Please make sure you download the version that matches your exact python version.
		4) WIX 3.5
		   From: http://wix.codeplex.com/releases/view/60102
		5) JDK 6.0
           From: http://www.oracle.com/technetwork/java/javase/downloads/jdk-6u32-downloads-1594644.html
		   You must also define an environment variable called "JAVA_HOME" that points to the JDK installation directory.
		   For example: set JAVA_HOME=c:\Program Files (x86)\Java\jdk1.6.0_32

	Optional requirements (To build the USB device driver):
		1) Microsoft WDK
		   From: http://www.microsoft.com/en-us/download/details.aspx?displaylang=en&id=11800
		   
		The package already includes a precompiled and digitally signed 32/64 bit driver.
	
	Optional Requirements (To build the documentation):
		1) Doxygen
		   From: http://www.stack.nl/~dimitri/doxygen/download.html#latestsrc
		2) GraphViz
		   From: http://www.graphviz.org/Download_windows.php
	
	Building OpenNI:
	    1) Uninstall the previous version.
		2) Go to the directory: "Platform\Win32\CreateRedist".
		   x86 32-bit - Run the script: "RedistMaker.bat y 32 y".
		   x64 64-bit - Run the script: "RedistMaker.bat y 64 y".
		   This will compile and prepare the redist exe files that includes everything.
		3) Install the exe you've just made which is located in Platform\Win32\CreateRedist\FinalXX\OPENNI-WinXX-1.X.X.X.exe 
		   (XX being the number of bits: 32 or 64)
		   The installer will also create the necessary environment variables (OPEN_NI_xxx), add the DLLs to the system path and register the internal modules with NiReg.
		   
		The visual studio solution is located in: Platform\Win32\Build\OpenNI.sln.
		
		When doing development it is recommended that you change the environment variables to point to your development directory instead of the default C:\Program Files\OpenNI.
		(This can save you lots back and forth file copying...)

		Important: Please note that even though the directory is called Win32, you can also use it to compile it for 64-bit targets (Win64/AMD64/x64).
	
	Building the USB driver (Optional):
		Simply go into the directory "OpenNI\Platform\Win32\Driver\Build" and run the "BuildAll.bat" script.
		For your development convenience, you can also use the solution: "Platform\Win32\Driver\Build\psdrv3.sln" but official driver builds should only be made with the batch file above, that uses the proper DDK environment.
		Note: The driver build tool requires a system environment variable called "DDKPATH" that points to the WDK installation dir (for example: "c:\WinDDK\7100.0.0"). To add an environment variable please follow these steps: Control Panel -> System -> Advanced -> Environment Variables -> New (at the "System Variables" tab).	
	   
Linux:
	Requirements:
		1) GCC 4.x
		   From: http://gcc.gnu.org/releases.html
		   Or via apt:
		   sudo apt-get install g++
		2) Python 2.6+/3.x
		   From: http://www.python.org/download/
		   Or via apt:
		   sudo apt-get install python
		3) LibUSB 1.0.x
		   From: http://sourceforge.net/projects/libusb/files/libusb-1.0/
		   Or via apt:
		   sudo apt-get install libusb-1.0-0-dev
		4) FreeGLUT3
		   From: http://freeglut.sourceforge.net/index.php#download
		   Or via apt:
		   sudo apt-get install freeglut3-dev
		5) JDK 6.0
		   From: http://www.oracle.com/technetwork/java/javase/downloads/jdk-6u32-downloads-1594644.html
		   Or via apt:
		     Ubuntu 10.x:				
		       sudo add-apt-repository "deb http://archive.canonical.com/ lucid partner"
		       sudo apt-get update
		       sudo apt-get install sun-java6-jdk
		     Ubuntu 12.x:				
		       sudo apt-get install openjdk-6-jdk

		   
	Optional Requirements (To build the documentation):
		1) Doxygen
		   From: http://www.stack.nl/~dimitri/doxygen/download.html#latestsrc
		   Or via apt:
		   sudo apt-get install doxygen
		2) GraphViz
		   From: http://www.graphviz.org/Download_linux_ubuntu.php
		   Or via apt:
		   sudo apt-get install graphviz

	Optional Requirements (To build the Mono wrapper):
		1) Mono
		   From: http://www.go-mono.com/mono-downloads/download.html
		   Or via apt:
		   sudo apt-get install mono-complete
		   
	Building OpenNI:
		1) Go into the directory: "Platform/Linux/CreateRedist".
		   Run the script: "./RedistMaker".
		   This will compile everything and create a redist package in the "Platform/Linux/Redist" directory.
		   It will also create a distribution in the "Platform/Linux/CreateRedist/Final" directory.
		2) Go into the directory: "Platform/Linux/Redist".
		   Run the script: "sudo ./install.sh" (needs to run as root)

  		   The install script copies key files to the following location:
		       Libs into: /usr/lib
		       Bins into: /usr/bin
		       Includes into: /usr/include/ni
		       Config files into: /var/lib/ni
			
		To build the package manually, you can run "make" in the "Platform\Linux\Build" directory.
		If you wish to build the Mono wrappers, also run "make mono_wrapper" and "make mono_samples".
		
	Building OpenNI using a cross-compiler:
		1) Make sure to define two environment variables:
		   - <platform>_CXX - the name of the cross g++ for platform <platform>
		   - <platform>_STAGING - a path to the staging dir (a directory which simulates the target root filesystem).
		   Note that <platform> should be upper cased.
		   For example, if wanting to compile for ARM from a x86 machine, ARM_CXX and ARM_STAGING should be defined.
		2) Go into the directory: "Platform/Linux/CreateRedist".
		   Run: "./RedistMaker <platform>" (for example: "./RedistMake Arm").
		   This will compile everything and create a redist package in the "Platform/Linux/Redist" directory.
		   It will also create a distribution in the "Platform/Linux/CreateRedist/Final" directory.
		3) To install OpenNI files on the target file system:
		   Go into the directory: "Platform/Linux/Redist".
		   Run the script: "./install.sh -c $<platform>_STAGING" (for example: "./install.sh -c $ARM_STAGING").

  		   The install script copies key files to the following location:
		       Libs into: STAGING/usr/lib
		       Bins into: STAGING/usr/bin
		       Includes into: STAGING/usr/include/ni
		       Config files into: STAGING/var/lib/ni
			
		To build the package manually, you can run "make PLATFORM=<platform>" in the "Platform\Linux\Build" directory.
		If you wish to build the Mono wrappers, also run "make PLATFORM=<platform> mono_wrapper" and "make PLATFORM=<platform> mono_samples".
	
MacOSX:
	Requirements:
		1) For Mac OSX 10.7, Xcode 4.3.2
		      From: http://developer.apple.com/devcenter/mac/index.action
		      http://adcdownload.apple.com/Developer_Tools/xcode_4.3.2/xcode_432_lion.dmg    
   
		      You will also need to download and install the "Commandline Tools for XCode - Late March 2012" package:
		      http://adcdownload.apple.com/Developer_Tools/command_line_tools_for_xcode_4.4__late_march_2012/cltools_lion_latemarch12.dmg
		   
		      Note: Since Xcode is now a normal application, it no longer automatically install itself. You have to copy&paste Xcode yourself into your Applications folder. 
			        (or run the application "Install Xcode" from your Applications folder / LaunchPad). 		   
			        It's also recommended to run the following command after you do the above:
				      sudo xcode-select -switch /Applications/Xcode.app/
		   
		   For Mac OSX 10.6, Xcode 3.2.6:
		      From: http://developer.apple.com/devcenter/mac/index.action
		      http://developer.apple.com/devcenter/download.action?path=/Developer_Tools/xcode_3.2.6_and_ios_sdk_4.3__final/xcode_3.2.6_and_ios_sdk_4.3.dmg
			   
		   Please note that you need to register as a mac developer (It's free!).
		   		   
		2) LibUSB 1.0.x (The patched development tree)
		   This can be installed by two ways:		   
		   A) MacPorts:      
		      From: http://www.macports.org/install.php
			  For Mac OSX 10.7: https://distfiles.macports.org/MacPorts/MacPorts-2.0.4-10.7-Lion.dmg
			  For Mac OSX 10.6: https://distfiles.macports.org/MacPorts/MacPorts-1.9.2-10.6-SnowLeopard.dmg
			  * You will also need to install LibTool:
			      sudo port install libtool			  
			  * And finally, LibUSB itself:
			      sudo port install libusb-devel +universal
		      Note: Do not forget the +universal, it's very important!!
		            If you're previously already installed libusb-devel then use "sudo port uninstall libusb-devel" and reinstall it again with the +universal flag.
					
		   B) Compile the source & install LibUSB yourself:
			  * Change the working directory to the prerequisites directory: "cd Platform\Linux-x86\Build\Prerequisites"
			  * Extract the tar file by running: "tar -xvjf libusb-1.0.8-osx.tar.bz2"
			  * Change the working directory to the libusb directory: "cd libusb"
			  * Run the following commands to build and install libusb:
			      /autogen.sh
			      ./configure LDFLAGS='-framework IOKit -framework CoreFoundation -arch i386 -arch x86_64 ' CFLAGS='-arch i386 -arch x86_64' --disable-dependency-tracking --prefix=/opt/local
			      make
			      sudo make install
				 
	Optional Requirements (To build the documentation):
		1) Doxygen
		   Install via MacPorts:
		   sudo port install doxygen
		2) GraphViz
		   Install via MacPorts:
		   sudo port install graphviz

	Optional Requirements (To build the Mono wrapper):
		1) Mono
		   From: http://www.go-mono.com/mono-downloads/download.html
		   http://download.mono-project.com/archive/2.10.9/macos-10-x86/10/MonoFramework-MDK-2.10.9_10.macos10.xamarin.x86.dmg
		   Install via MacPorts:
		   sudo port install mono
		   
	Building OpenNI:
		1) Go into the directory: "Platform/Linux-x86/CreateRedist".
		   Run the script: "./RedistMaker".
		   This will compile everything and create a redist package in the "Platform/Linux-x86/Redist" directory.
		   It will also create a distribution in the "Platform/Linux-x86/CreateRedist/Final" directory.
		2) Go into the directory: "Platform/Linux-x86/Redist".
		   Run the script: "sudo ./install.sh" (needs to run as root)

  		   The install script copies key files to the following location:
		       Libs into: /usr/lib
		       Bins into: /usr/bin
		       Includes into: /usr/include/ni
		       Config files into: /var/lib/ni
			
		To build the package manually, you can run "make" in the "Platform\Linux-x86\Build" directory.
		If you wish to build the Mono wrappers, also run "make mono_wrapper" and "make mono_samples".
