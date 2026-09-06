// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvPlatformHistoryTracelineData.hpp"

#include "VaAttachmentTraceLine.hpp"
#include "VaUtils.hpp"
#include "WkfEnvironment.hpp"
#include "WkfTeamVisibilityPrefObject.hpp"
#include "WkfUtils.hpp"

/*************************************************************************************************
 * @brief   Constructor. Creates a Traceline attachment and adds it to aPlatform
 * @param   aPlatform   The platform to attach the traceline to.
 * @param   aViewerPtr  The viewer onto which the traceline will be drawn.
 * @date 7/14/2017
 **************************************************************************************************/
RvPlatformHistory::TracelineData::TracelineData(wkf::Platform& aPlatform, vespa::VaViewer* aViewerPtr)
   : mPlatformPtr(&aPlatform)
{
   auto traceline =
      vespa::make_attachment<vespa::VaAttachmentTraceLine>(aPlatform,
                                                           aViewerPtr); // note, this needs to use GetStandardViewer in
                                                                        // a correct implementation
   vespa::VaAttachment::LoadAttachment(*traceline);
   mTraceLineUniqueId = traceline->GetUniqueId();
   traceline->AddStateBack(0.0f, 0.0f);
}

RvPlatformHistory::TracelineData::~TracelineData()
{
   mPlatformPtr->RemoveAttachment(mTraceLineUniqueId);
}

/*************************************************************************************************
 * @brief   Sets the traceline attachment's visibility.
 * @param   aVisible True to show, false to hide.
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::TracelineData::SetVisible(bool aVisible)
{
   vespa::VaAttachment* attachment = mPlatformPtr->FindAttachment(mTraceLineUniqueId);
   if (attachment != nullptr)
   {
      attachment->SetStateVisibility(aVisible);
   }
}

/*************************************************************************************************
 * @brief   Query if the traceline attachment is visible.
 * @return  True if visible, false if not.
 * @date 7/14/2017
 **************************************************************************************************/
bool RvPlatformHistory::TracelineData::IsVisible() const
{
   vespa::VaAttachment* attachment = mPlatformPtr->FindAttachment(mTraceLineUniqueId);
   if (attachment != nullptr)
   {
      return attachment->GetStateVisibility();
   }
   return false;
}

void RvPlatformHistory::TracelineData::SetColoration(const wkf::TraceStateValue& aColoration,
                                                     const std::vector<UtColor>& aColorArray)
{
   mColoration = aColoration;
   vespa::VaAttachmentTraceLine* tl =
      dynamic_cast<vespa::VaAttachmentTraceLine*>(mPlatformPtr->FindAttachment(mTraceLineUniqueId));
   if (tl != nullptr)
   {
      switch (aColoration)
      {
      case wkf::eTRACE_TEAM_COLOR:
         tl->SetStaticColor(
            wkfEnv.GetPreferenceObject<wkf::TeamVisibilityPrefObject>()->GetTeamUtColor(mPlatformPtr->GetSide()));
         break;
      case wkf::eTRACE_NAME_HASH:
         tl->SetStaticColor(wkf::util::GetHashUtColor(mPlatformPtr->GetName()));
         break;
      default:
         tl->SetStateColoration();
         tl->SetColorArray(aColorArray);
         break;
      }
      SetFullUpdateRequired(true);
   }
}

void RvPlatformHistory::TracelineData::SetLineWidth(unsigned int aWidth)
{
   mLineWidth = aWidth;

   vespa::VaAttachmentTraceLine* tl =
      dynamic_cast<vespa::VaAttachmentTraceLine*>(mPlatformPtr->FindAttachment(mTraceLineUniqueId));
   if (tl != nullptr)
   {
      tl->SetLineWidth(aWidth);
   }
}

/*************************************************************************************************
 * @brief   Updates the traceline to create new data points for where the platform had been in
 *          the provided ranges.
 * @param   aStateSet      An enum describing which state set is being used for coloration.
 * @param   aPlatformData  Data describing the recorded history for the platform.
 * @param   aPruneBefore   Timepoint where all recorded records in the tracelines before this should be removed
 * @param   aPruneAfter    Timepoint where all recorded records in the tracelines after this should be removed
 * @param   aLowRange      Timepoint range that need need datapoints added to the back of the traceline
 * @param   aHighRange     Timepoint range that need need datapoints added to the front of the traceline
 * @date 7/14/2017
 **************************************************************************************************/
void RvPlatformHistory::TracelineData::Update(wkf::TraceStateValue           aStateSet,
                                              rv::ResultPlatform*            aPlatformData,
                                              float                          aPruneBefore,
                                              float                          aPruneAfter,
                                              const std::pair<float, float>& aLowRange,
                                              const std::pair<float, float>& aHighRange)
{
   vespa::VaAttachmentTraceLine* traceLine =
      dynamic_cast<vespa::VaAttachmentTraceLine*>(mPlatformPtr->FindAttachment(mTraceLineUniqueId));
   if (traceLine != nullptr)
   {
      if (GetFullUpdateRequired())
      {
         traceLine->Reset();
      }
      aPlatformData->BeginRead();
      auto stateData = mStateSets.find(aStateSet);
      auto states    = aPlatformData->GetArray<rv::MsgEntityState>();
      if (stateData != mStateSets.end() && states != nullptr)
      {
         if (aHighRange.second > aHighRange.first) // if we have later data than currently loaded
         {
            auto iter = states->FindFirstIteratorAfter(aHighRange.first);
            while (iter != states->end())
            {
               const rv::MsgEntityState& msg = static_cast<rv::MsgEntityState&>(**iter);
               // Problem here, we shouldn't be able to fail this if-test if each state has a distinct time
               // but we are failing.  Skip to avoid a duplicate point
               if ((msg.simTime() > aHighRange.first) && (msg.simTime() <= aHighRange.second))
               {
                  rv::Vec3d pos     = msg.state().locationWCS();
                  double    ecef[3] = {pos.x(), pos.y(), pos.z()};
                  traceLine->AddPointBack(msg.simTime(), ecef);
               }
               ++iter;
            }
            int  startState{StateData::NO_STATE};
            auto stateChangeMap =
               stateData->second->GetStateChanges(aHighRange.first, aHighRange.second, startState, aPlatformData);
            if (GetFullUpdateRequired() || aHighRange.first <= aPruneBefore)
            {
               traceLine->AddStateBack(aPruneBefore, startState);
            }
            for (auto& it : stateChangeMap)
            {
               traceLine->AddStateBack(it.first, it.second);
            }
         }
         if (aLowRange.second > aLowRange.first) // if we have earlier data than currently loaded
         {
            auto iter = states->FindFirstIteratorBefore(aLowRange.second);
            if (iter != states->end())
            {
               while (true)
               {
                  const rv::MsgEntityState& msg = static_cast<rv::MsgEntityState&>(**iter);
                  if (msg.simTime() < aLowRange.first)
                  {
                     break;
                  }
                  rv::Vec3d pos     = msg.state().locationWCS();
                  double    ecef[3] = {pos.x(), pos.y(), pos.z()};
                  traceLine->AddPointFront(msg.simTime(), ecef);
                  if (iter == states->begin())
                  {
                     break;
                  }
                  --iter;
               }
            }
            traceLine->PopFrontState();
            int  startState{StateData::NO_STATE};
            auto stateChangeMap = stateData->second->GetStateChanges(aLowRange.first,
                                                                     aLowRange.second,
                                                                     startState,
                                                                     aPlatformData); // this should be used ot find the
                                                                                     // behavior in the given time range.
            for (auto rit = stateChangeMap.crbegin(); rit != stateChangeMap.crend(); ++rit)
            {
               traceLine->AddStateFront(rit->first, rit->second);
            }
            traceLine->AddStateFront(aLowRange.first, startState);
         }
      }
      aPlatformData->EndRead();

      if (aPruneAfter > -1.0f)
      {
         traceLine->PruneAfter(aPruneAfter);
      }
      if (aPruneBefore > -1.0f)
      {
         traceLine->PruneBefore(aPruneBefore);
      }
   }
}

std::unique_ptr<RvPlatformHistory::StateData> RvPlatformHistory::TracelineData::PopStateSet(wkf::TraceStateValue aStateValue)
{
   auto ret = mStateSets.find(aStateValue);
   if (ret != mStateSets.end())
   {
      auto ret2 = std::move(ret->second);
      mStateSets.erase(ret);
      return ret2;
   }
   return nullptr;
}
