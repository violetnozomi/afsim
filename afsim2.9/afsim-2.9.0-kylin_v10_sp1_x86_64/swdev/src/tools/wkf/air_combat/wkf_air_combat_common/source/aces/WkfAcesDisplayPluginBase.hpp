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

#ifndef ACESDISPLAY_PLUGINBASE_HPP
#define ACESDISPLAY_PLUGINBASE_HPP

#include "wkf_air_combat_common_export.h"

#include <queue>
#include <unordered_set>

#include <qmutex.h>

#include "UtVec3.hpp"
#include "VaCallbackHolder.hpp"
#include "WkfEntity.hpp"
#include "WkfPlugin.hpp"
#include "WkfScenario.hpp"
#include "WkfVtkEnvironment.hpp"
#include "aces/WkfAcesDisplayDataContainer.hpp"
#include "aces/WkfAcesDisplayDockWidget.hpp"

namespace wkf
{
namespace AcesDisplay
{
class WKF_AIR_COMBAT_COMMON_EXPORT PluginBase : public Plugin
{
   Q_OBJECT
public:
   PluginBase(const QString& aPluginName, const size_t aUniqueID, const QFlags<Roles>& aRoles = eSECONDARY_VIEWER);

   ~PluginBase();

   /** This is what adds the "ACES View" option to the map display's
    * context menu when a platform is selected.
    * @param aMenu menu to add to.
    * @param aEntityPtr entity to show menu for.
    */
   void BuildEntityContextMenu(QMenu* aMenu, Entity* aEntityPtr) override;

   /** Callback for a platform being deleted.
    * @param aPlatformPtr platform being deleted.
    */
   void PlatformToBeDeletedCB(wkf::Platform* aPlatformPtr);

   /** Update the GUI (all ACES Displays). */
   void GuiUpdate() override;
protected slots:
   /** Slot that sets the display focus to the platform index given.  This
    * is used to update the platform selection list for the focused display.
    * @param aPlatformIndex index of the platforms ACES display to gain focus.
    */
   void SetDisplayFocus(const size_t aPlatformIndex);
signals:
   /** Set the target platform list for the ACES Display.
    * @param aPlatforms platforms to set.
    */
   void SetTargetPlatforms(const std::vector<size_t>& aPlatforms);

protected:
   /** Checks if a platform has the required SituationAwarenessProcessor.
    * Returns true if so, false otherwise.
    * @param aPlatformIndex platform to check for a SAP.
    */
   virtual bool HasSituationAwarenessProcessor(const size_t aPlatformIndex);

   /** Called when a new display is opened.
    * @param aPlatformPtr platform to handle.
    */
   virtual void NewDisplay(wkf::Platform* aPlatformPtr);

   /** Called when a display is destroyed.
    * @param aIndex index of the platform linked to the display.
    */
   virtual void RemoveDisplay(size_t aIndex);

   /** Check and update if an EntityTruth object is valid.
    * @param aEntityTruth entity to handle.
    */
   virtual bool EntityTruthValid(EntityTruth& aEntityTruth) { return aEntityTruth.entityValid; }

   /** Update entity truth data for a given list of platforms.
    * @param aTruthPlatforms entities to update data for.
    */
   void UpdateEntityTruthData(std::vector<EntityTruth>& aTruthPlatforms);

   /** Reset(clear) the list of ACES Displays. */
   void Reset();

   wkf::AcesDisplay::DataContainer        mDataContainer;     ///< Container for all ACES data.
   std::map<size_t, QPointer<DockWidget>> mDisplays;          ///< Map of all Aces Displays, keyed by platform index.
   size_t                                 mDisplayInFocus{0}; ///< Index of the display in focus
   std::map<size_t, std::vector<size_t>> mTargetPlatforms; ///< List of target platforms to show engagement data for, if present.
private:
   // Return by val so I can return an empty string
   const std::string GetPlatformComment(const size_t                                           aPlatformIndex,
                                        const std::map<size_t, std::map<double, std::string>>& aPlatformComments,
                                        const double                                           aSimTime) const;

   /** Called when the DockWidget is created and connected to the platform.
    * @param aPlatformPtr pointer to the platform being connected.
    */
   virtual DockWidget* ConnectToPlatform(wkf::Platform* aPlatformPtr);

   /** Wrapper function that calls ConnectToPlatform.  Refactor this? */
   virtual void Tether();

   vespa::VaCallbackHolder mCallbacks;           ///< Callback container.
   std::set<size_t>        mPlatformsOfInterest; ///< Set of platforms with an open ACES Display.
};
} // namespace AcesDisplay
} // namespace wkf
#endif // ACESDISPLAY_PLUGINBASE_HPP
