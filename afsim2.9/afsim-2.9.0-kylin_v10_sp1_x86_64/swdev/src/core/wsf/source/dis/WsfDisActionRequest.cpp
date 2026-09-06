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

#include "dis/WsfDisActionRequest.hpp"

#include <string>

#include "Dis.hpp"
#include "DisActionEnums.hpp"
#include "DisClockTime.hpp"
#include "DisDatumEnums.hpp"
#include "DisIO.hpp"
#include "DisTime.hpp"
#include "DisTypes.hpp"
#include "DisVariableDatum.hpp"
#include "GenIConvertBigEndian.hpp"
#include "GenOConvertBigEndian.hpp"
#include "UtLog.hpp"
#include "dis/WsfDisUtil.hpp"

WsfDisActionRequest::WsfDisActionRequest(WsfDisInterface* aInterfacePtr)
   : DisActionRequest()
   , mInterfacePtr(aInterfacePtr)
{
   SetRequestId(WsfDisUtil::GetNextRequestNumber(mInterfacePtr)); // Serialized identifier handle.
   SetOriginatingEntity(DisEntityId(Dis::GetSite(), Dis::GetApplication(), 0));
}

WsfDisActionRequest::WsfDisActionRequest(DisPdu& aPdu, GenI& aGenI, WsfDisInterface* aInterfacePtr)
   : DisActionRequest(aPdu, aGenI)
   , mInterfacePtr(aInterfacePtr)
{
}

// virtual
WsfDisActionRequest::~WsfDisActionRequest() {}

// virtual
int WsfDisActionRequest::Process()
{
   const int DELETE_PDU = 1;

   bool actedUpon = false;

   DisEnum32 action  = GetActionId();
   double    simTime = WsfDisUtil::GetSimTime(mInterfacePtr);

   if (action == DisEnum::Action::Request::JOIN_EXERCISE)
   {
      actedUpon = ProcessJoinExercise(simTime);
   }
   else
   {
      if (WsfDisUtil::DebugEnabled(mInterfacePtr))
      {
         auto out = ut::log::debug() << "WsfDisActionRequest: Unrecognized ActionId received.";
         out.AddNote() << "Request will be ignored.";
      }
   }

   if (WsfDisUtil::DebugEnabled(mInterfacePtr))
   {
      auto out = ut::log::debug() << "WsfDisActionRequest: PDU received and processed.";
      out.AddNote() << "T = " << simTime;
      if (actedUpon)
      {
         out.AddNote() << "Acted upon. No reply required.";
      }
   }

   return DELETE_PDU;
}

// virtual
bool WsfDisActionRequest::ProcessJoinExercise(double aSimTime)
{
   bool success = false;

   if (GetActionId() == DisEnum::Action::Request::JOIN_EXERCISE)
   {
      std::string exerciseToJoinId;
      double      maxAppRate     = 0.0;
      double      maxAppTimeStep = 0.0;
      double      appTimeToStart = 0.0;

      if (ParseJoinExerciseRequest(appTimeToStart, maxAppRate, maxAppTimeStep, exerciseToJoinId))
      {
         ut::log::warning() << "WsfDisActionRequest: Does not yet act upon a Join Exercise Request.";
         success = true;
      }
   }

   return success;
}

void WsfDisActionRequest::MakeJoinExerciseRequest(const DisEntityId& aSimulationManager,
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

void WsfDisActionRequest::MakeTimeAdvanceRequest(const DisEntityId& aSimulationManager,
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

void WsfDisActionRequest::MakeResignExerciseRequest(double aSimTime)
{
   SetActionId(DisEnum::Action::Request::RESIGN_EXERCISE);
   DisClockTime disTime;
   // WsfDisInterface::SimTimeToDisClockTime(aSimTime, disTime);
   GetData().AddVariableDatum(DisVariableDatum(DisEnum::DatumTag::APPLICATION_TIME, disTime));
}

bool WsfDisActionRequest::ParseJoinExerciseRequest(double&      aStartTime,
                                                   double&      aMaxAppRate,
                                                   double&      aMaxAppTimeStep,
                                                   std::string& aExerciseToJoinId) const
{
   bool containsName = false;
   bool containsData = false;

   if (GetActionId() == DisEnum::Action::Request::JOIN_EXERCISE)
   {
      aMaxAppRate     = 0.0;
      aMaxAppTimeStep = 0.0;

      GenIConvertBigEndian converter;
      unsigned char        charString[4];
      float                floatValue;
      DisUint32            uintValue;

      for (unsigned int i = 0; i != GetData().GetNumFixedDatums(); ++i)
      {
         const std::vector<DisFixedDatum>& fixedArray = GetData().GetFixedDatums();
         const DisFixedDatum&              fixedItem  = fixedArray[i];
         DisEnum32                         id         = fixedItem.GetId();
         fixedItem.GetData(charString);

         if (id == DisEnum::DatumTag::APPLICATION_RATE)
         {
            converter.GetFromBuf(charString, floatValue);
            aMaxAppRate  = floatValue;
            containsData = true;
         }
         else if (id == DisEnum::DatumTag::APPLICATION_TIME_STEP)
         {
            converter.GetFromBuf(charString, uintValue);
            aMaxAppTimeStep = DisTime::GetAbsoluteTime(uintValue);
            containsData    = true;
         }
      }

      const std::vector<DisVariableDatum>& array2 = GetData().GetVariableDatums();
      const DisVariableDatum&              item2  = array2[0];

      if ((GetData().GetNumVariableDatums() == 1) && (item2.GetId() == DisEnum::DatumTag::EXERCISE_NAME))
      {
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
      }
   }

   return containsName && containsData;
}
