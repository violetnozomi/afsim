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

#ifndef WKFHDD_PAGENAV_HPP
#define WKFHDD_PAGENAV_HPP

#include "wkf_air_combat_common_export.h"

#include <QObject>
#include <QVector3D>

#include "WkfHDD_DataContainer.hpp"
#include "WkfHDD_PageTex.hpp"
#include "hdd/WkfHDD_Overlay.hpp"

class QString;

namespace vespa
{
class VaEntity;
class VaViewer;
} // namespace vespa

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageNav : public PageTex
{
public:
   /** This is an old HDD page that hasn't been modified.  For some reason if you don't include this as one of the
    * pages in the HDD, osg state sets will mess up and all draw code will be aliased and thicker.  Thus, this page
    * is required for everything to look normal.
    */
   PageNav(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr, vespa::VaEntity* aEntityPtr);
   ~PageNav() override = default;

   /** Handle a mouse wheel action. *
    * @param aX X position of the mouse.
    * @param aY Y position of the mouse.
    * @param aDelta how much the mouse wheel has changed.
    */
   void MouseWheel(int aX, int aY, int aDelta);

private:
   /** Draw function for this page. */
   void Draw();

   /** Draw a triangle in the middle of the nav window. */
   void DrawTriangle();

   vespa::VaViewer* mNavViewerPtr; ///< Pointer to the viewer used in this page.
   vespa::VaEntity* mEntityPtr;    ///< Pointer to the entity to view.
   float            mZoom;         ///< Zoom factor.
};
} // namespace HDD
#endif // WKFHDD_PAGENAV_HPP
