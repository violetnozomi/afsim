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

#ifndef WSFSTRINGTABLE_HPP
#define WSFSTRINGTABLE_HPP

#include "WsfStringId.hpp"

class WsfStringTable
{
   WsfStringTable(const WsfStringTable&);
   WsfStringTable& operator=(const WsfStringTable&);

public:
   WsfStringTable();

   // For WsfTaskManager
   const WsfStringId cACCEPTED;
   const WsfStringId cREJECTED;
   const WsfStringId cCOMPLETED;
   const WsfStringId cIN_PROGRESS;
   const WsfStringId cACKNOWLEDGE_CANCEL;
   const WsfStringId cACKNOWLEDGE_COMPLETE;
   const WsfStringId cOBJECT_RELEASED;
   const WsfStringId cSUCCESSFUL;
   const WsfStringId cUNSUCCESSFUL;

   const WsfStringId cWSF_DROP_TRACK_MESSAGE;
   const WsfStringId cWSF_TRACK_MESSAGE;
   const WsfStringId cWSF_ASSOCIATION_MESSAGE;
   const WsfStringId cWSF_TASK_ASSIGN_MESSAGE;
   const WsfStringId cWSF_TASK_CANCEL_MESSAGE;
   const WsfStringId cWSF_TASK_CONTROL_MESSAGE;
   const WsfStringId cWSF_TASK_STATUS_MESSAGE;
};

#endif
