/****************************************************************************
*                                                                           *
*  OpenNI 1.x Alpha                                                         *
*  Copyright (C) 2011 PrimeSense Ltd.                                       *
*                                                                           *
*  This file is part of OpenNI.                                             *
*                                                                           *
*  OpenNI is free software: you can redistribute it and/or modify           *
*  it under the terms of the GNU Lesser General Public License as published *
*  by the Free Software Foundation, either version 3 of the License, or     *
*  (at your option) any later version.                                      *
*                                                                           *
*  OpenNI is distributed in the hope that it will be useful,                *
*  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the             *
*  GNU Lesser General Public License for more details.                      *
*                                                                           *
*  You should have received a copy of the GNU Lesser General Public License *
*  along with OpenNI. If not, see <http://www.gnu.org/licenses/>.           *
*                                                                           *
****************************************************************************/
#include "SampleManager.h"

/// @brief Method which prints the usage and exits the program.
/// 
/// @param filename 
void UsageAndExit(char* filename)
{
    printf("\nUsage: %s [-recordingFilename Filename] [-s selectorType]\n\n\n" , filename);
    printf("-RecordingFilename Filename         Use the recording in Filename for input.\n\n");
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
int main(int argc, char** argv)
{    
    SampleManager* pSampleManager=NULL;
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
    if(pSampleManager==NULL)
    {
        pSampleManager=XN_NEW(ClosestSampleManager,1); // choose the default.
    }

    if(pSampleManager->StartSample(argc,argv)!=XN_STATUS_OK) 
    {
        UsageAndExit(argv[0]); // The manager will only return if it fails to initialize...
    }
}
