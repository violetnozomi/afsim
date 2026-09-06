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

#ifndef WKFHDD_PAGETFLIR_HPP
#define WKFHDD_PAGETFLIR_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_Page.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageTFLIR : public Page
{
public:
   struct TFLIR_Data
   {
      // TFLIR related draw data goes here.
   };

   PageTFLIR(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~PageTFLIR() override = default;

private:
   /** Draw this page. */
   void Draw() override;

   /** Draw the TFLIR circle.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position (center of drawing).
    * @param aY y position (center of drawing).
    * @param aRadius radius of the circle.
    * @param aScale scale of drawing.
    */
   void DrawCircle(osg::ref_ptr<osg::Group> aVectorGroup,
                   const double             aX,
                   const double             aY,
                   const double             aRadius,
                   const double             aScale);

   /** Draw the TFLIR crosshair.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position (center of drawing).
    * @param aY y position (center of drawing).
    * @param aRadius radius of the circle.
    * @param aScale scale of drawing.
    */
   void DrawCrosshair(osg::ref_ptr<osg::Group> aVectorGroup,
                      const double             aX,
                      const double             aY,
                      const double             aRadius,
                      const double             aScale);

   TFLIR_Data mData; ///< Data for this page.
};
} // namespace HDD
#endif // WKFHDD_PAGETFLIR_HPP
