# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************

# example_graph will read command chains from a provided AER file and generate
# a PDF of the entire scenario's commandchain network.  This demonstrates using
# pymystic, using additional modules, and pulling data out of specific message types
#
# It is run with python and a command line argument to an AER file:
# > python example_graph path/myfile.aer
#
# IMPORTANT: This example requires both GraphViz and the graphviz Python module
#            Ensure that the path to GraphViz binaries is present in the PATH environment variable
#            Ensure that graphviz is present in the lib/site-packages of the Python installation

import sys
import pymystic
from graphviz import Digraph

with pymystic.Reader(sys.argv[1]) as r:
   dot = Digraph('command chains')

   for a in r:
      if (a['msgtype'] == 'MsgPlatformInfo'):
         dot.node(a['name']);
         for c in a['commandChains']:
            dot.edge(c['commander'], a['name'])

dot.unflatten(10, True, 10).render('graph.gv', view=True)
