# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

Hey, this is what I have used to try to run valgrind on vespa and VTK.

When I run valgrind, the call is made like this:
valgrind --gen-suppressions=all --leak-check=full --num-callers=40 --log-file=out --suppressions=/local/swdev/cme_all/vespatk/valgrind_suppressions/opengl.supp --suppressions=/local/swdev/cme_all/vespatk/valgrind_suppressions/qt.supp --suppressions=/local/swdev/cme_all/vespatk/valgrind_suppressions/osg.supp --suppressions=/local/swdev/cme_all/vespatk/valgrind_suppressions/osgearth.supp --suppressions=/local/swdev/cme_all/vespatk/valgrind_suppressions/misc.supp --error-limit=no --run-libc-freeres=no -v ./vespa

Obvisously you will need to update for your application and path structure.

misc.supp - contains the suppressions I have found I needed, that I haven't been able to categorize
opengl.supp - contains the suppressions I have found that apply to opengl and gpu drivers
osgearth.supp - contains the suppressions I have found that apply to osgearth libraries
osg.supp - contains the suppressions I have found that apply to osg libraries
qt.supp - contains the suppressions I have found that apply to qt libraries

The existence of a suppression does not imply that we are not doing something wrong!  Question everything.

Thanks,
Jon
