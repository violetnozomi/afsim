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

#ifndef WKFHDD_DOCKABLE_HPP
#define WKFHDD_DOCKABLE_HPP

#include "wkf_air_combat_common_export.h"

#include <QDialog>
#include <QDockWidget>
#include <QVector3D>

#include "UtMemory.hpp"
#include "WkfDockWidget.hpp"
#include "hdd/WkfHDD_ArrowButton.hpp"
#include "hdd/WkfHDD_DataContainer.hpp"
#include "hdd/WkfHDD_MenuButton.hpp"

namespace vespa
{
class VaEntity;
}

namespace wkf
{
class Platform;
}

namespace HDD
{
class TabButton;
class Overlay;
class Page;
class PageEngine;
class PageHeader;
class PageTWD;
class PageTactical;
class PageHorizon;
class PageHUD;
class PageFUEL;
class PageSMS;
class Viewer;
class WKF_AIR_COMBAT_COMMON_EXPORT Dockable : public wkf::DockWidget
{
   Q_OBJECT

   friend class PageMenu; // Friend here so that PageMenu can use a private function for SetButton1ClickAction.

public:
   Dockable(vespa::VaEntity* aEntityPtr,
            int              aWidth,
            int              aHeight,
            QStringList      aPages,
            QMainWindow*     aParent = nullptr,
            Qt::WindowFlags  aFlags  = Qt::WindowFlags());

   ~Dockable() override;

   /** Draw function for menu buttons.
    * @param aPosition position of the menu with the button clicked.
    * @param aPageName name of the button clicked.
    */
   void MenuPageSelected(const size_t aPosition, const std::string& aPageName);

   /** Return the name of the entity for this window. */
   QString GetName() const;

   /** Move all of the buttons to the front of the HDD overlay. */
   void MoveButtonsToFront();

   /** Set the track processor data for this HDD window.  Any page that needs
    * data from a specific track processor will get it here.
    * @param aData track processor list data to use.
    * @param aESM_TrackProcessor name of ESM/RWR track processor (optional).
    */
   void SetTrackProcessorData(const std::vector<SimData::TrackProcessor>& aData, const std::string aESM_TrackProcessor);

   /** Set the flight controls data for this HDD window.  Any page that needs
    * flight controls data will get it here.
    * @param aData flight controls data to use.
    */
   void SetFlightControlsData(const SimData::FlightControlsData& aData);

   /** Set the nav data for this HDD window.  Any page that needs
    * nav data will get it here.
    * @param aData nav data to use.
    */
   void SetNavData(const SimData::NavData& aData);

   /** Set the weapon data for this HDD window.  Any page that needs
    * weapons data will get it here.
    * @param aData weapon data to use.
    */
   void SetWeaponData(const SimData::WeaponDataSummary& aData);

   /** Set the fuel data for this HDD window.  Any page that needs
    * fuel data will get it here.
    * @param aData fuel data to use.
    */
   void SetFuelData(const SimData::FuelData& aData);

   /** Set the platform data for this HDD window.  Any page that needs
    * platform data will get it here.
    * @param aData platform data to use.
    */
   void SetPlatformData(const SimData::PlatformData& aData);

   /** Set the master track data for this HDD window.  Any page that needs
    * master track data will get it here.
    * @param aData track data to use.
    */
   void SetTrackData(const SimData::TargetTrackItemList& aData);

   /** Set the assets list data for this HDD window.  Any page that needs
    * assets list data will get it here.
    * @param aData assets list data to use.
    */
   void SetAssetsListData(const SimData::AssetsList& aData);

   /** This sets if the connection to the overlay has been destroyed/ lost (draw x over screen).
    * @param aState state to set.
    */
   void SetConnectionDestroyed(const bool aState);

   void closeEvent(QCloseEvent* aEvent) override { emit HDD_Closed(); }
signals:
   void HDD_Closed();

private:
   /** Set the visibility of a page based on if
    * there is another page in front of it.
    * @param aPosition position of the page.
    */
   void SetPageVisibility(const size_t aPosition);

   /** Set the visibility of the tabs and buttons in a section
    * based on the page position given.
    * @param aPosition position of the page.
    * @param aState visibility state to set.
    */
   void SetSectionButtonVisibility(const size_t aPosition, const bool aState);

   /** Add a new page to the current slot.  If the page
    * already exists, move it and replace that slot with
    * a blank page.
    * @param aPosition position to add a new page.
    * @param aPage new page to add.
    */
   void AddPage(const size_t aPosition, Page* aPage);

   /** Return a new page based on the name given.
    * @param aPageName name of the new page type to create.
    * @param aPosition position of the new page.
    */
   Page* NewPage(const std::string& aPageName, const size_t aPosition);

   /** Delete a page based on the name given.
    * @param aPageName name of the new page type to create.
    */
   void DeletePage(const std::string& aPageName);

   /** Return a page in the page map, if found.
    * @param aName name of the page to find.
    */
   size_t FindPageWithName(const std::string& aName);

   /** Show a page.
    * @param aId id of the page to show.
    * @param aState true to show, false to hide.
    */
   void ShowPage(int aId, bool aState);

   /** Triggered when a viewer is destroyed.
    * @param aUniqueId pointer to the viewer.
    */
   void ViewerDestroyedCB(vespa::VaViewer* aUniqueId);

   /** Triggered when a page is clicked.  Small pages will swap with
    * their sections main display.
    * @param aPage page clicked on.
    */
   void PageClicked(Page* aPage);

   /** Swap two pages positions.
    * @param aPosition1 position of page 1.
    * @param aPosition2 position of page 2.
    */
   void SwapPages(const size_t aPosition1, const size_t aPosition2);

   /** Expand a page.
    * @param aItem page to expand.
    */
   void Expand(int aItem);

   /** Retract a page.
    * @param aItem page to retract.
    */
   void Retract(int aItem);

   /** Push a button, and return if successful.
    * @param aButton button pressed.
    */
   bool PushButton(int aButton);

   /** Push a menu button, opening the menu to pick pages.
    * @param aButton button pressed.
    */
   void PushMenuButton(int aButton);

   /** Build the context menu entry for the HDD window.
    * @param aMenu menu to add to.
    * @param aPos position to add the entry.
    */
   bool BuildContextMenu(QMenu& aMenu, const QPoint& aPos) override;

   /** Handle a scene graph action. */
   void SceneGraphAction();

   /** Triggered when a tab is clicked.
    * @param aTab tab clicked on.
    */
   void TabClicked(std::shared_ptr<TabButton> aTab);

   /** Get the menu button relating to a page position. This can return 1-4.
    * @param aPosition position of the page.
    */
   size_t GetMenuButton(const size_t aPosition);

   /** Get the section of a page using it's position.  This can return 1-4.
    * @param aPosition position of the page.
    */
   size_t GetPageSection(const size_t aPosition);

   // Viewer and entity
   // VaViewer must be a raw pointer.
   Viewer* mViewerPtr{
      nullptr}; ///< Pointer to the viewer for the HDD window. This holds all of the graphics for the HDD window.
   vespa::VaEntity* mEntityPtr{nullptr};    ///< Pointer to the entity of the HDD window.
   std::string      mConnectedPlatformName; ///< Name of the connected platform.

   // Buttons/tabs/overlays owned by the window.
   std::map<size_t, ArrowButton>                mButtons;     ///< Map of buttons for the HDD window.
   std::map<size_t, MenuButton>                 mMenuButtons; ///< Map of menu buttons for the HDD window.
   std::map<size_t, std::shared_ptr<TabButton>> mTabs;        ///< Map of tabs for the HDD window.
   std::map<size_t, std::shared_ptr<TabButton>> mActiveTab;   ///< Map of active tabs for regions 1-4.

   // Container for the 12 pages
   std::map<size_t, std::unique_ptr<Page>> mPageMap; ///< Map of HDD pages. This holds all 12 main pages.

   // These pointers are for linking to update functions (SetTrackData, SetPlatformData, etc..)
   std::unique_ptr<PageHeader>     mHeader; ///< Pointer to the header page (bar across the top of the HDD)
   std::map<size_t, PageTactical*> mTSDs;   ///< Map of TSDs (1, 2, 3)
   PageHorizon*                    mHorizonDisplay{nullptr}; ///< Pointer to the horizon display.
   PageTWD*                        mTWD{nullptr};            ///< Pointer to the TWD.
   PageHUD*                        mHUD{nullptr};            ///< Pointer to the HUD.
   PageFUEL*                       mFUEL{nullptr};           ///< Pointer to the FUEL display.
   PageSMS*                        mSMS{nullptr};            ///< Pointer to the SMS display.
   PageEngine*                     mENG{nullptr};            ///< Pointer to the ENG display.
   Overlay*                        mScreenOverlayPtr{
      nullptr}; ///< Pointer to the screen overlay, used for drawing when the connection has been destroyed/lost.
};
} // namespace HDD

#endif // WKFHDD_DOCKABLE_HPP
