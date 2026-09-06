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

#ifndef WKFHDD_PAGEHORIZON_HPP
#define WKFHDD_PAGEHORIZON_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_OverlayHorizon.hpp"
#include "WkfHDD_Page.hpp"

namespace HDD
{
/** This Page draws a horizon indicator.  This is one of the old HDD pages that hasn't been modified yet. */
class WKF_AIR_COMBAT_COMMON_EXPORT PageHorizon : public Page
{
public:
   struct EFI_Data
   {
      // EFI data goes here.
   };

   PageHorizon(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~PageHorizon() override = default;

   /** Return if the page is currently visible. */
   const bool IsVisible() const override;

   /** Return the overlay name. */
   const std::string& OverlayName() const override;

   /** Bring the overlay to the front of the viewer. */
   void BringOverlayToFront() override;

   /** Remove this page's overlay from the viewer. */
   void RemoveOverlay() override;

   /** Set the pitch and roll for this page's overlay.
    * @param aPitch Pitch to set.
    * @param aRoll Roll to set.
    */
   void SetOwnshipPitchAndRoll_deg(const float aPitch_deg, const float aRoll_deg);

   /** Set the position of this page.
    * @param aPosition position to set.
    */
   void SetPosition(const size_t aPosition) override;

   /** Show or hide this page.
    * @param aState true to show, false to hide.
    */
   void SetVisible(const bool aState) override;

private:
   /** Draw on this page. (Not used) */
   void Draw() override{};

   OverlayHorizon* mOverlayHorizonPtr; ///< Pointer to the overlay for this page.
   EFI_Data        mData;              ///< Data for this page.
};
} // namespace HDD

#endif // WKFHDD_PAGEHORIZON_HPP
