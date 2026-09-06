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

#include "dis/WsfDisActionResponseR.hpp"

#include <cassert>

#include "Dis.hpp"
#include "DisActionEnums.hpp"
#include "DisActionRequestR.hpp"
#include "DisClockTime.hpp"
#include "DisControlEnums.hpp"
#include "DisDatumEnums.hpp"
#include "DisDatumSpec.hpp"
#include "DisFixedDatum.hpp"
#include "DisPduEnums.hpp"
#include "DisTime.hpp"
#include "DisTransactionEnums.hpp"
#include "GenI.hpp"
#include "GenIConvertBigEndian.hpp"
#include "GenOConvertBigEndian.hpp"
#include "UtLog.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisActionResponseR::WsfDisActionResponseR(WsfDisInterface* aInterfacePtr)
   : DisActionResponseR()
   , mInterfacePtr(aInterfacePtr)
{
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisActionResponseR::WsfDisActionResponseR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisActionResponseR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisActionResponseR::~WsfDisActionResponseR() = default;

void WsfDisActionResponseR::MakeJoinExerciseResponse(double       aSimTime,
                                                     double       aSimRate,
                                                     double       aSimTimeStep,
                                                     unsigned int aRequestStatus,
                                                     unsigned int aTimeScheme,
                                                     unsigned int aFeedbackTime,
                                                     unsigned int aTimeLatency)
{
   const GenOConvertBigEndian converter;
   unsigned char              buffer[8];

   SetResponseStatus(static_cast<DisEnum32>(aRequestStatus));

   if (aTimeScheme != DisEnum::Control::Scheme::OTHER)
   {
      converter.PutToBuf(buffer, static_cast<DisEnum32>(aTimeScheme));
      GetData().AddFixedDatum(DisFixedDatum(DisEnum::DatumTag::TIME_SCHEME, buffer));
   }

   if (aFeedbackTime != 0)
   {
      converter.PutToBuf(buffer, static_cast<DisEnum32>(aFeedbackTime));
      GetData().AddFixedDatum(DisFixedDatum(DisEnum::DatumTag::FEEDBACK_TIME, buffer));
   }

   if (aFeedbackTime != 0)
   {
      converter.PutToBuf(buffer, static_cast<DisEnum32>(aTimeLatency));
      GetData().AddFixedDatum(DisFixedDatum(DisEnum::DatumTag::TIME_LATENCY, buffer));
   }

   if (aSimRate > 0.0)
   {
      converter.PutToBuf(buffer, static_cast<float>(aSimRate));
      GetData().AddFixedDatum(DisFixedDatum(DisEnum::DatumTag::APPLICATION_RATE, buffer));
   }

   if (aSimTimeStep > 0.0)
   {
      DisClockTime simTimeStep;
      simTimeStep.SetHour(0); // Assume small time steps...
      simTimeStep.SetTimePastHour(DisTime::GetTimeStamp(aSimTimeStep));
      GetData().AddVariableDatum(DisVariableDatum(DisEnum::DatumTag::APPLICATION_TIME_STEP, simTimeStep));
   }
}

/*void WsfDisActionResponseR::MakeTimeAdvanceRequestResponse(int       aRequestorSerialId,
                                    int       aRequestStatus,
                                    int       aTimeScheme,
                                    DisUint32 aFeedbackTime,
                                    DisUint32 aTimeLatency,
                                    double    aSimRate,
                                    double    aSimTimeStep,
                                    double    aSimTime)
{
}*/

/*void WsfDisActionResponseR::MakeResignExerciseResponse(int       aRequestorSerialId,
                                int       aRequestStatus,
                                int       aTimeScheme,
                                DisUint32 aFeedbackTime,
                                DisUint32 aTimeLatency,
                                double    aSimRate,
                                double    aSimTimeStep,
                                double    aSimTime)
{
}*/

bool WsfDisActionResponseR::ProcessResponseTo(DisPdu* aInitiatorPduPtr)
{
   bool                     processed  = false;
   const DisActionRequestR* requestPtr = dynamic_cast<DisActionRequestR*>(aInitiatorPduPtr);
   if (requestPtr != nullptr)
   {
      if (requestPtr->GetActionId() == DisEnum::Action::Request::JOIN_EXERCISE)
      {
         processed = ProcessJoinExerciseResponseTo(requestPtr);
      }
      else if (requestPtr->GetActionId() == DisEnum::Action::Request::TIME_ADVANCE)
      {
         // processed = ProcessTimeAdvanceResponseTo(requestPtr);
         processed = true;
      }
      else
      {
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::error() << "WsfDisActionRequestR: Initializing was not a Join Exercise Request.";
            out.AddNote() << "Received response was ignored.";
         }
      }
   }
   return processed;
}

bool WsfDisActionResponseR::ProcessJoinExerciseResponseTo(const DisActionRequestR* aRequestPtr)
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
            auto out = ut::log::debug() << "WsfDisActionResponseR: Got feedback time but does not use it.";
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
            auto out = ut::log::debug() << "WsfDisActionResponseR: Got request latency but does not use it.";
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
            auto out = ut::log::debug() << "WsfDisActionResponseR: Received an unparsed fixed datum.";
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
            auto out = ut::log::debug() << "WsfDisActionResponseR: Received an unparsed variable datum.";
            out.AddNote() << "ID: " << static_cast<int>(varData.GetId());
         }
      }
   }

   const bool enableTimeAdvance = false;
   WsfDisUtil::GetSimulation(mInterfacePtr)
      .SetTimeParameters(newTimeScheme, newSimTime + newTimeStep, newSimRate, newTimeStep, enableTimeAdvance);

   return numberSupplied == numberParsed;
}
