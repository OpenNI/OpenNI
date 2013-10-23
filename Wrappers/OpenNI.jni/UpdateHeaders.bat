cd ..\..\Platform\Win32\Build\Wrappers\OpenNI.java
call Build.bat
cd ..\..\..\..\..\Wrappers\OpenNI.jni
"%JAVA_HOME%\bin\javah" -classpath ..\..\Platform\Win32\Build\Wrappers\OpenNI.java\bin org.openni.NativeMethods
CreateMethods.py