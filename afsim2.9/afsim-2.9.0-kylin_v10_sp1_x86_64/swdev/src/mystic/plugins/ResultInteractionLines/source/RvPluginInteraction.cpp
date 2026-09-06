// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#include "RvPluginInteraction.hpp"

#include "RvEnvironment.hpp"
#include "RvInteractionDb.hpp"
#include "RvResultData.hpp"
#include "VaUtils.hpp"
#include "WkfPlatform.hpp"
#include "WkfScenario.hpp"
#include "WkfViewer.hpp"
#include "WkfVtkEnvironment.hpp"
#include "interaction/WkfAttachmentInteraction.hpp"
#include "interaction/WkfInteractionPrefWidget.hpp"

WKF_PLUGIN_DEFINE_SYMBOLS(RvInteraction::Plugin, "Interactions", "Displays event driven interactions", "mystic")

RvInteraction::Plugin::Plugin(const QString& aPluginName, const size_t aUniqueId)
   : rv::PluginT<wkf::InteractionPluginBase>(aPluginName, aUniqueId)
   , mTimeout(30.0f)
   , mTimeoutGrew(false)
   , mLastTime(0.0f)
{
   connect(GetPrefObject(), &wkf::InteractionPrefObject::TimeoutChanged, this, &RvInteraction::Plugin::TimeoutChanged);
   connect(GetPrefObject(),
           &wkf::InteractionPrefObject::StackingAllowedChanged,
           this,
           &RvInteraction::Plugin::EventStackingChanged);
}

void RvInteraction::Plugin::AdvanceTimeRead(const rv::ResultData& aData)
{
   double time = aData.GetSimTime();
   if (aData.GetDb() != nullptr)
   {
      const rv::InteractionDb* interactionDB = aData.GetDb()->LockInteractionDb();

      std::map<unsigned int, wkf::AttachmentInteraction*>::iterator jt;
      bool                                                          valid;
      rv::InteractionDb::InteractionArray::range_pair               rangeData;

      // for every interaction
      for (jt = mInteractionMap.begin(); jt != mInteractionMap.end(); ++jt)
      {
         float lastUpdate              = GetLastUpdate(mLastUpdateMap, jt->first);
         float lastOneTimeUpdate       = GetLastUpdate(mLastOneTimeUpdateMap, jt->first);
         float lastUnpairedUpdateStart = GetLastUpdate(mLastUnpairedStartMap, jt->first);
         float lastUnpairedUpdateEnd   = GetLastUpdate(mLastUnpairedEndMap, jt->first);
         auto  platformIndex           = jt->first;

         if (mTimeoutGrew)
         {
            rangeData =
               interactionDB->GetUnpairedRangeData(platformIndex, time - mTimeout, lastUnpairedUpdateStart, valid);
            if (valid)
            {
               mLastUnpairedStartMap[jt->first] = nextafterf((*rangeData.first)->mTime, 0);
               for (auto it = rangeData.first; it <= rangeData.second; ++it)
               {
                  if ((*it)->mStart)
                  {
                     AddInteraction(jt->second, platformIndex, **it);
                  }
               }
            }
         }
         // going forward in time
         if (time >= mLastTime)
         {
            rangeData = interactionDB->GetRangeData(platformIndex, lastUpdate, time, valid);
            if (valid)
            {
               mLastUpdateMap[jt->first] = nextafterf((*rangeData.second)->mTime, FLT_MAX);
               for (auto it = rangeData.first; it <= rangeData.second; ++it)
               {
                  if ((*it)->mStart)
                  {
                     AddInteraction(jt->second, platformIndex, **it);
                  }
                  else
                  {
                     RemoveInteraction(jt->second, platformIndex, **it);
                  }
               }
            }
            rangeData = interactionDB->GetOneTimeRangeData(platformIndex, lastOneTimeUpdate, time, valid);
            if (valid)
            {
               mLastOneTimeUpdateMap[jt->first] = nextafterf((*rangeData.second)->mTime, FLT_MAX);
               for (auto it = rangeData.first; it <= rangeData.second; ++it)
               {
                  if ((*it)->mStart)
                  {
                     AddInteraction(jt->second, platformIndex, **it);
                  }
                  else
                  {
                     RemoveInteraction(jt->second, platformIndex, **it);
                  }
               }
            }
            // now handle unpaired interactions
            // first look at what has happened between our last update and now to know what to add
            rangeData = interactionDB->GetUnpairedRangeData(platformIndex, lastUnpairedUpdateEnd, time, valid);
            if (valid)
            {
               mLastUnpairedEndMap[jt->first] = nextafterf((*rangeData.second)->mTime, FLT_MAX);
               for (auto it = rangeData.first; it <= rangeData.second; ++it)
               {
                  if ((*it)->mStart)
                  {
                     AddInteraction(jt->second, platformIndex, **it);
                  }
               }
            }

            // next look what is happening before the timeout to see what need to stop
            rangeData =
               interactionDB->GetUnpairedRangeData(platformIndex, lastUnpairedUpdateStart, time - mTimeout, valid);

            if (valid)
            {
               mLastUnpairedStartMap[jt->first] = nextafterf((*rangeData.second)->mTime, FLT_MAX);
               for (auto it = rangeData.first; it <= rangeData.second; ++it)
               {
                  if ((*it)->mStart)
                  {
                     RemoveInteraction(jt->second, platformIndex, **it);
                  }
               }
            }
         }
         else if (time < mLastTime)
         {
            rangeData = interactionDB->GetRangeData(platformIndex, time, lastUpdate, valid); // should be inclusive of
                                                                                             // the end on pivot
            if (valid)
            {
               mLastUpdateMap[jt->first] = nextafterf((*rangeData.first)->mTime, 0);
               for (auto it = rangeData.second; it >= rangeData.first; --it)
               {
                  if ((*it)->mStart)
                  {
                     RemoveInteraction(jt->second, platformIndex, **it);
                  }
                  else
                  {
                     AddInteraction(jt->second, platformIndex, **it);
                  }
                  if ((*it)->mIndex == (*rangeData.first)->mIndex)
                  {
                     break;
                  }
               }
            }
            rangeData =
               interactionDB->GetOneTimeRangeData(platformIndex, time, lastOneTimeUpdate, valid); // should be inclusive
                                                                                                  // of the end on pivot
            if (valid)
            {
               mLastOneTimeUpdateMap[jt->first] = nextafterf((*rangeData.first)->mTime, 0);
               for (auto it = rangeData.second; it >= rangeData.first; --it)
               {
                  if ((*it)->mStart)
                  {
                     RemoveInteraction(jt->second, platformIndex, **it);
                  }
                  else
                  {
                     AddInteraction(jt->second, platformIndex, **it);
                  }
                  if ((*it)->mIndex == (*rangeData.first)->mIndex)
                  {
                     break;
                  }
               }
            }
            rangeData =
               interactionDB->GetUnpairedRangeData(platformIndex, time - mTimeout, lastUnpairedUpdateStart, valid);
            if (valid)
            {
               mLastUnpairedStartMap[jt->first] = nextafterf((*rangeData.first)->mTime, 0);
               for (auto it = rangeData.second; it >= rangeData.first; --it)
               {
                  if ((*it)->mStart)
                  {
                     AddInteraction(jt->second, platformIndex, **it);
                  }
                  if ((*it)->mIndex == (*rangeData.first)->mIndex)
                  {
                     break;
                  }
               }
            }
            rangeData = interactionDB->GetUnpairedRangeData(platformIndex, time, lastUnpairedUpdateEnd, valid);
            if (valid)
            {
               mLastUnpairedEndMap[jt->first] = nextafterf((*rangeData.first)->mTime, 0);
               for (auto it = rangeData.second; it >= rangeData.first; --it)
               {
                  if ((*it)->mStart)
                  {
                     RemoveInteraction(jt->second, platformIndex, **it);
                  }
                  if ((*it)->mIndex == (*rangeData.first)->mIndex)
                  {
                     break;
                  }
               }
            }
         }
      }

      aData.GetDb()->UnlockInteractionDb();
   }
   mLastTime    = time;
   mTimeoutGrew = false;
}

void RvInteraction::Plugin::RegularRead(const rv::ResultData& aData)
{
   AdvanceTimeRead(aData);
}

void RvInteraction::Plugin::ResetOptionStates()
{
   mLastUpdateMap.clear();
   mLastOneTimeUpdateMap.clear();
   mLastUnpairedStartMap.clear();
   mLastUnpairedEndMap.clear();

   wkf::Scenario* scenPtr = vaEnv.GetStandardScenario();
   if (scenPtr)
   {
      const std::map<unsigned int, wkf::Platform*>& platMap = scenPtr->GetIndexPlatformMap();
      for (auto& it : platMap)
      {
         wkf::AttachmentInteraction* interactionPtr =
            dynamic_cast<wkf::AttachmentInteraction*>(it.second->FindAttachment("WkfAttachmentInteraction"));
         if (interactionPtr)
         {
            it.second->RemoveAttachment(interactionPtr->GetUniqueId());
         }
      }
   }
   mInteractionMap.clear();
}

void RvInteraction::Plugin::SetPlatformOptionState(int aId, bool aState, wkf::Platform* aPlatformPtr)
{
   if (aPlatformPtr)
   {
      int  index = aPlatformPtr->GetIndex();
      auto it    = mInteractionMap.find(index);

      if (aState)
      {
         if (it == mInteractionMap.end())
         {
            mInteractionMap[index] = vespa::make_attachment<wkf::AttachmentInteraction>(*aPlatformPtr,
                                                                                        vaEnv.GetStandardViewer(),
                                                                                        "WkfAttachmentInteraction",
                                                                                        GetPrefObject());
            AddRegisteredTypes(*mInteractionMap[index]);
            it = mInteractionMap.find(index);
            vespa::VaAttachment::LoadAttachment(*it->second);
         }
      }
      if (it != mInteractionMap.end())
      {
         std::pair<std::string, int> id = mIdLookup[aId];
         it->second->ShowInteractionOfType(id, aState); // give it a color, too
         if (aState)
         {
            it->second->SetColor(id.first, GetTypeColor(id.first));
         }
      }
   }
}

Qt::CheckState RvInteraction::Plugin::GetPlatformOptionState(int aId, const wkf::Platform* aPlatformPtr) const
{
   const auto& jt = mIdLookup.find(aId);
   if (jt != mIdLookup.end())
   {
      auto it = mInteractionMap.find(aPlatformPtr->GetIndex());
      if (it != mInteractionMap.end())
      {
         return (it->second->InteractionsOfTypeAreShown(jt->second)) ? Qt::Checked : Qt::Unchecked;
      }
   }
   return Qt::Unchecked;
}

void RvInteraction::Plugin::TimeoutChanged(const double aValue)
{
   if (mTimeout < aValue)
   {
      mTimeoutGrew = true;
      // we need to look further back and add stuff
   }
   mTimeout = aValue;
}

void RvInteraction::Plugin::EventStackingChanged(bool aAllow)
{
   for (auto&& interaction : mInteractionMap)
   {
      interaction.second->SetStackingAllowed(aAllow);
   }
}

void RvInteraction::Plugin::ClearScenario(bool aFullReset)
{
   mInteractionMap.clear(); // pointers will be cleaned up by the scenario
   mLastTime = 0.0;
   mLastUpdateMap.clear();
   mLastOneTimeUpdateMap.clear();
   mLastUnpairedStartMap.clear();
   mLastUnpairedEndMap.clear();
}

void RvInteraction::Plugin::AddInteraction(wkf::AttachmentInteraction*           aAttachmentPtr,
                                           unsigned int                          aPlatformOfInterest,
                                           const rv::InteractionDb::Interaction& aInteraction)
{
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      if (aPlatformOfInterest == aInteraction.mTarget)
      {
         vespa::VaEntity* src = scenarioPtr->FindPlatformByIndex(aInteraction.mSource);
         if (src)
         {
            aAttachmentPtr->AddInteraction(std::make_pair(aInteraction.mType, wkf::AttachmentInteraction::eINCOMING),
                                           src,
                                           aInteraction.mData,
                                           aInteraction.mId);
         }
      }
      if (aPlatformOfInterest == aInteraction.mSource)
      {
         vespa::VaEntity* tgt = scenarioPtr->FindPlatformByIndex(aInteraction.mTarget);
         if (tgt)
         {
            aAttachmentPtr->AddInteraction(std::make_pair(aInteraction.mType, wkf::AttachmentInteraction::eOUTGOING),
                                           tgt,
                                           aInteraction.mData,
                                           aInteraction.mId);
         }
      }
   }
}

void RvInteraction::Plugin::RemoveInteraction(wkf::AttachmentInteraction*           aAttachemntPtr,
                                              unsigned int                          aPlatformOfInterest,
                                              const rv::InteractionDb::Interaction& aInteraction)
{
   auto* scenarioPtr = vaEnv.GetStandardScenario();
   if (scenarioPtr)
   {
      if (aPlatformOfInterest == aInteraction.mTarget)
      {
         vespa::VaEntity* src = scenarioPtr->FindPlatformByIndex(aInteraction.mSource);
         if (src)
         {
            aAttachemntPtr->RemoveInteraction(std::make_pair(aInteraction.mType, wkf::AttachmentInteraction::eINCOMING),
                                              src,
                                              aInteraction.mId);
         }
      }
      else if (aPlatformOfInterest == aInteraction.mSource)
      {
         vespa::VaEntity* tgt = scenarioPtr->FindPlatformByIndex(aInteraction.mTarget);
         if (tgt)
         {
            aAttachemntPtr->RemoveInteraction(std::make_pair(aInteraction.mType, wkf::AttachmentInteraction::eOUTGOING),
                                              tgt,
                                              aInteraction.mId);
         }
      }
   }
}

float RvInteraction::Plugin::GetLastUpdate(const std::map<unsigned int, float>& aMap, const unsigned int aId) const
{
   auto&& value = aMap.find(aId);
   if (value != aMap.end())
   {
      return value->second;
   }
   return -1.0f;
}

void RvInteraction::Plugin::PluginsLoaded()
{
   for (auto& ext : rvEnv.GetExtensions())
   {
      for (auto& card : ext->GetStateCardList())
      {
         RegisterStateCard(card->mName, card->mIcon, card->mGroup);
      }
      for (auto& interaction : ext->GetInteractionList())
      {
         RegisterInteraction(interaction->mName, interaction->mGroup, interaction->mHoverText, interaction->mColor);
      }
   }
}
