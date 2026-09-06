// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVPLUGIN_ACESDISPLAY_HPP
#define RVPLUGIN_ACESDISPLAY_HPP

#include <set>

#include "RvAirCombatEventPipeClasses.hpp"
#include "RvPlugin.hpp"
#include "RvTrackDb.hpp"
#include "aces/WkfAcesDisplayPluginBase.hpp"

namespace rv
{
class MsgSA_AssetPerceptions;
}

namespace RvAcesDisplay
{
class Plugin : public rv::PluginT<wkf::AcesDisplay::PluginBase>
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);

   /** Read a platform comment and store it in platform comments for display on the ACES Display.
    * @param aData data to find the comments matching platform from.
    * @param aComment comment to read.
    */
   void CommentRead(rv::ResultDb& aData, const rv::MsgComment& aComment) override;

protected slots:
   /** Called when a platform is added.  This will be used to create
    * an EntityTruth object with data at the start.
    * @param aSimTime sim time platform is added.
    * @param aData data to read from.
    * @param aPlatform platform info to read from.
    */
   void PlatformAddedRead(double aSimTime, rv::ResultDb& aData, const rv::MsgPlatformInfo& aPlatform) override;

   /**
    * RegularRead is called on regular intervals by Results-Vis. This
    * implementation just calls AdvanceTimeRead.
    * @param aData  The data from the .aer file.
    */
   void RegularRead(const rv::ResultData& aData) override;

   /**
    * This reads new data from the .aer file and translates it into the
    * data structures required by the Situation Awareness Display.
    * @param aData  The data from the .aer file.
    */
   void AdvanceTimeRead(const rv::ResultData& aData) override;

   /** Reset(clear) the list of Aces Displays
    * @param aFullReset full reset if true.
    */
   void ClearScenario(bool aFullReset) override;

protected:
   /** Checks if a platform has the required SituationAwarenessProcessor.
    * Returns true if so, false otherwise.
    * @param aPlatformIndex platform to check for a SAP.
    */
   bool HasSituationAwarenessProcessor(const size_t aPlatformIndex) override;

   /** Check and update if an EntityTruth object is valid.
    * @param aEntityTruth entity to handle.
    */
   bool EntityTruthValid(wkf::AcesDisplay::EntityTruth& aEntityTruth) override;

private:
   /** Populate an EntityPerception container using the event pipe equivalent.
    * @param aEntityPerception perception container to fill.
    * @param aData data to fill with.
    */
   void PopulateEntityPerception(wkf::AcesDisplay::EntityPerception& aEntityPerception,
                                 const rv::SA_EntityPerception&      aData);

   /** Populate engagement data for a platform
    * @param aData data to fill with.
    * @param aTempData data container to fill.
    * @param aPlatform platform to populate data for.
    */
   void PopulateEngagementData(const rv::ResultData&             aData,
                               wkf::AcesDisplay::EngagementData& aTempData,
                               rv::ResultPlatform*               aPlatform);

   /** Populate behavior history data for a platform
    * @param aData data to fill with.
    * @param aBehaviorHistory data container to fill.
    * @param aPlatform platform to populate data for.
    */
   void PopulateBehaviorHistory(const rv::ResultData&     aData,
                                std::vector<std::string>& aBehaviorHistory,
                                rv::ResultPlatform*       aPlatform);

   /** Update the data container for all connected ACES Displays.
    * @param aData data to read from.
    */
   void UpdateDataContainer(const rv::ResultData& aData);

   /** Check if a platform has SA Data (Does it have an SA Processor?)
    * @param aPlatform platform to check.
    * @param aSimTime time to check at.
    */
   const bool HasSA_Data(rv::ResultPlatform* aPlatform, const double aSimTime);
};
} // namespace RvAcesDisplay
#endif // RVPLUGIN_ACESDISPLAY_HPP
