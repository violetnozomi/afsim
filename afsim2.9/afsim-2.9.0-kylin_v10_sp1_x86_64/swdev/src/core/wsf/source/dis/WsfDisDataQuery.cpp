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

#include "dis/WsfDisDataQuery.hpp"

#include "Dis.hpp"
#include "UtLog.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisUtil.hpp"


//! Constructor
//! Local Origination
//! Generate WsfDisDataQuery to be sent to one or more external simulations.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisDataQuery::WsfDisDataQuery(WsfDisInterface* aInterfacePtr)
   : DisDataQuery()
   , mInterfacePtr(aInterfacePtr)
{
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntityId(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

//! Constructor
//! Remote Origination
//! Generate WsfDisDataQuery while processing an externally received DisDataQuery PDU.
//! @param aPdu          A reference to the incoming Protocol Data Unit (PDU).
//! @param aGenI         A GenI helper reference used to retrieve PDU information from the buffer.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisDataQuery::WsfDisDataQuery(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisDataQuery(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisDataQuery::~WsfDisDataQuery() {}

bool WsfDisDataQuery::TakeAction()
{
   bool tookAction = false;
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      // For now, our only action is to say that we can't...
      tookAction = true;
      ut::log::warning() << "WsfDisDataQuery: Not enabled to act on incoming queries.";
   }
   return tookAction;
}

// virtual
int WsfDisDataQuery::Process()
{
   const int DELETE_PDU = 1;
   bool      actedUpon  = TakeAction();
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      double simTime = WsfDisUtil::GetSimulation(mInterfacePtr).GetSimTime();
      double pduTime = WsfDisUtil::GetTimeFromPdu(mInterfacePtr, *this);

      auto out = ut::log::debug() << "WsfDisDataQuery: PDU received and processed.";
      out.AddNote() << "Sim T = " << simTime;
      out.AddNote() << "PDU T = " << pduTime;
      if (actedUpon)
      {
         out.AddNote() << "Acted upon. No reply required.";
      }
   }
   return DELETE_PDU;
}
