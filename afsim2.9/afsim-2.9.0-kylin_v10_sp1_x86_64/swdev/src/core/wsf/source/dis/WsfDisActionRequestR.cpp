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

#include "dis/WsfDisActionRequestR.hpp"

#include "Dis.hpp"
#include "DisActionEnums.hpp"
#include "DisActionResponseR.hpp"
#include "DisClockTime.hpp"
#include "DisControlEnums.hpp"
#include "DisDatumEnums.hpp"
#include "DisIO.hpp"
#include "DisPduEnums.hpp"
#include "DisTime.hpp"
#include "DisTypes.hpp"
#include "GenIConvertBigEndian.hpp"
#include "GenOConvertBigEndian.hpp"
#include "UtLog.hpp"
#include "WsfSimulation.hpp"
#include "dis/WsfDisActionResponseR.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisActionRequestR::WsfDisActionRequestR(WsfDisInterface* aInterfacePtr)
   : DisActionRequestR()
   , mInterfacePtr(aInterfacePtr)
{
   SetReliabilityService(WsfDisUtil::GetReliabilityService(mInterfacePtr));
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisActionRequestR::WsfDisActionRequestR(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisActionRequestR(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisActionRequestR::~WsfDisActionRequestR() {}

DisPdu* WsfDisActionRequestR::ProduceResponse(bool aComplete)
{
   WsfDisActionResponseR* responsePtr = new WsfDisActionResponseR(mInterfacePtr);

   responsePtr->SetReceivingEntity(GetOriginatingEntity());
   responsePtr->SetRequestId(GetRequestId());

   DisEnum32 requestStatus = DisEnum::Action::RequestStatus::PENDING;

   if (GetActionId() == DisEnum::Action::Request::JOIN_EXERCISE)
   {
      // Note:  WSF is not yet implemented as a Simulation Manager, but this is
      // a placeholder to fill in a response to test the functionality.
      DisUint32 current    = DisTime::GetAbsoluteTimestamp();
      DisUint32 stamp      = GetTimestamp();
      DisUint32 latency    = current - stamp;
      double    simTime    = WsfDisUtil::GetSimTime(mInterfacePtr);
      double    maxSimRate = 0.0; // mInterfacePtr->GetMaxSimulationRate();

      double       simTimeStep = 0.0; // mInterfacePtr->GetMaxSimulationTimeStep();
      DisClockTime timeStep;
      timeStep.SetHour(0); // assume small time steps....
      timeStep.SetTimePastHour(DisTime::GetTimeStamp(simTimeStep));

      int timeScheme = DisEnum::Control::Scheme::SCALED_AND_STEPPED;

      if (aComplete)
      {
         requestStatus = DisEnum::Action::RequestStatus::COMPLETE;
      }
      responsePtr->MakeJoinExerciseResponse(simTime + 0.001,
                                            maxSimRate,
                                            simTimeStep,
                                            requestStatus,
                                            timeScheme,
                                            GetTimestamp(),
                                            latency);
   }
   else if (GetActionId() == DisEnum::Action::Request::TIME_ADVANCE)
   {
      // This is a placeholder just to fill in a response to test the functionality.
      if (aComplete)
      {
         requestStatus = DisEnum::Action::RequestStatus::COMPLETE;
      }
      if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         ut::log::debug() << "WsfDisActionRequestR: Time Advance Request is not given an adequate response yet.";
      }
   }
   else if (GetActionId() == DisEnum::Action::Request::RESIGN_EXERCISE)
   {
      // This is a placeholder just to fill in a response to test the functionality.
      if (aComplete)
      {
         requestStatus = DisEnum::Action::RequestStatus::COMPLETE;
      }
      if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         ut::log::warning() << "WsfDisActionRequestR: Resign Exercise Request is not given an adequate response yet.";
      }
   }
   else
   {
      if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         auto out =
            ut::log::warning() << "WsfDisActionRequestR: Action Request received that cannot be interpreted by WSF.";
         out.AddNote() << "An acknowledge response was returned, but the request was ignored.";
      }
   }

   responsePtr->SetResponseStatus(requestStatus);

   return responsePtr;
}

// virtual
void WsfDisActionRequestR::TimedOut()
{
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisActionRequestR: Unhandled virtual TimedOut() call.";
   }
}

void WsfDisActionRequestR::MakeJoinExerciseRequest(const DisEntityId& aSimulationManager,
                                                   double             aAppJoinTime,
                                                   double             aMaxAppRate,
                                                   double             aMaxAppTimeStep,
                                                   const std::string& aExerciseToJoinId)
{
   SetActionId(DisEnum::Action::Request::JOIN_EXERCISE);

   SetReceivingEntity(aSimulationManager);

   if (aMaxAppRate > 0.0)
   {
      GenOConvertBigEndian converter;
      unsigned char        buffer[4];

      float appRate = static_cast<float>(aMaxAppRate);
      converter.PutToBuf(buffer, appRate);
      GetData().AddFixedDatum(DisFixedDatum(DisEnum::DatumTag::APPLICATION_RATE, buffer));
   }

   if (aMaxAppTimeStep > 0.0)
   {
      DisClockTime disTime;
      disTime.SetHour(0);
      disTime.SetTimePastHour(DisTime::GetTimeStamp(aMaxAppTimeStep));
      GetData().AddVariableDatum(DisVariableDatum(DisEnum::DatumTag::APPLICATION_TIME_STEP, disTime));
   }

   // The absence of this parameter means "join immediately"
   if (aAppJoinTime > 0.0)
   {
      DisClockTime disTime;
      disTime.SetHour(0);
      disTime.SetTimePastHour(DisTime::GetTimeStamp(aAppJoinTime));
      GetData().AddVariableDatum(DisVariableDatum(DisEnum::DatumTag::APPLICATION_TIME, disTime));
   }

   if (!aExerciseToJoinId.empty())
   {
      DisVariableDatum nameVarDatum(DisVariableDatum(DisEnum::DatumTag::EXERCISE_NAME,
                                                     (unsigned char*)(aExerciseToJoinId.c_str()),
                                                     static_cast<DisUint32>(aExerciseToJoinId.length() * 8)));
      GetData().AddVariableDatum(nameVarDatum);
   }
}

void WsfDisActionRequestR::MakeTimeAdvanceRequest(const DisEntityId& aSimulationManager,
                                                  double             aSimTime,
                                                  double             aAppRate,
                                                  double             aAppTimeStep)
{
   SetActionId(DisEnum::Action::Request::TIME_ADVANCE);

   SetReceivingEntity(aSimulationManager);

   if (aSimTime != 0.0)
   {
      DisClockTime disTime;
      WsfDisUtil::SimTimeToDisClockTime(aSimTime, disTime);
      GetData().AddVariableDatum(DisVariableDatum(DisEnum::DatumTag::APPLICATION_TIME, disTime));
   }

   if (aAppTimeStep != 0.0)
   {
      DisClockTime disTime;
      WsfDisUtil::SimTimeToDisClockTime(aAppTimeStep, disTime);
      GetData().AddVariableDatum(DisVariableDatum(DisEnum::DatumTag::APPLICATION_TIME_STEP, disTime));
   }

   if (aAppRate != 0.0)
   {
      GenOConvertBigEndian converter;
      unsigned char        buffer[4];

      float appRate = static_cast<float>(aAppRate);
      converter.PutToBuf(buffer, appRate);
      GetData().AddFixedDatum(DisFixedDatum(DisEnum::DatumTag::APPLICATION_RATE, buffer));
   }
}

void WsfDisActionRequestR::MakeResignExerciseRequest(double aSimTime)
{
   SetActionId(DisEnum::Action::Request::RESIGN_EXERCISE);
   DisClockTime disTime;
   // WsfDisInterface::SimTimeToDisClockTime(aSimTime, disTime);
   GetData().AddVariableDatum(DisVariableDatum(DisEnum::DatumTag::APPLICATION_TIME, disTime));
}

bool WsfDisActionRequestR::ProcessJoinExercise()
{
   // Since WSF is (not yet) implemented as a Simulation Manager,
   // do not take any action on this PDU, other than responding to it affirmatively.
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      auto out = ut::log::debug() << "WsfDisActionRequestR: WSF does not implement a SM function.";
      out.AddNote() << "ProcessJoinExercise was received, but no action will be taken.";
      out.AddNote() << "Request will be acknowledged as if it were functional.";
   }
   return true;
}

bool WsfDisActionRequestR::ProcessTimeAdvance(double aSimTime, double aSimRate, double aSimTimeStep)
{
   // Our Simulation Manager has just told us to advance to a new SimTime.
   // Take his time stamp, adjust for a clock skew and/or latency, and then
   // further adjust for our ZERO reference time.  Set the resulting new
   // aSimTime as our Max Clock time, and start the clock.
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      ut::log::warning() << "WsfDisActionRequestR: ProcessTimeAdvanceRequest ignored.";
   }
   return true;
}

bool WsfDisActionRequestR::ProcessResignExercise()
{
   // Since WSF is (not yet) implemented as a Simulation Manager,
   // do not take any action on this PDU, other than responding to it affirmatively.
   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      auto out = ut::log::debug() << "WsfDisActionRequestR: WSF does not implement a SM function.";
      out.AddNote() << "ProcessResignExercise ignored.";
   }
   return true;
}

bool WsfDisActionRequestR::ParseData(double&       aSimTime,
                                     double&       aAppRate,
                                     double&       aAppTimeStep,
                                     unsigned int& aTimeScheme,
                                     std::string&  aExerciseToJoinId) const
{
   // The following specifies the data representation for specific Non-Real Time protocol data.
   //   a) Application rate shall be represented by a 32-bit floating point number.
   //   b) Application timestamp shall be represented by a timestamp (see 6.2.88).
   //   c) Feedback time shall be represented by a timestamp (see 6.2.88).
   //   d) Simulation rate shall be represented by a 32-bit floating point number.
   //   e) Simulation time shall be represented by a Clock Time record (see 6.2.16).
   //   f) Simulation timestep shall be represented by a Clock Time record (see 6.2.16).
   //   g) Time interval shall be represented by a timestamp (see 6.2.88).
   //   h) Time latency shall be represented by a timestamp (see 6.2.88).
   //   i) Time scheme shall be represented by a 32-bit unsigned integer (see Section 7 of SISO-REF-010).

   // const unsigned int FALSE = 0;
   unsigned int paramsSupplied = 0;
   unsigned int paramsParsed   = 0;
   aExerciseToJoinId.clear();
   aTimeScheme = DisEnum::Control::Scheme::OTHER;

   GenIConvertBigEndian converter;
   unsigned char        charString[4];
   DisEnum32            id;

   unsigned int iDatum;
   for (iDatum = 0; iDatum != GetData().GetNumFixedDatums(); ++iDatum)
   {
      ++paramsSupplied;

      const std::vector<DisFixedDatum>& fixedArray = GetData().GetFixedDatums();
      const DisFixedDatum&              fixedItem  = fixedArray[iDatum];
      id                                           = fixedItem.GetId();
      fixedItem.GetData(charString);

      if (id == DisEnum::DatumTag::APPLICATION_RATE)
      {
         float floatValue;
         converter.GetFromBuf(charString, floatValue);
         aAppRate = static_cast<double>(floatValue);
         ++paramsParsed;
      }
      else if (id == DisEnum::DatumTag::TIME_SCHEME)
      {
         DisUint32 uintValue;
         converter.GetFromBuf(charString, uintValue);
         aTimeScheme = uintValue;
         ++paramsParsed;
      }
      else
      {
         if (WsfDisUtil::DebugEnabled(mInterfacePtr))
         {
            auto out = ut::log::warning() << "WsfDisActionRequestR: Unknown fixed datum encountered.";
            out.AddNote() << "Ignored.";
         }
      }
   }

   for (iDatum = 0; iDatum != GetData().GetNumVariableDatums(); ++iDatum)
   {
      ++paramsSupplied;

      const std::vector<DisVariableDatum>& array2 = GetData().GetVariableDatums();
      const DisVariableDatum&              item2  = array2[iDatum];

      id = item2.GetId();

      if (id == DisEnum::DatumTag::EXERCISE_NAME)
      {
         bool containsName = false;

         const unsigned char* buffer;
         unsigned int         length2;

         item2.GetData(buffer, length2);
         std::string exerciseToJoin;
         for (unsigned int j = 0; j < length2; j += 8)
         {
            exerciseToJoin.push_back(buffer[j / 8]);
            containsName = true;
         }

         if (containsName)
         {
            aExerciseToJoinId = exerciseToJoin;
         }

         ++paramsParsed;
      }
      else if (id == DisEnum::DatumTag::APPLICATION_TIME)
      {
         // unsigned int absoluteFlag;
         DisClockTime disTime;
         item2.GetClockTimeFromBuffer(disTime);
         // WsfDisInterface::DisClockTimeToSimTime(disTime, absoluteFlag, aSimTime);
         // assert(absoluteFlag == FALSE);
         ++paramsParsed;
      }
      else if (id == DisEnum::DatumTag::APPLICATION_TIME_STEP)
      {
         // unsigned int absoluteFlag;
         DisClockTime disTime;
         item2.GetClockTimeFromBuffer(disTime);
         // WsfDisInterface::DisClockTimeToSimTime(disTime, absoluteFlag, aAppTimeStep);
         // assert(absoluteFlag == FALSE);
         ++paramsParsed;
      }
   }

   return paramsParsed == paramsSupplied;
}
