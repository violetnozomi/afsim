# ****************************************************************************
# CUI
#
# The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
#
# The use, dissemination or disclosure of data in this file is subject to
# limitation or restriction. See accompanying README and LICENSE for details.
# ****************************************************************************
import sys
import os

srcExt = '.cpp,.cxx,.hpp,.hxx,.h'.split(',')

hdr = '''// ****************************************************************************
// Copyright 2003-2013 The Boeing Company
//
// Analytic Framework for Network Enabled Systems (AFNES) software is provided
// 'As Is', with no warranties, either express or implied, and Boeing will not
// be liable for any damages arising in any way from the use of the AFNES
// software.
//
// WARNING: EXPORT CONTROLLED This document contains technical data whose
// export is restricted by the Arms Export Control Act (Title 22, U.S.C.
// Sec 2751 et seq) or Export Administration Act of 1979, as amended, Title 50
// U.S.C.; App. 2401, et seq.  Violators of these export laws are subject to
// severe criminal penalties. (DoD Directive 5230.25)
// ****************************************************************************'''.splitlines()

# This list includes the test files from the dis, gneio, and util libraries
# These files are not delivered
ignored_files = '''diff_match_patch.cpp diff_match_patch.h
                   Test_DisUnderwaterAcoustic.cpp
                   Test_DisUnderwaterAcoustic.hpp
                   Test_DisDetonation.cpp
                   Test_DisResupplyReceived.cpp
                   Test_DisTransmitter.hpp
                   Test_DisRecordQueryR.cpp
                   Test_DisRepairResponse.hpp
                   Test_DisResupplyOffer.hpp
                   TestFactory.hpp
                   Test_DisCommentR.cpp
                   Test_DisOther.cpp
                   Test_DisSetData.cpp
                   Test_DisRepairComplete.cpp
                   Test_DisIff.hpp
                   Test_DisActionRequest.cpp
                   Test_DisCreateEntityR.cpp
                   Test_DisResupplyReceived.hpp
                   Test_DisTransferControl.cpp
                   Test_DisRemoveEntityR.hpp
                   Test_DisCreateEntity.hpp
                   Test_DisTransferControl.hpp
                   Test_DisIffAtcNavaids.hpp
                   Test_DisEmission.cpp
                   Test_DisDetonation.hpp
                   Test_DisActionRequest.hpp
                   Test_DisSetDataR.hpp
                   Test_DisEventReportR.cpp
                   Test_DisSignal.hpp
                   Test_DisServiceRequest.hpp
                   Test_DisDirectedEnergyFire.cpp
                   Test_DisRecordR.cpp
                   Test_DisDataQueryR.hpp
                   Test_DisFire.cpp
                   Test_DisDataQuery.hpp
                   Test_DisSignal.cpp
                   Test_DisRemoveEntityR.cpp
                   Test_DisDataR.cpp
                   Test_DisIff.cpp
                   Test_DisDataQueryR.cpp
                   Test_DisStopFreezeR.hpp
                   Test_DisStopFreeze.cpp
                   Test_DisStartResumeR.cpp
                   Test_DisEventReport.hpp
                   Test_DisRemoveEntity.cpp
                   Test_DisActionResponse.cpp
                   Test_DisRepairResponse.cpp
                   Test_DisSetDataR.cpp
                   Test_DisAcknowledgeR.cpp
                   Test_DisActionRequestR.hpp
                   Test_DisReceiver.cpp
                   Test_main.cpp
                   Test_DisEntityDamageStatus.cpp
                   Test_DisCreateEntityR.hpp
                   Test_DisDesignator.hpp
                   Test_DisActionResponse.hpp
                   Test_DisAcknowledge.cpp
                   Test_DisRemoveEntity.hpp
                   Test_DisServiceRequest.cpp
                   Test_DisComment.cpp
                   Test_DisTransmitter.cpp
                   Test_DisOther.hpp
                   Test_DisResupplyCancel.hpp
                   Test_DisActionRequestR.cpp
                   Test_DisStopFreeze.hpp
                   Test_DisDesignator.cpp
                   Test_DisAttribute.hpp
                   Test_DisReceiver.hpp
                   Test_DisSetRecordR.cpp
                   Test_DisDirectedEnergyFire.hpp
                   Test_DisStartResume.cpp
                   Test_DisIffAtcNavaids.cpp
                   Test_DisAcknowledge.hpp
                   Test_DisEmission.hpp
                   Test_DisResupplyCancel.cpp
                   Test_DisCreateEntity.cpp
                   Test_DisAcknowledgeR.hpp
                   Test_DisResupplyOffer.cpp
                   Test_DisEventReport.cpp
                   Test_DisStartResume.hpp
                   Test_DisComment.hpp
                   Test_DisStopFreezeR.cpp
                   Test_DisActionResponseR.cpp
                   Test_DisStartResumeR.hpp
                   Test_DisRecordR.hpp
                   Test_classes.hpp
                   Test_DisDataQuery.cpp
                   DisTestHelper.hpp
                   Test_DisDataR.hpp
                   Test_DisSetData.hpp
                   Test_DisRepairComplete.hpp
                   Test_DisSpotted.hpp
                   Test_DisEntityState.cpp
                   Test_DisAttribute.cpp
                   Test_DisCommentR.hpp
                   Test_DisEntityState.hpp
                   Test_DisData.cpp
                   Test_DisFire.hpp
                   Test_DisSetRecordR.hpp
                   Test_DisActionResponseR.hpp
                   Test_DisSpotted.cpp
                   Test_DisEntityDamageStatus.hpp
                   Test_DisData.hpp
                   Test_DisEventReportR.hpp
                   Test_DisRecordQueryR.hpp
                   graph.cpp
                   Exec.cpp
                   CarChevyClientMsg.hpp
                   CarChevyClientMsg.cpp
                   CarClientMsgFactory.hpp
                   CarFordClientMsg.hpp
                   CarFordClientMsg.cpp
                   CarClientMsgFactory.cpp
                   Car.hpp
                   Car.cpp
                   CarMsgFactory.hpp
                   CarMsgFactory.cpp
                   CarFord.hpp
                   CarFord.cpp
                   CarChevy.hpp
                   CarChevy.cpp
                   CarServer.cpp
                   CarServer.h
                   CarServerCC.cpp
                   CarServerCC.h                   
		   doxygen_main_page.h
                '''.split()

def checkFile(path):
   # header is less than 1000 chars
   # splitlines() handles different endline types
   txt = [s.strip() for s in open(path,'r').read(1000).splitlines()]
   if not (len(txt) >= len(hdr) and hdr == txt[:len(hdr)]):
      print 'Bad header in ', path
      return False
   return True

foundBadHeader = False
for root, dirs, files in os.walk('.'):
   for f in files:
      for ext in srcExt:
         if f.endswith(ext) and f not in ignored_files:
            if not checkFile(os.path.join(root, f)):
               foundBadHeader = True

if foundBadHeader: 
   sys.exit(1)
