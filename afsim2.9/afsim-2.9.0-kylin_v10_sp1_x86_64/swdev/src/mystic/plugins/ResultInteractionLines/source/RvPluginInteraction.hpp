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
#ifndef RVPLUGININTERACTION_HPP
#define RVPLUGININTERACTION_HPP

#include "RvInteractionDb.hpp"
#include "RvPlugin.hpp"
#include "interaction/WkfInteractionPluginBase.hpp"

namespace wkf
{
class AttachmentInteraction;
class Platform;
} // namespace wkf

namespace RvInteraction
{
class Interface;

// Developers can add interactions from data plug-ins by doing the following (ResultDataCyber is the standard example)
// 1. Implement your derived DataExtension::GetInteractionList() [GetStateCardList for cards] to communicate the
// interaction types and default preferences to this plug-in
// 2. Implement your derived DataExtension::AddMessage() / RemoveMessage() to interact to the InteractionDB.

class Plugin : public rv::PluginT<wkf::InteractionPluginBase>
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);

   void           ResetOptionStates() override;
   void           SetPlatformOptionState(int aId, bool aState, wkf::Platform* aPlatformPtr) override;
   Qt::CheckState GetPlatformOptionState(int aId, const wkf::Platform* aPlatformPtr) const override;

   void TimeoutChanged(const double);
   void EventStackingChanged(bool aAllow);
   void ClearScenario(bool aFullReset) override;

protected:
   void AdvanceTimeRead(const rv::ResultData& aData) override;
   void RegularRead(const rv::ResultData& aData) override;

private:
   void AddInteraction(wkf::AttachmentInteraction*           aAttachmentPtr,
                       unsigned int                          aPlatformOfInterest,
                       const rv::InteractionDb::Interaction& aInteraction);
   void RemoveInteraction(wkf::AttachmentInteraction*           aAttachmentPtr,
                          unsigned int                          aPlatformOfInterest,
                          const rv::InteractionDb::Interaction& aInteraction);

   float GetLastUpdate(const std::map<unsigned int, float>& aMap, const unsigned int aId) const;

   void PluginsLoaded() override;

   using InteractionMap = std::map<unsigned int, wkf::AttachmentInteraction*>;

   InteractionMap mInteractionMap;
   float          mTimeout;
   bool           mTimeoutGrew;
   float          mLastTime;

   std::map<unsigned int, float> mLastUpdateMap;
   std::map<unsigned int, float> mLastOneTimeUpdateMap;
   std::map<unsigned int, float> mLastUnpairedStartMap;
   std::map<unsigned int, float> mLastUnpairedEndMap;
};
} // namespace RvInteraction
#endif
