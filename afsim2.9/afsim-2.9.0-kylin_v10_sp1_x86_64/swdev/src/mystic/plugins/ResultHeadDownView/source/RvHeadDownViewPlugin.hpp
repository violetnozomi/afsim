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
#ifndef RVHEADSDOWNDISPLAYPLUGIN_HPP
#define RVHEADSDOWNDISPLAYPLUGIN_HPP

#include "RvPlugin.hpp"
#include "RvTrackDb.hpp"
#include "hdd/WkfHDD_Dockable.hpp"
#include "hdd/WkfHDD_PrefWidget.hpp"

namespace RvHeadsDownDisplay
{
class Plugin : public rv::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t& aUniqueId);
   ~Plugin() override = default;

   /** Advance time and read from the result data. This data will get pushed to the UpdateDataContainer function.
    * @param aData data to read from.
    */
   void AdvanceTimeRead(const rv::ResultData& aData) override;

   /** Adds an action to the context menu, allowing a HDD window to be opened
    * for a platform.
    * @param aMenu menu to add to.
    * @param aEntityPtr entity to add.
    */
   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;

   /** Reset(clear) the list of HDDs
    * @param aFullReset full reset if true.
    */
   void ClearScenario(bool aFullReset) override;

   /** Connect a platform to a new HDD window.
    * @param aPlatformName platform name.
    * @param aParent parenting widget for the new HDD window.
    */
   void ConnectToPlatform(const QString& aPlatformName, QWidget* aParent);

   /** Return a list of preference widgets. */
   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   /** Update the GUI (all HDDs). */
   void GuiUpdate() override;

   /** Read from the result data. This data will get pushed to the UpdateDataContainer function.
    * @param aData data to read from.
    */
   void RegularRead(const rv::ResultData& aData) override;

   /** Checks if a platform has the required SituationAwarenessProcessor.
    * Returns true if so, false otherwise.
    * @param aPlatformPtr platform to check for a SAP.
    */
   bool HasSituationAwarenessProcessor(wkf::Platform* aPlatformPtr);

private:
   /** Handler for connecting to a platform (opening a HDD window). */
   void ConnectToPlatformActionHandler();

   /** Release a platform, closing it's HDD window.
    * @param aPlatformName name of the platform to release.
    */
   void ReleasePlatform(const QString& aPlatformName);

   /** Reset(clear) the list of HDDs. */
   void Reset();

   /** Update the data container for all connected HDDs.
    * @param aData data to read from.
    */
   void UpdateDataContainer(const rv::ResultData& aData);

   /** Check if a platform has SA Data (Does it have an SA Processor?)
    * @param aPlatform platform to check.
    * @param aSimTime time to check at.
    */
   const bool HasSA_Data(rv::ResultPlatform* aPlatform, const double aSimTime);

   HDD::DataContainer                         mDataContainer; ///< Container for all HDD data.
   std::map<QString, QPointer<HDD::Dockable>> mHdds;          ///< Mapping of platforms to HDD windows.
   const HDD::PrefObject*                     mPrefObjectPtr; ///< Pointer to the preference object for this plugin.
   PluginUiPointer<HDD::PrefWidget>           mPrefWidgetPtr; ///< Pointer to the preference widget for this plugin.
   bool                                       mInitialized;   ///< True if the db has data, false otherwise.
};
} // namespace RvHeadsDownDisplay
#endif // RVHEADSDOWNDISPLAYPLUGIN_HPP
