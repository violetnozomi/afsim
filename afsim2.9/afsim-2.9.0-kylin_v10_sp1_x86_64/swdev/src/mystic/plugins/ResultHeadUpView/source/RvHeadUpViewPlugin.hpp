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
#ifndef RVHEADSUPDISPLAYPLUGIN_HPP
#define RVHEADSUPDISPLAYPLUGIN_HPP

#include "RvPlugin.hpp"
#include "hud/WkfHUD_DataContainer.hpp"
#include "hud/WkfHUD_DockWidget.hpp"
#include "hud/WkfHUD_PrefWidget.hpp"

namespace RvHeadsUpDisplay
{
class Plugin : public rv::Plugin
{
public:
   Plugin(const QString& aPluginName, const size_t& aUniqueId);
   ~Plugin() override = default;

   /** Advance time and read from the result data.
    * @param aData data to read from.
    */
   void AdvanceTimeRead(const rv::ResultData& aData) override;

   /** Adds an action to the context menu, allowing a HUD window to be opened.
    * @param aMenu menu to add to.
    * @param aEntityPtr entity to add.
    */
   void BuildEntityContextMenu(QMenu* aMenu, wkf::Entity* aEntityPtr) override;

   /** Reset(clear) the list of HUDs
    * @param aFullReset full reset if true.
    */
   void ClearScenario(bool aFullReset) override;

   /** Connect a platform to a new HUD window. */
   void ConnectToPlatform();

   /** Return a list of preference widgets. */
   QList<wkf::PrefWidget*> GetPreferencesWidgets() const override;

   /** Update the GUI (all HUDs). */
   void GuiUpdate() override;

   /** Read from the result data.
    * @param aData data to read from.
    */
   void RegularRead(const rv::ResultData& aData) override;

   /** Checks if a platform has the required SituationAwarenessProcessor.
    * Returns true if so, false otherwise.
    * @param aPlatformPtr platform to check for a SAP.
    */
   bool HasSituationAwarenessProcessor(wkf::Platform* aPlatformPtr);

private:
   /** Set the first person state.
    * @param aState true for first person, false for third person.
    */
   void FirstPerson(bool aState);

   /** Release(delete) a HUD window.
    * @param aDockWidget window to release.
    */
   void ReleasePlatform(QPointer<wkf::HUD_DockWidget> aDockWidget);

   /** Reset(clear) the list of HUDs. */
   void Reset();

   /** Set the show hud state.
    * @param aState true to set the hud visible, false to hide it.
    */
   void ShowHUD(bool aState);

   /** Update the data container for all connected HUDs.
    * @param aData data to read from.
    */
   void UpdateDataContainer(const rv::ResultData& aData);

   /** Update the data for a HUD.
    * @param aDockWidget HUD to update
    */
   void UpdateDataForHud(QPointer<wkf::HUD_DockWidget> aDockWidget);

   /** Handle post-initialization for a HUD window.
    * @param aDockWidget HUD window to post-initialize.
    */
   void ViewerInitializedHandler(QPointer<wkf::HUD_DockWidget> aDockWidget);

   /** Check if a platform has SA Data (Does it have an SA Processor?)
    * @param aPlatform platform to check.
    * @param aSimTime time to check at.
    */
   const bool HasSA_Data(rv::ResultPlatform* aPlatform, const double aSimTime);

   wkf::HUD_DataContainer                           mDataContainer; ///< Container for all HUD data.
   std::map<QString, QPointer<wkf::HUD_DockWidget>> mHuds;          ///< Mapping of platforms to HUD windows.
   const wkf::HUD_PrefObject*                       mPrefObjectPtr; ///< Pointer to the preference object.
   PluginUiPointer<wkf::HUD_PrefWidget>             mPrefWidgetPtr; ///< Pointer to the preference widget.
};
} // namespace RvHeadsUpDisplay
#endif // RVHEADSUPDISPLAYPLUGIN_HPP
