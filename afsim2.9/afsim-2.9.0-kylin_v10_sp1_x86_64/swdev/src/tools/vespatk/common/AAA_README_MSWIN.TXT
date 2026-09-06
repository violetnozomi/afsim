# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
                            Common Modeling Environment (CME)
                           Installation And Setup Instructions
                              For Microsoft Windows Systems

                                   15 August 2014

-------------------------------------------------------------------------------

Installation
------------

CME is distributed as an archived zip file.  You may install it
one by copying the file one directory above the installation target,
right clicking and selecting Winzip->Extract to here.

New versions may be installed over existing older versions, and the older
versions will be maintained.  As of version 6.3, applications are executed from
the bin.vX.Y.Z directory, previously they were executed from the win directory.

-------------------------------------------------------------------------------

Uninstalling
------------

CME may be uninstalled by simply removing the 'CME' directory tree from
whatever it was installed.  It does not create any registry entries or anything
else that needs to be deleted.

NOTE: Make sure you move any data files that you have created in the
      directory tree to some other location BEFORE deleting the tree!

-------------------------------------------------------------------------------

Running The Programs From The MS-DOS Prompt
-------------------------------------------

To invoke a program from the MS-DOS prompt, open a MS-DOS window and enter:

     X:\install_dir\cme\bin.vx.y.z\YYYYY.exe <optional-arguments>

Where 'X:\install_dir' is the directory where you installed CME and 'YYYYY.exe' is the application
you want to invoke.  For instance, if you loaded CME on the D: drive and want
to invoke VESPA:

     d:\cme\bin.v6.3.3\vespa

-------------------------------------------------------------------------------

Defining Shortcuts
----------------------------------------------------

1. Open a Windows Explorer window and drill down to the \cme\win
   directory on the disk where you installed CME.

2. Press and hold down the right mouse button and ALT key on the desired executable
   and drag it to the desktop (or where ever else you desire).  Release the right
   mouse button

3. Rename the created shortcut to something like MyVespa (or whatever you want)

4. Right click on the shortcut and select 'Properties'.

5. Select the 'Shortcut' tab (if not already selected).

   In the 'Target:' field, add any command line arguments you desire.

   In the 'Start In:' field, enter the default directory you want to be in
   when the program starts.

   In the 'Shortcut Key:' field, assign a shortcut key if desired.

6. Click the 'OK' button to dismiss the dialog box.


