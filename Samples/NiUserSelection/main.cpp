/*****************************************************************************
*                                                                            *
*  OpenNI 1.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#include "SampleManager.h"

/// @defgroup UserSelectionMain User selection main
/// @brief User selection main.cpp functions
/// @ingroup UserSelectionSampleFiles

/// @brief Method which prints the usage and exits the program.
///
/// @param filename The filename of the executable
/// @ingroup UserSelectionMain
void UsageAndExit(char* filename)
{
    printf("\nUsage: %s [-recordingFilename Filename] [-s selectorType] [-ShowLowConfidence]\n\n\n" , filename);
    printf("-RecordingFilename Filename         Use the recording in Filename for input.\n\n");
    printf("-ShowLowConfidence                  Draws limbs with low confidence as dotted/dashed lines (hidden by default)\n\n");
    printf("-s selectorType                     Chooses the selector type to use (i.e. how to select which users to track).\n");
    printf("                -ClosestSelector N: The N closest users are tracked.\n");
    printf("                -SingleWave:        Whichever user waved last is tracked.\n");
    printf("                -MultipleWave N:    Upto N skeleton are tracked. Wave used to select/unselect user for tracking.\n\n");
    printf("   Note:\n");
    printf("        - If the '-s' option is omitted,  ClosestSelector with a value of 1 is used by default.\n");
    printf("        - The '-s' option will be ignored if calibration pose is required. In this case, anyone striking that pose will be tracked.\n\n");

    exit(EXIT_FAILURE);
}



/// @brief The main function of the sample
/// 
/// This is the main function of the sample. Its purpose is to be the starting point for the
/// sample and according to the command line arguments, to decide which sample behavior to use.
/// @ingroup UserSelectionMain
int main(int argc, char** argv)
{    
    SampleManager* pSampleManager=NULL; // will hold the sample manager which initializes and runs the sample
    if(argc>1)
    {
        // check if the argument is asking for help...
        if(xnOSStrCmp(argv[1],"--help")==0 ||
           xnOSStrCmp(argv[1],"-help")==0 ||
           xnOSStrCmp(argv[1],"/help")==0 ||
           xnOSStrCmp(argv[1],"\\help")==0 ||
           xnOSStrCmp(argv[1],"?")==0 ||
           xnOSStrCmp(argv[1],"--?")==0 ||
           xnOSStrCmp(argv[1],"-?")==0 ||
           xnOSStrCmp(argv[1],"/?")==0 ||
           xnOSStrCmp(argv[1],"\\?")==0)
        {
            UsageAndExit(argv[0]);
        }

        // go over the arguments and see if we have the '-s' switch. If so it means we
        // are going to choose the selector type.
        for(int i=1; i<argc-1; i++)
        {
            if(xnOSStrCmp(argv[i],"-s")==0)
            {
                if(xnOSStrCmp(argv[i+1],"ClosestSelector")==0)
                {
                    // we are in the option '-s ClosestSelector N'. 
                    if(argc<=i+2 || strlen(argv[i+2])>2)
                    {
                        UsageAndExit(argv[0]); // either there is no N at all or it is more than 1 digit
                    }
                    XnUInt32 n=argv[i+2][0]-'0'; // translate digit to number
                    pSampleManager=XN_NEW(ClosestSampleManager,n); // choose ClosestSelector with n
                }
                else if(xnOSStrCmp(argv[i+1],"SingleWave")==0)
                {
                    // option is '-s SingleWave'
                    pSampleManager=XN_NEW(SingleWaveSampleManager);
                }
                else if(xnOSStrCmp(argv[i+1],"MultipleWave")==0)
                {
                    // option is '-s SingleWave N'
                    if(argc<=i+2 || strlen(argv[i+2])>2)
                    {
                        UsageAndExit(argv[0]); // either there is no N at all or it is more than 1 digit
                    }
                    XnUInt32 n=argv[i+2][0]-'0'; // translate digit to number
                    pSampleManager=XN_NEW(MultipleWaveSampleManager,n);
                }
                else
                {
                    // an illegal option was used...
                    UsageAndExit(argv[0]);
                }
                break; // we found the switch so we ignore all others
            }
        }
    }
    if(pSampleManager==NULL) // by default we use the closest detector option
    {
        pSampleManager=XN_NEW(ClosestSampleManager,1); // choose the default.
    }

    if(pSampleManager->StartSample(argc,argv)!=XN_STATUS_OK) 
    {
		XN_DELETE(pSampleManager);
        UsageAndExit(argv[0]); // The manager will only return if it fails to initialize...
    }
}
