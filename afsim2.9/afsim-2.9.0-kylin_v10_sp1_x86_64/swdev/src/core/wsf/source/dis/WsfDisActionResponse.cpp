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

#include "dis/WsfDisActionResponse.hpp"

#include <cassert>

#include "Dis.hpp"
#include "DisActionEnums.hpp"
#include "DisClockTime.hpp"
#include "DisControlEnums.hpp"
#include "DisDatumEnums.hpp"
#include "DisDatumSpec.hpp"
#include "DisFixedDatum.hpp"
#include "DisTime.hpp"
#include "GenI.hpp"
#include "GenIConvertBigEndian.hpp"
#include "GenOConvertBigEndian.hpp"
#include "UtLog.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisActionRequest.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisActionResponse::WsfDisActionResponse(WsfDisInterface* aInterfacePtr)
   : DisActionResponse()
   , mInterfacePtr(aInterfacePtr)
{
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisActionResponse::WsfDisActionResponse(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisActionResponse(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisActionResponse::~WsfDisActionResponse() {}

void WsfDisActionResponse::MakeJoinExerciseResponse(const DisEntityId& aRequestorDisId,
                                                    int                aRequestorSerialId,
                                                    int                aRequestStatus,
                                                    int                aTimeScheme,
                                                    DisUint32          aFeedbackTime,
                                                    DisUint32          aTimeLatency,
                                                    double             aSimTime,
                                                    double             aSimRate,
                                                    double             aSimTimeStep)
{
   const GenOConvertBigEndian converter;
   unsigned char              buffer[4];

   SetReceivingEntity(aRequestorDisId);
   SetRequestId(aRequestorSerialId);
   SetRequestStatus(aRequestStatus);

   // These two float values (if non-zero) are known immediately.  Some of the other
   // float values are known only after ALL the requests to join have come in.

   converter.PutToBuf(buffer, static_cast<float>(aFeedbackTime));
   GetData().AddFixedDatum(DisFixedDatum(DisEnum::DatumTag::FEEDBACK_TIME, buffer));

   if (aTimeLatency != 0.0)
   {
      converter.PutToBuf(buffer, static_cast<float>(aTimeLatency));
      GetData().AddFixedDatum(DisFixedDatum(DisEnum::DatumTag::TIME_LATENCY, buffer));
   }
}

bool WsfDisActionResponse::ProcessResponseTo(DisPdu* aInitiatorPduPtr)
{
   bool                    processed  = false;
   const DisActionRequest* requestPtr = dynamic_cast<DisActionRequest*>(aInitiatorPduPtr);
   if (requestPtr != nullptr)
   {
      if (requestPtr->GetActionId() == DisEnum::Action::Request::JOIN_EXERCISE)
      {
         processed = ProcessJoinExerciseResponseTo(requestPtr);
      }
      else if (requestPtr->GetActionId() == DisEnum::Action::Request::TIME_ADVANCE)
      {
         processed = true;
      }
      else
      {
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::error() << "WsfDisActionRequest: Initializing was not a Join Exercise Request.";
            out.AddNote() << "Received response was ignored.";
         }
      }
   }
   return processed;
}

bool WsfDisActionResponse::ProcessJoinExerciseResponseTo(const DisActionRequest* aRequestPtr)
{
   unsigned int numberSupplied = 0;
   unsigned int numberParsed   = 0;

   const GenIConvertBigEndian converter;
   unsigned char              buffer[4];
   typedef DisUint32          DisTimeStamp;

   // a) Application rate shall be represented by a 32-bit floating point number.
   // b) Application timestamp shall be represented by a timestamp (see 6.2.88).
   // c) Feedback time shall be represented by a timestamp (see 6.2.88).
   // d) Simulation rate shall be represented by a 32-bit floating point number.
   // e) Simulation time shall be represented by a Clock Time record (see 6.2.16).
   // f) Simulation timestep shall be represented by a Clock Time record (see 6.2.16).
   // g) Time interval shall be represented by a timestamp (see 6.2.88).
   // h) Time latency shall be represented by a timestamp (see 6.2.88).
   // i) Time scheme shall be represented by a 32-bit unsigned integer (see Section 7 of SISO-REF-010).

   // First, since we got a response from somebody,
   // let's assume they are now our Simulation Manager
   // mInterfacePtr->SetSimulationManager(GetOriginatingEntity());

   double newSimTime    = -1.0;
   double newSimRate    = 0.0;
   double newTimeStep   = 0.0;
   int    newTimeScheme = 0;

   unsigned int iDatum;
   for (iDatum = 0; iDatum != GetData().GetNumFixedDatums(); ++iDatum)
   {
      ++numberSupplied;

      const std::vector<DisFixedDatum>& fixedVec  = GetData().GetFixedDatums();
      const DisFixedDatum&              fixedData = fixedVec[iDatum];
      fixedData.GetData(buffer);

      if (fixedData.GetId() == DisEnum::DatumTag::TIME_SCHEME)
      {
         converter.GetFromBuf(buffer, newTimeScheme);
         ++numberParsed;
      }
      else if (fixedData.GetId() == DisEnum::DatumTag::FEEDBACK_TIME)
      {
         DisTimeStamp feedbackTime;
         converter.GetFromBuf(buffer, feedbackTime);
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::debug() << "WsfDisActionResponse: Got feedback time but does not use it.";
            out.AddNote() << "Feedback Time: " << feedbackTime; // units?
         }
         ++numberParsed;
      }
      else if (fixedData.GetId() == DisEnum::DatumTag::TIME_LATENCY)
      {
         DisTimeStamp requestLatency;
         converter.GetFromBuf(buffer, requestLatency);

         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::debug() << "WsfDisActionResponse: Got request latency but does not use it.";
            out.AddNote() << "Request Latency: " << requestLatency; // units?
         }

         // DisTimeStamp requestTimeStamp = aRequestPtr->GetTimestamp();
         // DisUint32 currentTime = DisTime::GetAbsoluteTimestamp();
         // DisUint32 totalLatency = currentTime - requestTimeStamp;

         // double elapsedTimeRequestSeconds = DisTime::GetAbsoluteTime(requestLatency);
         // double totalElapsedTimeSeconds = DisTime::GetAbsoluteTime(totalLatency);

         ++numberParsed;
      }
      else if ((fixedData.GetId() == DisEnum::DatumTag::SIMULATION_RATE) ||
               (fixedData.GetId() == DisEnum::DatumTag::APPLICATION_RATE))
      {
         // I am making the assumption that Application Rate and Simulation Rate
         // are the same value, but from the SA versus SM perspective.
         // Will treat them as the same quantity.  (?)
         float floatValue;
         converter.GetFromBuf(buffer, floatValue);
         newSimRate = static_cast<double>(floatValue);
         ++numberParsed;
      }
      else
      {
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::debug() << "WsfDisActionResponse: Received an unparsed fixed datum.";
            out.AddNote() << "ID: " << static_cast<int>(fixedData.GetId());
         }
      }
   }

   for (iDatum = 0; iDatum != GetData().GetNumVariableDatums(); ++iDatum)
   {
      ++numberSupplied;

      const std::vector<DisVariableDatum>& varVec  = GetData().GetVariableDatums();
      const DisVariableDatum&              varData = varVec[iDatum];

      unsigned int         bufferLength;
      const unsigned char* buffer2;
      varData.GetData(buffer2, bufferLength);
      DisClockTime disTime;
      unsigned int absolute;

      if ((varData.GetId() == DisEnum::DatumTag::SIMULATION_TIME) ||
          (varData.GetId() == DisEnum::DatumTag::APPLICATION_TIME))
      {
         varData.GetClockTimeFromBuffer(disTime);
         newSimTime = 3600 * disTime.GetHour() + DisTime::GetSecsAfterHour(disTime.GetTimePastHour(), absolute);
         ++numberParsed;
      }
      else if ((varData.GetId() == DisEnum::DatumTag::SIMULATION_TIME_STEP) ||
               (varData.GetId() == DisEnum::DatumTag::APPLICATION_TIME_STEP))
      {
         varData.GetClockTimeFromBuffer(disTime);
         newTimeStep = 3600 * disTime.GetHour() + DisTime::GetSecsAfterHour(disTime.GetTimePastHour(), absolute);
         ++numberParsed;
      }
      else
      {
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::debug() << "WsfDisActionResponse: Received an unparsed variable datum.";
            out.AddNote() << "ID: " << static_cast<int>(varData.GetId());
         }
      }
   }

   const bool enableTimeAdvance = false;
   WsfDisUtil::GetSimulation(mInterfacePtr)
      .SetTimeParameters(newTimeScheme, newSimTime + newTimeStep, newSimRate, newTimeStep, enableTimeAdvance);

   return numberSupplied == numberParsed;
}
