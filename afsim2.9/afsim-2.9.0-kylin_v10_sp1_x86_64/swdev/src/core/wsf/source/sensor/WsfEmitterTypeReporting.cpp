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

#include "WsfEmitterTypeReporting.hpp"

#include <cassert>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtRandom.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfObject.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"

// =================================================================================================
WsfEmitterTypeReporting::WsfEmitterTypeReporting()
   : mEmitterReportingRules()
   , mDefaultTimeToDeclare(0)
   , mDefaultTimeToReevaluate(0)
   , mReportingRulesSupplied(false)
   , mEmitterReportState()
{
   mDefaultEmitterRules = NewReportingRules();
}

// =================================================================================================
WsfEmitterTypeReporting::WsfEmitterTypeReporting(const WsfEmitterTypeReporting& aSrc)
   : mEmitterReportingRules(aSrc.mEmitterReportingRules)
   , mDefaultEmitterRules(std::make_shared<ReportingRules>(*aSrc.mDefaultEmitterRules))
   , mDefaultTimeToDeclare(aSrc.mDefaultTimeToDeclare)
   , mDefaultTimeToReevaluate(aSrc.mDefaultTimeToReevaluate)
   , mReportingRulesSupplied(aSrc.mReportingRulesSupplied)
   , mEmitterReportState()
{
}

// =================================================================================================
WsfEmitterTypeReporting& WsfEmitterTypeReporting::operator=(const WsfEmitterTypeReporting& aRhs)
{
   if (this != &aRhs)
   {
      mEmitterReportingRules   = aRhs.mEmitterReportingRules;
      mDefaultTimeToDeclare    = aRhs.mDefaultTimeToDeclare;
      mDefaultTimeToReevaluate = aRhs.mDefaultTimeToReevaluate;
      mReportingRulesSupplied  = aRhs.mReportingRulesSupplied;
      mEmitterReportState.clear();

      mDefaultEmitterRules = std::make_shared<ReportingRules>(*aRhs.mDefaultEmitterRules);
   }
   return *this;
}

// =================================================================================================
bool WsfEmitterTypeReporting::ProcessInput(UtInput& aInput)
{
   std::string command(aInput.GetCommand());
   if (command != "reported_emitter_type")
   {
      return false;
   }

   UtInputBlock inputBlock(aInput);

   std::list<WsfStringId> currentTypes;
   bool                   defaultSet = false;
   ReportingRulesRef      currentRules(NewReportingRules());
   bool                   hasType = false;
   bool                   hasData = false;

   while (inputBlock.ReadCommand(command))
   {
      bool myCommand = false;
      if (hasType && hasData && (command == "type" || command == "default_type"))
      {
         AddEmitterReportTypes(aInput, currentRules, defaultSet, currentTypes);
         hasType      = false;
         hasData      = false;
         defaultSet   = false;
         currentRules = ReportingRulesRef(NewReportingRules());
         currentTypes.clear();
      }

      if (command == "type")
      {
         myCommand = true;
         std::string emitterType;
         aInput.ReadValue(emitterType);
         currentTypes.emplace_back(emitterType);
         hasType = true;
      }
      else if (command == "default_type")
      {
         myCommand  = true;
         defaultSet = true;
         hasType    = true;
      }
      else if (command == "default_time_to_declare")
      {
         myCommand = true;
         aInput.ReadValueOfType(mDefaultTimeToDeclare, UtInput::cTIME);
         mDefaultEmitterRules->mTimeToDeclare = mDefaultTimeToDeclare;
      }
      else if (command == "default_time_to_reevaluate")
      {
         myCommand = true;
         aInput.ReadValueOfType(mDefaultTimeToReevaluate, UtInput::cTIME);
         mDefaultEmitterRules->mTimeToReevaluate = mDefaultTimeToReevaluate;
      }
      else
      {
         if (hasType)
         {
            if (command == "time_to_declare")
            {
               myCommand = true;
               hasData   = true;
               aInput.ReadValueOfType(currentRules->mTimeToDeclare, UtInput::cTIME);
            }
            else if (command == "time_to_reevaluate")
            {
               myCommand = true;
               hasData   = true;
               aInput.ReadValueOfType(currentRules->mTimeToReevaluate, UtInput::cTIME);
            }
            else if (command == "report_truth")
            {
               if (currentRules->mConfidenceTable.empty())
               {
                  myCommand                    = true;
                  currentRules->mReportingType = ReportingRules::cTRUTH;
               }
               else
               {
                  throw UtInput::BadValue(aInput, "report_truth cannot be used with report_type");
               }
            }
            else if (command == "report_type")
            {
               if (currentRules->mReportingType == ReportingRules::cTRUTH)
               {
                  throw UtInput::BadValue(aInput, "report_truth cannot be used with report_type");
               }
               currentRules->mReportingType = ReportingRules::cTABLE;
               myCommand                    = true;
               hasData                      = true;
               double      confidence;
               std::string reportedType;
               std::string confidenceStr;
               aInput.ReadValue(reportedType);
               aInput.ReadValue(confidenceStr);
               if (confidenceStr == "remainder")
               {
                  if (currentRules->mConfidenceTable.empty())
                  {
                     confidence = 1.0;
                  }
                  else
                  {
                     confidence = 1.0 - currentRules->mConfidenceTable.back().first;
                     if (confidence <= 0.0)
                     {
                        throw UtInput::BadValue(aInput, "Total confidence is already at 1.0, remainder is 0.0");
                     }
                  }
               }
               else
               {
                  aInput.PushBack(confidenceStr);
                  aInput.ReadValue(confidence);
               }

               double lastValue = 0.0;
               if (!currentRules->mConfidenceTable.empty())
               {
                  lastValue = currentRules->mConfidenceTable.back().first;
               }
               double confidenceSum = lastValue + confidence;
               if (1.0 - confidenceSum < -.001)
               {
                  throw UtInput::BadValue(aInput, "Confidence values add up to greater than 1.0");
               }
               currentRules->mConfidenceTable.emplace_back(confidenceSum, WsfStringId(reportedType));
            }
         }
      }

      if (!myCommand)
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   if (hasData && hasType)
   {
      AddEmitterReportTypes(aInput, currentRules, defaultSet, currentTypes);
   }

   return true;
}

// =================================================================================================
//! Create an emitter report state for the specified transmitter if it doesn't already exist.
void WsfEmitterTypeReporting::AddEmitterReportState(double aSimTime, WsfEM_Xmtr* aXmtrPtr)
{
   WsfPlatformPart*  xmtrPartPtr = aXmtrPtr->GetArticulatedPart();
   TargetEmitterPair tgtEmtrPair =
      std::make_pair(aXmtrPtr->GetPlatform()->GetIndex(), aXmtrPtr->GetArticulatedPart()->GetUniqueId());
   if (mEmitterReportState.find(tgtEmtrPair) == mEmitterReportState.end())
   {
      ReportingRulesRef rulesPtr = FindReportingRules(*xmtrPartPtr);
      ReportState       state;
      state.mNextTimeToEvaluate = aSimTime + rulesPtr->mTimeToDeclare;
      state.mTruthId            = xmtrPartPtr->GetTypeId();
      state.mEmitterDerivedId   = nullptr;
      state.mReportConfidence   = 0;
      state.mRules              = rulesPtr;
      state.mUniqueXmtrPartId   = xmtrPartPtr->GetUniqueId();
      mEmitterReportState.insert(make_pair(tgtEmtrPair, state));
   }
}

// =================================================================================================
void WsfEmitterTypeReporting::RemoveAllEmitterReportStates()
{
   mEmitterReportState.clear();
}

// =================================================================================================
void WsfEmitterTypeReporting::RemoveEmitterReportState(size_t aPlatformIndex, unsigned int aUniqueXmtrPartId)
{
   auto reportIter = mEmitterReportState.find(std::make_pair(aPlatformIndex, aUniqueXmtrPartId));
   if (reportIter != mEmitterReportState.end())
   {
      if (reportIter->second.mUniqueXmtrPartId == aUniqueXmtrPartId)
      {
         mEmitterReportState.erase(reportIter);
      }
   }
}

// =================================================================================================
void WsfEmitterTypeReporting::UpdateReportedEmitterType(double           aSimTime,
                                                        bool             aSensorReportsType,
                                                        WsfSensorResult& aResult,
                                                        WsfPlatform*     aTargetPtr,
                                                        WsfTrack*        aTrackPtr)
{
   bool                                   emitterTypeIdsValid(false);
   std::vector<WsfTrack::EmitterTypeData> emitterTypeIds;
   for (WsfComponentList::RoleIterator<WsfArticulatedPart> iter(*aTargetPtr); !iter.AtEnd(); ++iter)
   {
      WsfArticulatedPart* partPtr = (*iter);
      if (partPtr == nullptr)
      {
         continue;
      }

      TargetEmitterPair tgtEmtrPair(std::make_pair(aTargetPtr->GetIndex(), partPtr->GetUniqueId()));
      auto              rsi = mEmitterReportState.find(tgtEmtrPair);
      if (rsi != mEmitterReportState.end())
      {
         ReportState& state = rsi->second;
         if (aSimTime >= state.mNextTimeToEvaluate)
         {
            if (state.mRules->mTimeToReevaluate > 0.0)
            {
               while (aSimTime >= state.mNextTimeToEvaluate)
               {
                  state.mNextTimeToEvaluate += state.mRules->mTimeToReevaluate;
               }
            }

            WsfStringId derivedEmitterId;
            if (state.mRules->mReportingType == ReportingRules::cTRUTH)
            {
               state.mReportConfidence = 1.0;
               state.mEmitterDerivedId = state.mTruthId;
            }
            else if (state.mRules->mReportingType == ReportingRules::cTABLE)
            {
               state.mReportConfidence =
                  state.mRules->DrawForType(aTargetPtr->GetSimulation()->GetRandom(), state.mEmitterDerivedId);
            }
            else if ((!mReportingRulesSupplied) && aSensorReportsType)
            {
               // If no reporting rules were provided then type would not normally be provided. But if
               // the user declared 'reports_type' in the sensor definition then truth type will be
               // reported. This provides compatibility with the original base class implementation.
               state.mReportConfidence = 1.0;
               state.mEmitterDerivedId = state.mTruthId;
            }
            else
            {
               state.mReportConfidence = 0.0;
               state.mEmitterDerivedId = nullptr;
            }
         }

         if (state.mEmitterDerivedId != 0)
         {
            emitterTypeIds.emplace_back(state.mTruthId, state.mEmitterDerivedId);
            emitterTypeIdsValid = true;
         }
      }
   }
   aTrackPtr->SetEmitterTypeIdValid(emitterTypeIdsValid);
   aTrackPtr->SetEmitterTypeIdList(emitterTypeIds);
}

// =================================================================================================
// private
void WsfEmitterTypeReporting::AddEmitterReportTypes(UtInput&                aInput,
                                                    ReportingRulesRef       aReportingRules,
                                                    bool                    aIsDefault,
                                                    std::list<WsfStringId>& aTypeList)
{
   if (aReportingRules->mReportingType == ReportingRules::cTABLE)
   {
      if (aReportingRules->mConfidenceTable.empty() || (aReportingRules->mConfidenceTable.back().first < 1.0))
      {
         throw UtInput::BadValue(aInput, "Confidence values do not add up to 1.0");
      }
   }

   for (auto& typeId : aTypeList)
   {
      mEmitterReportingRules.insert(make_pair(typeId, aReportingRules));
   }
   if (aIsDefault)
   {
      mDefaultEmitterRules = aReportingRules;
   }
   mReportingRulesSupplied = true;
}

// =================================================================================================
//! Returns the reporting rules for the specified WsfObject
// private
WsfEmitterTypeReporting::ReportingRulesRef WsfEmitterTypeReporting::FindReportingRules(const WsfObject& aEmitterType)
{
   ReportingRulesRef reportRules;
   auto              ruleIter = mEmitterReportingRules.find(aEmitterType.GetTypeId());
   if (ruleIter != mEmitterReportingRules.end())
   {
      reportRules = ruleIter->second;
   }
   else
   {
      const WsfObject::TypeList& typeList = aEmitterType.GetTypeList();
      if (!typeList.empty())
      {
         WsfStringId currentTypeName;
         auto        typeIter = typeList.begin();
         do
         {
            currentTypeName = *typeIter;
            ruleIter        = mEmitterReportingRules.find(currentTypeName);
            if (ruleIter != mEmitterReportingRules.end())
            {
               reportRules = ruleIter->second;
               mEmitterReportingRules.emplace(currentTypeName, reportRules);
            }
            ++typeIter;
         } while ((reportRules) && (typeIter != typeList.end()));

         if (!reportRules)
         {
            reportRules = mDefaultEmitterRules;
            mEmitterReportingRules.emplace(currentTypeName, mDefaultEmitterRules);
         }
      }
   }

   assert(reportRules);
   return reportRules;
}

// =================================================================================================
// private
WsfEmitterTypeReporting::ReportingRulesRef WsfEmitterTypeReporting::NewReportingRules()
{
   ReportingRulesRef rules(new ReportingRules);
   rules->mTimeToDeclare    = mDefaultTimeToDeclare;
   rules->mTimeToReevaluate = mDefaultTimeToDeclare;
   rules->mReportingType    = ReportingRules::cNOTHING;
   return rules;
}

// =================================================================================================
// Nested struct WsfEmitterTypeReporting::ReportingRules.
// =================================================================================================

// =================================================================================================
//! Determine the reported emitter type based on a random draw
//! @param aRandom              The random number generator
//! @param aReportedEmitterType Set to the drawn emitter type name
//! @return The confidence value for the reported type.
double WsfEmitterTypeReporting::ReportingRules::DrawForType(ut::Random& aRandom, WsfStringId& aReportedEmitterType)
{
   // Do a linear search to find the correct emitter type.
   double draw  = aRandom.Uniform<double>();
   int    index = 0;
   double prev  = 0.0;
   while (mConfidenceTable[index].first <= draw)
   {
      prev = mConfidenceTable[index++].first;
   }
   aReportedEmitterType = mConfidenceTable[index].second;
   return mConfidenceTable[index].first - prev;
}
