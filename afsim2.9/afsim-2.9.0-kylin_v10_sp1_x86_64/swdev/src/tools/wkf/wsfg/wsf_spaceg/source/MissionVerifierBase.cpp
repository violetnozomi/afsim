// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "MissionVerifierBase.hpp"

#include "OrbitalModelToSequence.hpp"
#include "UtCast.hpp"
#include "UtLogPublisher.hpp"
#include "UtLogSubscriber.hpp"
#include "WsfOrbitalMissionSequence.hpp"
#include "WsfSpaceMoverBase.hpp"
#include "WsfSpaceOrbitalMissionVerificationContext.hpp"
#include "WsfSpaceVerifyTypes.hpp"

namespace
{

// These are taken from the DefaultGradient in UtQtGradientMap.
static const std::vector<std::pair<double, UtColor>> cCOLOR_VALUES = {{0.00, UtColor{1.0f, 1.0f, 1.0f}},
                                                                      {0.01, UtColor{0.0f, 0.0f, 1.0f}},
                                                                      {0.25, UtColor{0.0f, 1.0f, 1.0f}},
                                                                      {0.50, UtColor{0.0f, 1.0f, 0.0f}},
                                                                      {0.75, UtColor{1.0f, 1.0f, 0.0f}},
                                                                      {1.00, UtColor{1.0f, 0.0f, 0.0f}}};

} // namespace

namespace wsfg
{

namespace spaceg
{

//! Verify the given mission sequence for the given platform.
//!
//! This method is a template pattern method to carry out the bulk of the work for
//! verification of mission sequences. The given orbital mission data, @p aData,
//! is converted into a WsfOrbitalMissionSequence, and is verified against the
//! space mover for the platform with the given @p aPlatformName.
//!
//! \param aPlatformName - The name of the platform against which to verify the given mission.
//! \param aData         - The orbital mission data to verify against the given platform.
//! \returns             - The results of the verification.
MissionVerificationResults MissionVerifierBase::Verify(const std::string& aPlatformName, const OrbitalMissionData& aData)
{
   MissionVerificationResults retval{};

   WsfSpaceMoverBase* moverPtr = GetSpaceMover(aPlatformName);
   if (moverPtr)
   {
      UtCalendar simStartTime;
      UtCalendar creationTime;
      std::tie(simStartTime, creationTime) = GetStartEpoch(aPlatformName);

      // Transform the model into a WsfOrbitalMissionSequence.
      OrbitalModelToSequence                     transformer(GetScenario());
      std::unique_ptr<WsfOrbitalMissionSequence> sequencePtr = transformer.Transform(aData);

      ut::log::Subscriber consoleSub{"raw", false};

      auto beforeEventOperation = [&retval, &consoleSub, &simStartTime](int                      aSequenceIndex,
                                                                        const WsfOrbitalEvent*   aEventPtr,
                                                                        const UtCalendar&        aTime,
                                                                        const UtOrbitalElements& aElements,
                                                                        double                   aDeltaV_Avail)
      {
         consoleSub.TakeQueue();
         ut::log::Publisher::RegisterSubscriber(&consoleSub);
         VerificationResults value = retval.GetResults(aSequenceIndex);
         value.SetTimeBefore(aTime);
         value.SetSecondsBefore(aTime.GetTimeSince(simStartTime));
         value.SetElementsBefore(aElements);
         value.SetDeltaV_AvailBefore(aDeltaV_Avail);
         retval.SetResults(aSequenceIndex, value);
      };

      auto afterEventOperation = [&retval, &consoleSub, &simStartTime](int                        aSequenceIndex,
                                                                       const WsfOrbitalEvent*     aEventPtr,
                                                                       const UtCalendar&          aTime,
                                                                       const UtOrbitalElements&   aElements,
                                                                       double                     aDeltaV_Avail,
                                                                       wsf::space::verify::Status aStatus)
      {
         auto joinMessages = [](std::queue<ut::log::Message>&& aQueue) -> std::string
         {
            std::string output;
            while (!aQueue.empty())
            {
               // Because consoleSub is only subscribed to "raw", there should not be any notes.
               output += aQueue.front().mData;
               aQueue.pop();
            }
            return output;
         };

         VerificationResults value = retval.GetResults(aSequenceIndex);
         value.SetTimeAfter(aTime);
         value.SetSecondsAfter(aTime.GetTimeSince(simStartTime));
         value.SetElementsAfter(aElements);
         value.SetDeltaV_AvailAfter(aDeltaV_Avail);
         value.SetStatus(aStatus);
         value.SetOutputLog(joinMessages(consoleSub.TakeQueue()));
         retval.SetResults(aSequenceIndex, value);
      };

      wsf::space::OrbitalMissionVerificationContext verifier{moverPtr->GetOrbitalManeuvering(),
                                                             moverPtr->GetPropagator(),
                                                             *sequencePtr,
                                                             ""};
      verifier.Initialize(creationTime);
      if (!verifier.VerifyMission(beforeEventOperation, afterEventOperation))
      {
         if (retval.GetSize() == 0)
         {
            throw std::runtime_error("Verification of mission sequence failed.\n"
                                     "Check that the platform has a sufficiently defined mover.");
         }
      }

      SetColors(retval);
   }

   return retval;
}

void MissionVerifierBase::SetColors(MissionVerificationResults& aResults)
{
   int  numEvents = ut::cast_to_int(aResults.GetSize());
   auto operation = [numEvents, &aResults](int aIndex, VerificationResults aValues)
   {
      aValues.SetPreviewColor(GetColor(aIndex, numEvents));
      aResults.SetResults(aIndex, aValues);
   };
   aResults.Apply(operation);
}

UtColor MissionVerifierBase::GetColor(int aIndex, int aSize)
{
   UtColor retval{};
   if (aIndex == 0)
   {
      retval = cCOLOR_VALUES[0].second;
   }
   else if (aIndex == aSize - 1)
   {
      retval = cCOLOR_VALUES[cCOLOR_VALUES.size() - 1].second;
   }
   else
   {
      double param = static_cast<double>(aIndex) / (aSize - 1);

      auto comparator = [param](const std::pair<double, UtColor>& aValue) { return aValue.first > param; };

      auto top = std::find_if(cCOLOR_VALUES.begin(), cCOLOR_VALUES.end(), comparator);
      if (top == cCOLOR_VALUES.begin())
      {
         retval = top->second;
      }
      else if (top == cCOLOR_VALUES.end())
      {
         retval = cCOLOR_VALUES.crbegin()->second;
      }
      else
      {
         auto bottom = top;
         --bottom;
         double relParam = (param - bottom->first) / (top->first - bottom->first);
         retval          = UtColor{Mix(bottom->second[0], top->second[0], relParam),
                          Mix(bottom->second[1], top->second[1], relParam),
                          Mix(bottom->second[2], top->second[2], relParam)};
      }
   }
   return retval;
}

float MissionVerifierBase::Mix(float aStart, float aStop, double aParam)
{
   return (1.0f - aParam) * aStart + aParam * aStop;
}

} // namespace spaceg

} // namespace wsfg
