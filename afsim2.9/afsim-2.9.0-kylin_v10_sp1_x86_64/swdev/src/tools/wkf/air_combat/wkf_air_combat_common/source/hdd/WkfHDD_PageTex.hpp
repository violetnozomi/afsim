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

#ifndef WKFHDD_PAGETEX_HPP
#define WKFHDD_PAGETEX_HPP

#include "wkf_air_combat_common_export.h"

#include <QObject>
#include <QVector3D>

#include "VaOverlay.hpp"
#include "VaOverlayTexture.hpp"
#include "WkfHDD_Page.hpp"

namespace vespa
{
class VaViewer;
}

namespace HDD
{
/** This is a helper class for Page Nav, that renders a viewer to a texture object, and is old HDD code. */
class WKF_AIR_COMBAT_COMMON_EXPORT PageTex : public Page
{
public:
   PageTex(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   PageTex(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr, const char* aTexture, bool aSubtex = false);
   ~PageTex() override = default;

   /** Return if the page is currently visible. */
   const bool IsVisible() const override;

   /** Get the name of this page's overlay. */
   const std::string& OverlayName() const override;

   /** Remove this page's overlay from the viewer. */
   void RemoveOverlay() override;

   /** Set the position of this page.
    * @param aPosition position to set.
    */
   void SetPosition(const size_t aPosition) override;

   /** Set if this page should be shown.
    * @param aState true to show, false to hide.
    */
   void SetVisible(const bool aState) override;

protected:
   vespa::VaOverlayTexture* mOverlayTexPtr; ///< Pointer to the overlay texture.
private:
   /** Draw on this page. (Not used by PageTex but needs overriden) */
   void Draw() override{};
};
} // namespace HDD
#endif // WKFHDD_PAGETEX_HPP
