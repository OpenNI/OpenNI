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
import re

# ----------------------- MAIN -------------------------
java_header = open("org_openni_NativeMethods.h")
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
