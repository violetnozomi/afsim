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

#ifndef WKFHDD_PAGEMENU_HPP
#define WKFHDD_PAGEMENU_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_Page.hpp"
#include "WkfHDD_TextButton.hpp"

namespace HDD
{
class Dockable;
class WKF_AIR_COMBAT_COMMON_EXPORT PageMenu : public Page
{
public:
   explicit PageMenu(vespa::VaViewer* aViewerPtr, const size_t aPosition, Dockable* aParentPtr);
   ~PageMenu() override;

   /** Set the position of this page.
    * @param aPosition position to set.
    */
   void SetPosition(const size_t aPosition) override;

   /** Set if this page should be shown.
    * @param aState true to show, false to hide.
    */
   void SetVisible(const bool aState) override;

private:
   /** Draw this page.  This will show a button for each
    * page type to choose from.
    */
   void Draw() override;

   std::map<size_t, TextButton> mButtonMap; ///< Map of all buttons for the menu page.
};
} // namespace HDD
#endif // WKFHDD_PAGEMENU_HPP
