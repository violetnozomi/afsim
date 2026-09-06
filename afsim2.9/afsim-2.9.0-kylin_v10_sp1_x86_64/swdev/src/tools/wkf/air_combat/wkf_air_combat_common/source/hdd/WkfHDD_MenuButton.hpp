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

#ifndef WKFHDD_MENUBUTTON_HPP
#define WKFHDD_MENUBUTTON_HPP

#include "wkf_air_combat_common_export.h"

#include <QPointer>

#include "hdd/WkfHDD_Button.hpp"
#include "hdd/WkfHDD_PageMenu.hpp"

namespace vespa
{
class VaViewer;
}

namespace HDD
{
class Page;
class WKF_AIR_COMBAT_COMMON_EXPORT MenuButton : public Button
{
public:
   explicit MenuButton(const std::string& aOverlayName,
                       Page*              aPagePtr,
                       vespa::VaViewer*   aViewerPtr,
                       PageMenu*          aPageMenuPtr,
                       const QRectF&      aRect = QRect(0, 0, 0, 0));
   ~MenuButton() = default;

   /** Draw this page. */
   virtual void Draw();

   /** Set the visibility of this button's page.
    * @param aState state to set.
    */
   void SetPageVisible(const bool aState);

   /** Set the visibility of the menu page.
    * @param aState state to set.
    */
   void SetMenuVisible(const bool aState);

   /** Set the position of the menu page.  This can be 1-4.
    * @param aPosition position to set.
    */
   void SetMenuPosition(const size_t aPosition);

   /** Return a pointer to the page. */
   Page* GetPage() const { return mPagePtr; }

   /** Set the page for this menu button.
    * @param aPagePtr pointer to the page.
    */
   void SetPage(Page* aPagePtr) { mPagePtr = aPagePtr; }

   /** Toggle the menu state, returning the new state. */
   bool ToggleMenu() { return mMenuOpen = !mMenuOpen; }

   /** Return if the menu is open for this button. */
   const bool IsMenuOpen() const { return mMenuOpen; }

private:
   Page*              mPagePtr;     ///< Pointer to the page under this menu button.
   QPointer<PageMenu> mPageMenuPtr; ///< Pointer to the menu page for this menu button.
   bool mMenuOpen; ///< True if the menu is open, false otherwise.  This will change the text to MENU POPUP.
};
} // namespace HDD
#endif // WKFHDD_MENUBUTTON_HPP
