// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfStringTable.hpp"

WsfStringTable::WsfStringTable()
   : cACCEPTED("ACCEPTED")
   , cREJECTED("REJECTED")
   , cCOMPLETED("COMPLETED")
   , cIN_PROGRESS("IN_PROGRESS")
   , cACKNOWLEDGE_CANCEL("ACKNOWLEDGE_CANCEL")
   , cACKNOWLEDGE_COMPLETE("ACKNOWLEDGE_COMPLETE")
   , cOBJECT_RELEASED("OBJECT_RELEASED")
   , cSUCCESSFUL("SUCCESSFUL")
   , cUNSUCCESSFUL("UNSUCCESSFUL")
   , cWSF_DROP_TRACK_MESSAGE("WSF_DROP_TRACK_MESSAGE")
   , cWSF_TRACK_MESSAGE("WSF_TRACK_MESSAGE")
   , cWSF_ASSOCIATION_MESSAGE("WSF_ASSOCIATION_MESSAGE")
   , cWSF_TASK_ASSIGN_MESSAGE("WSF_TASK_ASSIGN_MESSAGE")
   , cWSF_TASK_CANCEL_MESSAGE("WSF_TASK_CANCEL_MESSAGE")
   , cWSF_TASK_CONTROL_MESSAGE("WSF_TASK_CONTROL_MESSAGE")
   , cWSF_TASK_STATUS_MESSAGE("WSF_TASK_STATUS_MESSAGE")
{
}
