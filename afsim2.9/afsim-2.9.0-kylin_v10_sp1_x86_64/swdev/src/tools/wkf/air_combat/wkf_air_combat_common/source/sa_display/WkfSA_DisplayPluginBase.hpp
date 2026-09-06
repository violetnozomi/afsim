// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2020 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef SA_DISPLAY_PLUGINBASE_HPP
#define SA_DISPLAY_PLUGINBASE_HPP

#include "wkf_air_combat_common_export.h"

#include <queue>
#include <unordered_set>

#include <qmutex.h>

#include "UtVec3.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfEntity.hpp"
#include "WkfPlugin.hpp"
#include "sa_display/WkfSA_DisplayDataContainer.hpp"
#include "sa_display/WkfSA_DisplayDockWidget.hpp"

namespace wkf
{
namespace SA_Display
{
class WKF_AIR_COMBAT_COMMON_EXPORT PluginBase : public Plugin
{
   Q_OBJECT
public:
   PluginBase(const QString& aPluginName, const size_t aUniqueID, const QFlags<Roles>& aRoles = eSECONDARY_VIEWER);

   ~PluginBase();

   /** This is what adds the "SA View" option to the map display's
    * context menu when a platform is selected.
    * @param aMenu menu to add to.
    * @param aEntityPtr entity to show menu for.
    */
   void BuildEntityContextMenu(QMenu* aMenu, Entity* aEntityPtr) override;

   /** This is called when a platform is added. The mPlatformTetheredOnStartList
    * feature probably won't be used in this plugin, and the existence of this
    * function so far seems to be predicated upon that feature, so this function
    * may be removed later.
    * @param aPlatformPtr platform being added.
    */
   void PlatformsAddedCB(const wkf::PlatformList& aPlatforms);

   /** Callback for a platform being deleted.
    * @param aPlatformPtr platform being deleted.
    */
   void PlatformToBeDeletedCB(wkf::Platform* aPlatformPtr);

   /** Get the action list. */
   QList<Action*> GetActions() const override { return mActions; }

   /** Update the GUI (all SA Displays). */
   void GuiUpdate() override;

protected:
   /** Checks if a platform has the required SituationAwarenessProcessor.
    * Returns true if so, false otherwise.
    * @param aPlatformPtr platform to check for a SAP.
    */
   virtual bool HasSituationAwarenessProcessor(wkf::Platform* aPlatformPtr) { return true; }

   /** Called when a new display is opened.
    * @param aPlatformPtr platform to handle.
    */
   virtual void NewDisplay(wkf::Platform* aPlatformPtr) {}

   /** Called when a display is destroyed.
    * @param aIndex index of the platform linked to the display.
    */
   virtual void RemoveDisplay(size_t aIndex) {}

   /** Reset(clear) the list of SA Displays. */
   void Reset();

   wkf::SA_Display::DataContainer         mDataContainer; ///< Container for all SA data.
   std::map<size_t, QPointer<DockWidget>> mSA_Displays;   ///< Map of all SA Displays, keyed by platform index.
private:
   /** Called when the DockWidget is created and connected to the platform.
    * @param aIndex index of the platform being connected.
    * @param aPlatformName name of the platform being connected.
    */
   virtual DockWidget* ConnectToPlatform(size_t aIndex, const QString& aPlatformName);

   /** Wrapper function that calls ConnectToPlatform.  Refactor this? */
   virtual void Tether();

   // Not sure what this does, but it's related to the mPlatformTetheredOnStartList
   // feature, so it probably will get cut.
   struct Hasher
   {
      size_t operator()(const QString& key) const noexcept { return std::hash<std::string>()(key.toStdString()); }
   };

   std::unordered_set<QString, Hasher> mPlatformTetheredOnStartList; ///< Tethered on start list.
   vespa::VaCallbackHolder             mCallbacks;                   ///< Callback container.
   QList<Action*>                      mActions;                     ///< Action container.
};
} // namespace SA_Display
} // namespace wkf
#endif // SA_DISPLAY_PLUGINBASE_HPP
