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

#ifndef WKFHDD_TABBUTTON_HPP
#define WKFHDD_TABBUTTON_HPP

#include "wkf_air_combat_common_export.h"

#include <QPointer>

#include "hdd/WkfHDD_Button.hpp"

namespace vespa
{
class VaViewer;
}

namespace HDD
{
class Page;
class WKF_AIR_COMBAT_COMMON_EXPORT TabButton : public Button
{
public:
   explicit TabButton(const std::string& aOverlayName, Page* aPagePtr, vespa::VaViewer* aViewerPtr, const QRectF& aRect);
   ~TabButton() = default;

   /** Draw the tab. */
   virtual void Draw();

   /** Return the position of this tab's page. */
   size_t const GetPagePosition() const;

   /** Return a pointer to the page. */
   Page* GetPage() const { return mPagePtr; }

   /** Set the page for this tab.
    * @param aPagePtr pointer to the page.
    */
   void SetPage(Page* aPagePtr) { mPagePtr = std::move(aPagePtr); }

   /** Set the visibility of this tab's page.
    * @param aState state to set.
    */
   void SetPageVisible(const bool aState);

   /** Set the position of this tab's page.
    * @param aPosition position to set.
    */
   void SetPagePosition(const size_t aPosition);

private:
   Page* mPagePtr; ///< Pointer to the page for this tab.
};
} // namespace HDD
#endif // WKFHDD_TABBUTTON_HPP
