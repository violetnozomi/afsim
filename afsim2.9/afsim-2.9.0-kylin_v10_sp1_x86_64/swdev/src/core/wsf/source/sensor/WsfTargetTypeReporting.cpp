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

#include "WsfTargetTypeReporting.hpp"

#include <cassert>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtRandom.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfObject.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorResult.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrack.hpp"

// =================================================================================================
WsfTargetTypeReporting::WsfTargetTypeReporting()
   : mTargetReportingRules()
   , mDefaultTimeToDeclare(0)
   , mDefaultTimeToReevaluate(0)
   , mReportingRulesSupplied(false)
   , mTargetReportState()
{
   mDefaultTargetRules = NewReportingRules();
}

// =================================================================================================
WsfTargetTypeReporting::WsfTargetTypeReporting(const WsfTargetTypeReporting& aSrc)
   : mTargetReportingRules(aSrc.mTargetReportingRules)
   , mDefaultTargetRules(std::make_shared<ReportingRules>(*aSrc.mDefaultTargetRules))
   , mDefaultTimeToDeclare(aSrc.mDefaultTimeToDeclare)
   , mDefaultTimeToReevaluate(aSrc.mDefaultTimeToReevaluate)
   , mReportingRulesSupplied(aSrc.mReportingRulesSupplied)
   , mTargetReportState()
{
}

// =================================================================================================
bool WsfTargetTypeReporting::ProcessInput(UtInput& aInput)
{
   std::string command(aInput.GetCommand());
   if (command != "reported_target_type")
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
         AddTargetReportTypes(aInput, currentRules, defaultSet, currentTypes);
         hasType      = false;
         hasData      = false;
         defaultSet   = false;
         currentRules = ReportingRulesRef(NewReportingRules());
         currentTypes.clear();
      }

      if (command == "type")
      {
         myCommand = true;
         std::string targetType;
         aInput.ReadValue(targetType);
         currentTypes.emplace_back(targetType);
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
         mDefaultTargetRules->mTimeToDeclare = mDefaultTimeToDeclare;
      }
      else if (command == "default_time_to_reevaluate")
      {
         myCommand = true;
         aInput.ReadValueOfType(mDefaultTimeToReevaluate, UtInput::cTIME);
         mDefaultTargetRules->mTimeToReevaluate = mDefaultTimeToReevaluate;
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
               if (currentRules->mConfidenceTable.empty() && currentRules->mEmitterTable.empty())
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

               std::string reportedType;
               std::string confidenceStr;
               aInput.ReadValue(reportedType);
               aInput.ReadValue(confidenceStr);
               if (confidenceStr == "emitter")
               {
                  if ((currentRules->mReportingType != ReportingRules::cNOTHING) &&
                      (currentRules->mReportingType != ReportingRules::cEMITTERS))
                  {
                     throw UtInput::BadValue(aInput, "reporting by emitters cannot be used with reporting by probability");
                  }

                  myCommand                    = true;
                  hasData                      = true;
                  currentRules->mReportingType = ReportingRules::cEMITTERS;
                  std::list<WsfStringId> emitterList;
                  do
                  {
                     std::string emitterType;
                     aInput.ReadValue(emitterType);
                     emitterList.emplace_back(emitterType);
                     aInput.ReadValue(confidenceStr);
                  } while (confidenceStr == "emitter");
                  aInput.PushBack(confidenceStr);

                  emitterList.unique();
                  emitterList.sort();
                  currentRules->mEmitterTable.emplace_back(emitterList, WsfStringId(reportedType));
               }
               else
               {
                  if ((currentRules->mReportingType != ReportingRules::cNOTHING) &&
                      (currentRules->mReportingType != ReportingRules::cTABLE))
                  {
                     throw UtInput::BadValue(aInput, "reporting by probability cannot be used with reporting by emitters");
                  }

                  currentRules->mReportingType = ReportingRules::cTABLE;
                  myCommand                    = true;
                  hasData                      = true;
                  double confidence;
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
      }
      if (!myCommand)
      {
         throw UtInput::UnknownCommand(aInput);
      }
   }

   if (hasData && hasType)
   {
      AddTargetReportTypes(aInput, currentRules, defaultSet, currentTypes);
   }

   return true;
}

// =================================================================================================
//! Create an emitter report state for the specified transmitter if it doesn't already exist.
void WsfTargetTypeReporting::AddTargetReportState(double aSimTime, WsfPlatform* aTargetPtr)
{
   size_t tgtIndex = aTargetPtr->GetIndex();
   if (mTargetReportState.find(tgtIndex) == mTargetReportState.end())
   {
      ReportingRulesRef rulesPtr = FindReportingRules(*aTargetPtr);
      ReportState       state;
      state.mNextTimeToEvaluate = aSimTime + rulesPtr->mTimeToDeclare;
      state.mTruthId            = aTargetPtr->GetTypeId();
      state.mTargetDerivedId    = nullptr;
      state.mReportConfidence   = 0;
      state.mRules              = rulesPtr;
      state.mUniqueId           = aTargetPtr->GetUniqueId();
      mTargetReportState.insert(std::make_pair(tgtIndex, state));
   }
}

// =================================================================================================
void WsfTargetTypeReporting::RemoveAllTargetReportStates()
{
   mTargetReportState.clear();
}

// =================================================================================================
void WsfTargetTypeReporting::RemoveTargetReportState(size_t aPlatformIndex)
{
   auto reportIter = mTargetReportState.find(aPlatformIndex);
   if (reportIter != mTargetReportState.end())
   {
      mTargetReportState.erase(reportIter);
   }
}

// =================================================================================================
void WsfTargetTypeReporting::UpdateReportedTargetType(double           aSimTime,
                                                      bool             aSensorReportsType,
                                                      WsfSensorResult& aResult,
                                                      WsfPlatform*     aTargetPtr,
                                                      WsfTrack*        aTrackPtr)
{
   auto rsi = mTargetReportState.find(aTargetPtr->GetIndex());
   if (rsi != mTargetReportState.end())
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

         WsfStringId derivedTgtId;
         if (state.mRules->mReportingType == ReportingRules::cTRUTH)
         {
            state.mReportConfidence = 1.0;
            state.mTargetDerivedId  = state.mTruthId;
         }
         else if (state.mRules->mReportingType == ReportingRules::cTABLE)
         {
            state.mReportConfidence =
               state.mRules->DrawForType(aTargetPtr->GetSimulation()->GetRandom(), state.mTargetDerivedId);
         }
         else if (state.mRules->mReportingType == ReportingRules::cEMITTERS)
         {
            int                    emitCount(aTrackPtr->GetEmitterTypeIdCount());
            std::list<WsfStringId> emitList;
            for (int i = 0; i < emitCount; ++i)
            {
               WsfTrack::EmitterTypeData emitData;
               aTrackPtr->GetEmitterTypeIdEntry(i, emitData);
               emitList.push_back(emitData.mEmitterDerivedId);
            }
            emitList.sort();
            state.mReportConfidence = state.mRules->DrawForType(emitList, state.mTargetDerivedId);
         }
         else if ((!mReportingRulesSupplied) && aSensorReportsType)
         {
            // If no reporting rules were provided then type would not normally be provided. But if
            // the user declared 'reports_type' in the sensor definition then truth type will be
            // reported. This provides compatibility with the original base class implementation.
            state.mReportConfidence = 1.0;
            state.mTargetDerivedId  = state.mTruthId;
         }
         else
         {
            state.mReportConfidence = 0.0;
            state.mTargetDerivedId  = nullptr;
         }
      }
      if (state.mTargetDerivedId != 0)
      {
         aResult.mMeasurement.SetTypeId(state.mTargetDerivedId);
         aResult.mMeasurement.SetTypeIdValid(true);
         aTrackPtr->SetTypeIdValid(true);
         aTrackPtr->SetTypeId(state.mTargetDerivedId);
      }
      else
      {
         aTrackPtr->SetTypeIdValid(false);
         aTrackPtr->SetTypeId(nullptr);
      }
   }
   else
   {
      aTrackPtr->SetTypeIdValid(false);
      aTrackPtr->SetTypeId(nullptr);
   }
}

// =================================================================================================
// private
void WsfTargetTypeReporting::AddTargetReportTypes(UtInput&                aInput,
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
      mTargetReportingRules.insert(make_pair(typeId, aReportingRules));
   }
   if (aIsDefault)
   {
      mDefaultTargetRules = aReportingRules;
   }
   mReportingRulesSupplied = true;
}

// =================================================================================================
//! Returns the reporting rules for the specified WsfObject
// private
WsfTargetTypeReporting::ReportingRulesRef WsfTargetTypeReporting::FindReportingRules(const WsfObject& aTargetType)
{
   ReportingRulesRef reportRules;
   auto              ruleIter = mTargetReportingRules.find(aTargetType.GetTypeId());
   if (ruleIter != mTargetReportingRules.end())
   {
      reportRules = ruleIter->second;
   }
   else
   {
      const WsfObject::TypeList& typeList = aTargetType.GetTypeList();
      if (!typeList.empty())
      {
         WsfStringId currentTypeName;
         auto        typeIter = typeList.begin();
         do
         {
            currentTypeName = *typeIter;
            ruleIter        = mTargetReportingRules.find(currentTypeName);
            if (ruleIter != mTargetReportingRules.end())
            {
               reportRules = ruleIter->second;
               mTargetReportingRules.insert(make_pair(currentTypeName, reportRules));
            }
            ++typeIter;
         } while (reportRules && (typeIter != typeList.end()));

         if (!reportRules)
         {
            reportRules = mDefaultTargetRules;
            mTargetReportingRules.insert(make_pair(currentTypeName, mDefaultTargetRules));
         }
      }
   }

   assert(reportRules);
   return reportRules;
}

// =================================================================================================
// private
WsfTargetTypeReporting::ReportingRulesRef WsfTargetTypeReporting::NewReportingRules()
{
   auto rules               = std::make_shared<ReportingRules>();
   rules->mTimeToDeclare    = mDefaultTimeToDeclare;
   rules->mTimeToReevaluate = mDefaultTimeToDeclare;
   rules->mReportingType    = ReportingRules::cNOTHING;
   return rules;
}

// =================================================================================================
// Nested struct WsfTargetTypeReporting::ReportingRules.
// =================================================================================================

// =================================================================================================
//! Determine the reported emitter type based on a random draw
//! @param aRandom             The random number generator
//! @param aReportedTargetType Set to the drawn emitter type name
//! @return The confidence value for the reported type.
double WsfTargetTypeReporting::ReportingRules::DrawForType(ut::Random& aRandom, WsfStringId& aReportedTargetType)
{
   // Do a linear search to find the correct emitter type.
   double draw  = aRandom.Uniform<double>();
   int    index = 0;
   double prev  = 0.0;
   while (mConfidenceTable[index].first <= draw)
   {
      prev = mConfidenceTable[index++].first;
   }
   aReportedTargetType = mConfidenceTable[index].second;
   return mConfidenceTable[index].first - prev;
}

// =================================================================================================
//! Determine the reported emitter type based on detected emitters
//! @param aEmitterList        The list of detected emitters
//! @param aReportedTargetType Set to the drawn emitter type name
//! @return The confidence value for the reported type.
double WsfTargetTypeReporting::ReportingRules::DrawForType(std::list<WsfStringId>& aEmitterList,
                                                           WsfStringId&            aReportedTargetType)
{
   if (!aEmitterList.empty())
   {
      for (auto& eti : mEmitterTable)
      {
         if (eti.first == aEmitterList)
         {
            aReportedTargetType = eti.second;
            return 1.0;
         }
      }
   }
   return 0.0;
}
