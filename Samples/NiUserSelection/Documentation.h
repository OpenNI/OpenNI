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
/** @page NIUserSelectionSampleOverview NIUserSelection Sample

<H1>Overview</H1>
<H2>Sample goals</H2>
The goal of this sample is to present the user selection logic and provide a good starting point for
applications needing their own user selection. In addition, this sample shows the use of pose 
detection capability to provide behavior for exiting the program and for user selection.<br><br>

<H2>What is user selection and why use it?</H2>

The logic behind this sample is the fact that there could be many users in the scene and we need a
way to figure out which users to track and which users represent which players.<br>

A @b user is an entity which appears in the scene. In most cases it is a human being (who can 
potentially play the game) but it could also be an inanimate object which the sensor decide is 
interesting enough to be considered.<br>
Multiple users can be part of the scene. Consider the basic case where several friends sit in the
living room and plan to play. All of them could be users but for a single player game, only one can
be a player.<br>
A @b player is someone who controls the input. In this sample, the player would be the user who 
controls the movements of the skeleton, but in general, an application could have many players controlling 
many, different, game elements.<br><br>

In addition to mapping users to players, user selection is important to limit the number of tracked
skeletons. This is because the tracking process can be CPU intensive if many users are tracked.<br><br>

There are many ways to select the users. In this sample, we can see several versions of two basic
schemes: 
- Choosing the closest users to be players
- Choosing a user which is in a certain pose.

A special case of user selection is the case where the user generator supports a skeleton and the
skeleton requires a calibration pose. In this case the user needs to strike a pose in order to be
tracked, this lends itself immediately a specific user selection paradigm: selecting a user who 
strikes the calibration pose. 

<H2>Using the sample</H2>
The sample provides support for four types of user selection:
- The default is to find the closest users (by default 1 but any number up to 9 is supported)
- SingleWave which switches to the last user who waved
- MultipleWave which start tracking anyone who waves (up to N predefined users) and unselects those 
  that wave again
- Using the calibration pose. This option happens implicitly if the skeleton implementation requires 
  a calibration pose, overriding the previous three.

In addition, if the user performs the exit pose (The CrossHandsPose implementation) then the program
will exit after maintaining it for 3 seconds.


<H3>Command line arguments</H3>
When running the program the following command line arguments are supported to configure the way the 
sample runs:

Usage: NiUserSelection.exe [-recordingFilename Filename] [-s selectorType] [-ShowLowConfidence]<br><br>
- @b -RecordingFilename @b Filename: Use the recording in Filename for input.
- @b -ShowLowConfidence: Draws limbs with low confidence as dotted/dashed lines (hidden by default)
- @b -s @b selectorType: Chooses the selector type to use (i.e. how to select which users to track).
                         It supports the following options for @b selectorType:
    - @b -ClosestSelector @b N: The N closest users are tracked.
    - @b -SingleWave: Whichever user waved last is tracked.
    - @b -MultipleWave @b N: Upto N skeleton are tracked. Wave used to select/unselect user for tracking.

@note
- If the '-s' option is omitted,  ClosestSelector with a value of 1 is used by default.
- The '-s' option will be ignored if calibration pose is required. In this case, anyone striking that pose will be tracked.

<H2>Limitations</H2>
To get the most out of this sample the user generator implementation should support the pose detection
capability and that capability should support the "Wave" pose and the "CrossHandsPose". That said,
the sample should work with any implementation but not with all options:
- If the CrossHandsPose pose is not supported by the pose detection capability, exit pose will not work
- If the Wave pose is not supported by the pose detection capability both SingleWave and MultipleWave
  will fail (i.e. only closest will work)
- If the skeleton requires a calibration pose, the user selection enforced will be using the calibration pose.

<h1>Implementation</H1>
The sample into three main modules:
- @ref UserSelectionGraphics
- @ref UserSelectionClasses
- @ref UserSelectionSampleFiles

The basic idea behind these modules is simple:
The @ref UserSelectionGraphics module handles the graphics only and therefore can be mostly ignored by
anyone who wishes to learn only the OpenNI elements.<br>
The @ref UserSelectionSampleFiles module shows how to access various OpenNI elements (although most of
it already appears in other, simpler samples) and the behavior itself is specific to this sample 
(although as usual with samples, it is a good starting place for an application).<br>
The @ref UserSelectionClasses is aimed at being taken as is by an application and extended, as such
it designed to be as generic as possible, aimed as much as possible to simply be included in a project.
*/

/** @defgroup UserSelectionModules User selection sample.
@brief Module which encompasses the entire user selection sample.

This module contains the NIUserSelection sample. For more information see @ref NIUserSelectionSampleOverview
*/



/** @defgroup UserSelectionGraphics User selection graphics
@brief Module to handle graphics for the sample

The main idea behind the graphics in this sample is to hide it as best as possible. The sample uses
OpenGL (or GLES if OpenGL is not available, in which case not all options are supported).<br>
The @ref SceneDrawer class was designed to exclude any OpenNI behavior and therefore this entire 
module can be ignored by anyone who just wishes to learn the OpenNI portions. All OpenNI elements
were abstracted by @ref UserTracker.<br><br>

The @ref SceneDrawer class is implemented as a singleton with a single entry point: 
@ref SceneDrawer::DrawScene. This method runs the sample and never exits (i.e. when it exits the 
entire program exits). @ref SceneDrawer also defines all @b keyboard interaction with the sample 
(as opposed to command line arguments and pose based interaction). The following keyboard options 
are supported:
- @b 'ESC': exits the sample
- @b 'x': This toggles whether to show pixels at all (as opposed to just the skeleton and labels). 
- @b 'b': Assuming pixels are shown (toggled by @b 'x' above), this toggles drawing the background. 
          By default the background and users are both shown. This toggles between the default and 
          showing the users only (no background). 
- @b 's': This toggles whether or not to show the skeleton.
- @b 'i': This toggles whether or not to show labels.
- @b 'l': Assuming labels are shown (toggled by @b 'i' above) this toggled whether to show just the 
          user id or additional information (such as selection state).
- @b 'p': Toggles pausing the sample.
@ingroup UserSelectionModules
*/


/** @defgroup UserSelectionClasses User selection classes
@brief User selection classes and objects

This module is designed to be a plug-in to an application. While not part of OpenNI interface it is
an example implementation of user selection which can be used as is in any application and easily 
extended to provide additional functionality.<br>

The user selection module is composed of two sub modules: 
- @ref UserSelectionSelectors which includes the various logic of how to @b choose a user
- @ref UserSelectionInitializers which includes the various logic of how to start tracking when
  the user was selected.

The main logic would be to choose a selector with the user selection paradigm and choose an initializer
with the desired way to start tracking and then simply activate both in a manner similar to this sample.
@ingroup UserSelectionModules
*/

/** @defgroup UserSelectionSelectors User selection selectors
@brief Module with classes to design on the logic of how to @b choose a user.

This module is responsible for the user selection process, i.e. how to choose a user. It is based
on the @ref UserSelector class which is an interface for all user selectors. The basic implementation
here is based on two types of selection:
- @ref UserSelectionClosestSelectors which is based on the closest user.
- @ref UserSelectionPoseSelectors which is based on a user which strikes a pose.
@ingroup UserSelectionClasses
*/

/** @defgroup UserSelectionInitializers User selection initializers
@brief Module with classes to handle the logic of how to start tracking when the user was selected.

This module is responsible for the process of starting to track the selected user. It provides a base
class (@ref TrackingInitializer) which is used as an interface and a single, default implementation
(@ref DefaultTrackingInitializer) which uses the standard way to start tracking (by requesting 
calibration and when completed successfully start the tracking).

@ingroup UserSelectionClasses
*/

/** @defgroup UserSelectionClosestSelectors User selection Closest selectors
@brief Implementation of closest user selection.

This module contains an implementation for a user selector which chooses the closest users first.
@ingroup UserSelectionSelectors
*/


/** @defgroup UserSelectionPoseSelectors User selection Pose selectors
@brief Implementation of pose based user selection.

This module contains an implementation for user selectors which chooses users based on the pose
the user strikes.
@ingroup UserSelectionSelectors
*/

/** @defgroup UserSelectionSampleFiles User selection sample files
@brief The sample base files

This module contains the base objects provided by the sample. These include:
- @ref UserTracker : This is the base class that abstracts OpenNI to @ref SceneDrawer and interacts
                    with the @ref UserSelectionClasses.
- @ref UserSelectionSampleManagers :  These are various managers responsible for doing the proper 
                                    initialization and running of the sample
- @ref ExitPoseDetector : An object responsible for figuring out when a user strikes the exit pose.
- main.cpp: Contains the @ref main method of the sample.
@ingroup UserSelectionModules
*/

/** @defgroup UserSelectionSampleManagers User selection sample managers
@brief Contains managers responsible for doing the proper initialization and running of the sample
@ingroup UserSelectionSampleFiles
*/
