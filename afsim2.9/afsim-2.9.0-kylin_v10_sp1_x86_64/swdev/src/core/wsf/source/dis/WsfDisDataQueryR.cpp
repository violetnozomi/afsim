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

#include "dis/WsfDisDataQueryR.hpp"

#include "Dis.hpp"
#include "DisActionEnums.hpp"
#include "DisControlEnums.hpp"
#include "DisDatumEnums.hpp"
#include "DisFixedDatum.hpp"
#include "DisPduEnums.hpp"
#include "DisTime.hpp"
#include "GenIConvertBigEndian.hpp"
#include "GenOConvertBigEndian.hpp"
#include "UtLog.hpp"
#include "WsfClockSource.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisDataR.hpp"
#include "dis/WsfDisUtil.hpp"

//! Constructor
//! Local Origination
//! Generate WsfDisDataQueryR to be sent to one or more external simulations.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisDataQueryR::WsfDisDataQueryR(WsfDisInterface* aInterfacePtr)
   : DisDataQueryR()
   , mInterfacePtr(aInterfacePtr)
{
   SetRequiredReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntityId(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

//! Constructor
//! Remote Origination
//! Generate WsfDisDataQueryR while processing an externally received DisDataQueryR PDU.
//! @param aPdu          A reference to the incoming Protocol Data Unit (PDU).
//! @param aGenI         A GenI helper reference used to retrieve PDU information from the buffer.
//! @param aInterfacePtr A pointer to the DIS interface.
WsfDisDataQueryR::WsfDisDataQueryR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisDataQueryR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisDataQueryR::~WsfDisDataQueryR() {}

void WsfDisDataQueryR::MakeTimeAdvanceReportRequest(double aTimeInterval)
{
   GetDatumSpec().AddFixedDatum(DisEnum::DatumTag::APPLICATION_TIME_STEP);
   GetDatumSpec().AddFixedDatum(DisEnum::DatumTag::APPLICATION_RATE);
   if (aTimeInterval != 0.0)
   {
      SetTimeInterval(DisTime::GetTimeStamp(aTimeInterval));
   }
}

DisPdu* WsfDisDataQueryR::ProduceResponse(bool& aIsDone)
{
   WsfDisDataR* responsePtr = new WsfDisDataR(mInterfacePtr);

   responsePtr->SetRequestId(GetRequestId());
   responsePtr->SetReceivingEntity(GetOriginatingEntity());

   // Per the DIS Standard, the response shall NOT require acknowledgment.
   responsePtr->SetReliabilityService(DisEnum::Pdu::Reliability::Unacknowledged);

   unsigned int totalQueries   = 0;
   unsigned int totalResponses = 0;

   // According to DIS Standard 1278.1-200x Draft 13, a Sim Manager will
   // place the following info into a request for a Sim Application to
   // respond with time advance reports:

   GenOConvertBigEndian   converter;
   unsigned char          buffer[4];
   std::vector<DisEnum32> fixedDatums    = GetDatumSpec().GetFixedDatums();
   std::vector<DisEnum32> variableDatums = GetDatumSpec().GetVariableDatums();

   unsigned int i;
   for (i = 0; i != GetDatumSpec().GetNumFixedRecords(); ++i)
   {
      ++totalQueries;

      DisEnum32 id = fixedDatums[i];

      if (id == DisEnum::DatumTag::APPLICATION_TIME_STEP)
      {
         DisFloat32 floatValue = static_cast<DisFloat32>(WsfDisUtil::GetSimulation(mInterfacePtr).GetTimestep());
         converter.PutToBuf(buffer, floatValue);
         responsePtr->GetData().AddFixedDatum(DisFixedDatum(id, buffer));
         ++totalResponses;
      }
      else if (id == DisEnum::DatumTag::APPLICATION_RATE)
      {
         DisFloat32 floatValue = static_cast<DisFloat32>(WsfDisUtil::GetSimulation(mInterfacePtr).GetClockRate());
         converter.PutToBuf(buffer, floatValue);
         responsePtr->GetData().AddFixedDatum(DisFixedDatum(id, buffer));
         ++totalResponses;
      }
   }

   for (i = 0; i != GetDatumSpec().GetNumVariableRecords(); ++i)
   {
      ++totalQueries;

      DisEnum32 id = variableDatums[i];

      if (id == DisEnum::DatumTag::EXERCISE_NAME)
      {
         // assert(false);
      }
   }

   if (totalQueries == 0)
   {
      if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         ut::log::warning() << "WsfDisDataQueryR: Received a null query (no datums requested).";
      }
   }
   else if ((totalQueries != totalResponses) && (WsfDisUtil::DebugEnabled(mInterfacePtr)))
   {
      ut::log::warning() << "WsfDisDataQueryR: Could not respond to all queries.";
   }

   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::debug() << "WsfDisDataQueryR: Will not yet queue time-interval data reports.";
   }

   aIsDone = (totalQueries == totalResponses);

   return responsePtr;
}

void WsfDisDataQueryR::TimedOut()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisDataQueryR: Unhandled virtual TimedOut() call.";
   }
}

// virtual
int WsfDisDataQueryR::Process()
{
   bool repliedTo = false;
   bool actedUpon = false;

   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      double simTime2 = WsfDisUtil::GetSimTime(mInterfacePtr);
      double pduTime  = WsfDisUtil::GetTimeFromPdu(mInterfacePtr, *this);

      auto out = ut::log::debug() << "WsfDisDataQueryR: PDU received and processed.";
      out.AddNote() << "Sim T = " << simTime2;
      out.AddNote() << "PDU T = " << pduTime;

      // At this point, there is no way for actedUpon or repliedTo to ever be true.
      // This is probably a bug, so all of the notes are being provided for if/when this gets fixed.
      if (actedUpon && repliedTo)
      {
         out.AddNote() << "Acted upon. Reply sent.";
      }
      else if (actedUpon && !repliedTo)
      {
         out.AddNote() << "Acted upon. No reply sent.";
      }
      else if (repliedTo)
      {
         out.AddNote() << "Not acted upon. Reply sent.";
      }
      else
      {
         out.AddNote() << "Not acted upon. No reply sent.";
      }
   }

   return DisPdu::Process();
}
