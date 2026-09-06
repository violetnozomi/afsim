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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef WSFDISFILTERALLOWANCE_HPP
#define WSFDISFILTERALLOWANCE_HPP

#include "wsf_export.h"

#include <string>

#include "DisEntityType.hpp"
#include "DisPdu.hpp"
#include "DisPduEnums.hpp"
#include "DisTypes.hpp"

//#include "dis/WsfDisEntityState.hpp"
class WsfDisInterface;
//#include "dis/WsfDisInterface.hpp"
//#include "dis/WsfDisPlatform.hpp"

class WSF_EXPORT WsfDisFilterAllowance
{
public:
   enum FilterType
   {
      cENTITYTYPE,
      cENTITYFORCE
   };
   enum TrackedFlag
   {
      cALL,
      cTRACKED
   };

   WsfDisFilterAllowance();
   virtual ~WsfDisFilterAllowance();

   // The default implementation is to filter upon ONLY Entity State Pdus.
   // This permits (upon command) a new type to be filtered.
   void SetPduType(DisEnum8 aNewPduType) { mPduType = aNewPduType; }

   bool PduAllowed(DisPdu& aPdu, WsfDisInterface* aInterfacePtr);

   DisEnum8   mPduType;
   FilterType mFilterType;

   TrackedFlag mTrackedFlag;
   std::string mTrackingSide;
   DisEnum8    mTrackingForceId;

   std::string   mEntityTypeName;
   DisEntityType mEntityType;

   std::string mEntitySide;
   DisEnum8    mEntityForceId;
};

#endif
