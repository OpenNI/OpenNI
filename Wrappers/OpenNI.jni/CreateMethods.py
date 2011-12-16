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
import re

# ----------------------- MAIN -------------------------
java_header = open("org_OpenNI_NativeMethods.h")
cont = java_header.read()
java_header.close()
result = open("methods.inl", "w")
result.write("static JNINativeMethod methods[] = {\n")

while True:
    match = re.search("Method:\s*(\w*)", cont)
    if match is None:
        break
    method_name = match.group(1)
    
    match = re.search("Signature:\s*([\w\(\)\[;/]*)", cont)
    if match is None:
        break
    signature = match.group(1)
        
    match = re.search("JNIEXPORT.*JNICALL (\w*)", cont)
    if match is None:
        break
    method = match.group(1)

    result.write('\t{ "' + method_name + '", "' + signature + '", (void*)&' + method + ' },\n')
    cont = cont[match.end():];

result.write('};\n');
result.close()
