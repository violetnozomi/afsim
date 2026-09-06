// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfOrbitalConjunctionProcessor.hpp"

#include <memory>

#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtOrbitalPropagator.hpp"
#include "WsfEnvironment.hpp"
#include "WsfNORAD_OrbitalPropagator.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTrackList.hpp"
#include "script/WsfScriptContext.hpp"

WsfOrbitalConjunctionProcessor::WsfOrbitalConjunctionProcessor(const WsfScenario& aScenario)
   : WsfScriptProcessor(aScenario, "WsfOrbitalConjunctionProcessor", "PROCESSOR")
   , mOptions{10000.0, UtMath::cPI / 60.0, 8.0, 0.0, 0.0, 1.0, 10.0}
   , mSearchInterval{36000.0}
   , mPropPtr{nullptr}
   , mDebug{false}
   , mPrimaries{}
   , mOnConjunctionPredictedPtr{nullptr}
{
}

WsfOrbitalConjunctionProcessor::WsfOrbitalConjunctionProcessor(const WsfScenario& aScenario,
                                                               const std::string& aPartClass,
                                                               const std::string& aPartName)
   : WsfScriptProcessor(aScenario, aPartClass, aPartName)
   , mOptions{10000.0, UtMath::cPI / 60.0, 8.0, 0.0, 0.0, 1.0, 10.0}
   , mSearchInterval{36000.0}
   , mPropPtr{nullptr}
   , mDebug{false}
   , mPrimaries{}
   , mOnConjunctionPredictedPtr{nullptr}
{
}

WsfOrbitalConjunctionProcessor::WsfOrbitalConjunctionProcessor(const WsfOrbitalConjunctionProcessor& aSrc)
   : WsfScriptProcessor(aSrc)
   , mOptions(aSrc.mOptions)
   , mSearchInterval{aSrc.mSearchInterval}
   , mPropPtr{nullptr}
   , mDebug{aSrc.mDebug}
   , mPrimaries(aSrc.mPrimaries)
   , mOnConjunctionPredictedPtr{nullptr}
{
   if (aSrc.mPropPtr != nullptr)
   {
      mPropPtr.reset(aSrc.mPropPtr->Clone());
   }
   mOnConjunctionPredictedPtr = mContextPtr->FindScript("on_conjunction_predicted");
}

//! Clone this object.
WsfProcessor* WsfOrbitalConjunctionProcessor::Clone() const
{
   return new WsfOrbitalConjunctionProcessor(*this);
}

//! Process the input to setup any options on this processor.
//! This will examine the @p aInput argument to see if the current command
//! applies to this object. If so, it will respond to that command. This
//! could be either by accepting a command that applies for
//! Orbital Conjunction Processors, or for its base class.
//! \param aInput - the input stream.
//! \returns      - true if this object could interpret the command; false otherwise.
bool WsfOrbitalConjunctionProcessor::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "search_interval")
   {
      aInput.ReadValueOfType(mSearchInterval, UtInput::cTIME);
      aInput.ValueGreaterOrEqual(mSearchInterval, 0.0);
   }
   else if (command == "filter_cut_distance")
   {
      aInput.ReadValueOfType(mOptions.mCutRadius, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mOptions.mCutRadius, 0.0);
   }
   else if (command == "search_step_size")
   {
      aInput.ReadValueOfType(mOptions.mStepRadians, UtInput::cANGLE);
      aInput.ValueGreater(mOptions.mStepRadians, 0.0);
   }
   else if (command == "exclusion_factor")
   {
      aInput.ReadValue(mOptions.mExclusionFactor);
      aInput.ValueGreaterOrEqual(mOptions.mExclusionFactor, 1.0);
   }
   else if (command == "default_variance")
   {
      aInput.ReadValueOfType(mOptions.mDefaultVariance, UtInput::cLENGTH);
      aInput.ValueGreater(mOptions.mDefaultVariance, 0.0);
   }
   else if (command == "default_object_size")
   {
      aInput.ReadValueOfType(mOptions.mDefaultObjectRadius, UtInput::cLENGTH);
      aInput.ValueGreater(mOptions.mDefaultObjectRadius, 0.0);
   }
   else if (command == "prediction_model")
   {
      std::string propModel;
      aInput.ReadValue(propModel);
      if (propModel == "default")
      {
         // do nothing; we will create the propagator during Initialize().
      }
      else if (propModel == "norad")
      {
         mPropPtr = ut::make_unique<WsfNORAD_OrbitalPropagator>();
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "primary")
   {
      UtInputBlock inputBlock(aInput);
      std::string  primary;
      while (inputBlock.ReadCommand(primary))
      {
         mPrimaries.insert(primary);
      }
   }
   else if (command == "debug")
   {
      mDebug = true;
   }
   else if (WsfScriptProcessor::ProcessInput(aInput))
   {
   }
   else if (mContextPtr->ProcessInput(aInput))
   {
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

//! First round initialization of this object.
bool WsfOrbitalConjunctionProcessor::Initialize(double aSimTime)
{
   bool ok = WsfScriptProcessor::Initialize(aSimTime);

   if (mPropPtr == nullptr)
   {
      mPropPtr = ut::make_unique<UtOrbitalPropagator>(
         ut::make_unique<ut::OrbitalState>(GetPlatform()->GetSimulation()->GetEnvironment().GetCentralBody(),
                                           ut::OrbitalState::CoordinateSystem::cEQUATORIAL,
                                           ut::OrbitalState::ReferenceFrame::cECI));
   }

   mOnConjunctionPredictedPtr = mContextPtr->FindScript("on_conjunction_predicted");
   if (mOnConjunctionPredictedPtr != nullptr)
   {
      ok &= mContextPtr->ValidateScript(mOnConjunctionPredictedPtr, "void", "Array<WsfOrbitalConjunctionReport>");
   }

   return ok;
}

//! Perform the conjunction assessment at periodic intervals.
void WsfOrbitalConjunctionProcessor::Update(double aSimTime)
{
   WsfScriptProcessor::Update(aSimTime);

   mOptions.mInitialSearchTime = aSimTime;
   mOptions.mFinalSearchTime   = aSimTime + mSearchInterval;
   if (mDebug)
   {
      auto logger = ut::log::debug() << "Updating conjunction processor.";
      logger.AddNote() << "T = " << aSimTime;
      logger.AddNote() << "Searching time interval [" << mOptions.mInitialSearchTime << ", "
                       << mOptions.mFinalSearchTime << "].";
   }

   std::vector<WsfLocalTrack*> primaryTracks{};
   std::vector<WsfLocalTrack*> secondaryTracks{};
   CategorizeLocalTracks(primaryTracks, secondaryTracks);

   if (!primaryTracks.empty())
   {
      std::vector<WsfOrbitalConjunctionRecord> records{};
      RunPrimaryPrimary(primaryTracks, records);
      RunPrimarySecondary(primaryTracks, secondaryTracks, records);
      SortRecords(records);
      if (mDebug)
      {
         auto logger = ut::log::debug() << "Discovered possible conjunctions.";
         logger.AddNote() << "Number of Conjunctions: " << records.size();
      }

      if ((mOnConjunctionPredictedPtr != nullptr) && (!records.empty()))
      {
         UtScriptClass*             classPtr            = mContextPtr->GetClass("WsfOrbitalConjunctionReport");
         std::vector<UtScriptData>* recordsForScriptPtr = new std::vector<UtScriptData>();
         for (auto& record : records)
         {
            recordsForScriptPtr->push_back(UtScriptData(new UtScriptRef(&record, classPtr, UtScriptRef::cDONT_MANAGE)));
         }

         UtScriptData     scriptRetVal;
         UtScriptDataList scriptArgs;
         UtScriptClass*   arrayClassPtr = mContextPtr->GetClass("Array");
         scriptArgs.push_back(new UtScriptRef(recordsForScriptPtr, arrayClassPtr));
         mContextPtr->ExecuteScript(aSimTime, mOnConjunctionPredictedPtr, scriptRetVal, scriptArgs);
         delete recordsForScriptPtr;
      }
   }
   else if (mDebug)
   {
      ut::log::debug() << "Conjunction analysis skipped: No primary objects tracked.";
   }
}

//! Determine if each local track is either a primary or a secondary object.
//! Each local track on this processor's owning platform is examined and placed into
//! @p aPrimaries if the track corresponds to one of the primary objects of concern to this
//! processor, or @p aSecondaries if it is not a primary object, but it is in the space
//! domain. Tracks that do not have a valid position and velocity are not categorized,
//! and are skipped for this update cycle.
//! \param aPrimaries [out]   - a collection of local tracks referring to primary objects.
//! \param aSecondaries [out] - a collection of local tracks referring to secondary objects.
void WsfOrbitalConjunctionProcessor::CategorizeLocalTracks(std::vector<WsfLocalTrack*>& aPrimaries,
                                                           std::vector<WsfLocalTrack*>& aSecondaries)
{
   WsfLocalTrackList& tracks = GetPlatform()->GetMasterTrackList();
   for (unsigned int i = 0; i < tracks.GetTrackCount(); ++i)
   {
      WsfLocalTrack* trackPtr = tracks.GetTrackEntry(i);
      if (trackPtr->GetSpatialDomain() == WsfSpatialDomain::WSF_SPATIAL_DOMAIN_SPACE)
      {
         // Filter out tracks that do not have both a position and velocity; insufficiently
         // defined tracks will lead to poor orbit definition.
         if (trackPtr->LocationValid() && trackPtr->VelocityValid())
         {
            if (mPrimaries.find(trackPtr->GetTargetName()) != mPrimaries.end())
            {
               aPrimaries.push_back(trackPtr);
            }
            else
            {
               aSecondaries.push_back(trackPtr);
            }
         }
      }
   }
   if (mDebug)
   {
      auto logger = ut::log::debug() << "Performing conjunction analysis for primary objects and secondary objects.";
      logger.AddNote() << "Number of Primary Objects: " << aPrimaries.size();
      logger.AddNote() << "Number of Secondary Objects: " << aSecondaries.size();
   }
}

//! Run a set of conjunction assessments.
//! This will assess conjunction possibilities for the @p aPrimary track against the range of target
//! tracks @p aBegin (inclusive) and @p aEnd (exclusive). For any predicted conjunction a record
//! giving details of that conjunction is added to @p aRecords.
//! \param aPrimary       - The primary track for the assessment.
//! \param aBegin         - The first target track for the assessment. This forms a range with @p aEnd.
//! \param aEnd           - (One past the) final target track for the assessment. This forms a range with @p aStart.
//! \param aRecords [out] - The set of predicted conjunction records.
void WsfOrbitalConjunctionProcessor::RunPairs(std::vector<WsfLocalTrack*>::const_iterator aPrimary,
                                              std::vector<WsfLocalTrack*>::const_iterator aBegin,
                                              std::vector<WsfLocalTrack*>::const_iterator aEnd,
                                              std::vector<WsfOrbitalConjunctionRecord>&   aRecords)
{
   for (auto i = aBegin; i != aEnd; ++i)
   {
      WsfOrbitalConjunctionAssessment conj(*(*aPrimary),
                                           mOptions.mDefaultObjectRadius,
                                           *(*i),
                                           mOptions.mDefaultObjectRadius,
                                           mOptions,
                                           mPropPtr.get());
      while (!conj.ReachedFinalTime())
      {
         if (WsfOrbitalConjunctionAssessment::Status::cCONJUNCTION_FOUND == conj.FindNext())
         {
            aRecords.push_back(conj.CurrentConjunction());
         }
      }
   }
}

//! Run conjunction assessments between the set of primaries with each other.
//! \param aPrimaries     - The collection of primary tracks.
//! \param aRecords [out] - The collection of conjunction records.
void WsfOrbitalConjunctionProcessor::RunPrimaryPrimary(const std::vector<WsfLocalTrack*>&        aPrimaries,
                                                       std::vector<WsfOrbitalConjunctionRecord>& aRecords)
{
   for (auto i = aPrimaries.cbegin(), k = aPrimaries.cend(); i != k; ++i)
   {
      auto j = i + 1;
      RunPairs(i, j, k, aRecords);
   }
}

//! Run conjunction assessments between each primary and the collection of secondaries.
//! \param aPrimaries     - The collection of primary tracks.
//! \param aSecondaries   - The collection of secondary tracks.
//! \param aRecords [out] - The collection of conjunction records.
void WsfOrbitalConjunctionProcessor::RunPrimarySecondary(const std::vector<WsfLocalTrack*>&        aPrimaries,
                                                         const std::vector<WsfLocalTrack*>&        aSecondaries,
                                                         std::vector<WsfOrbitalConjunctionRecord>& aRecords)
{
   for (auto i = aPrimaries.cbegin(), k = aPrimaries.cend(); i != k; ++i)
   {
      RunPairs(i, aSecondaries.cbegin(), aSecondaries.cend(), aRecords);
   }
}

//! Sort the conjunction records by increasing time of closest approach.
//! \param aRecords [inout] - The collection of conjunction records.
void WsfOrbitalConjunctionProcessor::SortRecords(std::vector<WsfOrbitalConjunctionRecord>& aRecords)
{
   auto predicate = [](const WsfOrbitalConjunctionRecord& a, const WsfOrbitalConjunctionRecord& b) -> bool
   { return a.mMinTime < b.mMinTime; };
   std::sort(aRecords.begin(), aRecords.end(), predicate);
}
