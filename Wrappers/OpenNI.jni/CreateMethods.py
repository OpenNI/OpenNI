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
