// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef RVPLUGIN_SA_DISPLAY_HPP
#define RVPLUGIN_SA_DISPLAY_HPP

#include <set>

#include "RvAirCombatEventPipeClasses.hpp"
#include "RvPlugin.hpp"
#include "RvTrackDb.hpp"
#include "sa_display/WkfSA_DisplayPluginBase.hpp"

namespace rv
{
class MsgSA_AssetPerceptions;
}

namespace RvSA_Display
{
class Plugin : public rv::PluginT<wkf::SA_Display::PluginBase>
{
public:
   Plugin(const QString& aPluginName, const size_t aUniqueId);

protected slots:

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

   /** Reset(clear) the list of SA_Displays
    * @param aFullReset full reset if true.
    */
   void ClearScenario(bool aFullReset) override;

protected:
   /**
    * Called when a new Situation Awareness Display is opened.
    * @param aPlatformPtr  The platform for which the new SA display was opened.
    */
   virtual void NewDisplay(wkf::Platform* aPlatformPtr);

   /** Called when a Situation Awareness Display is destroyed.
    * @param aIndex index of the platform linked to the display.
    */
   virtual void RemoveDisplay(size_t aIndex);

   /** Checks if a platform has the required SituationAwarenessProcessor.
    * Returns true if so, false otherwise.
    * @param aPlatformPtr platform to check for a SAP.
    */
   bool HasSituationAwarenessProcessor(wkf::Platform* aPlatformPtr) override;

private:
   /** Populate an EntityPerception container using the event pipe equivalent.
    * @param aEntityPerception perception container to fill.
    * @param aData data to fill with.
    */
   void PopulateEntityPerception(wkf::SA_Display::EntityPerception& aEntityPerception,
                                 const rv::SA_EntityPerception&     aData);

   /** Update the data container for all connected SA Displays.
    * @param aData data to read from.
    */
   void UpdateDataContainer(const rv::ResultData& aData);

   /** Check if a platform has SA Data (Does it have an SA Processor?)
    * @param aPlatform platform to check.
    * @param aSimTime time to check at.
    */
   const bool HasSA_Data(rv::ResultPlatform* aPlatform, const double aSimTime);

   std::set<size_t> mPlatformsOfInterest; ///< Set of platforms with an open SA Display.
};
} // namespace RvSA_Display
#endif // RVPLUGIN_SA_DISPLAY_HPP
