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

#ifndef WKFHDD_PAGEFCS_HPP
#define WKFHDD_PAGEFCS_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_Page.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageFCS : public Page
{
public:
   struct FCS_Data
   {
      osg::Vec2d centerControlPos{0.0, 0.0}; ///< Position of the diamond in the control box (x,y values from -1.0 to 1.0).
      double     bottomControlPos{0.0}; ///< Position of the diamond below the control box (x value from -1.0 to 1.0).
   };

   PageFCS(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~PageFCS() override = default;

private:
   /** Draw this page. */
   void Draw() override;

   /** Draw the control box at a position and size.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at (center of drawing)
    * @param aY y position to draw at (center of drawing)
    * @param aBoxW width of box.
    * @param aBoxH height of box.
    * @param aScale scale of drawing.
    */
   void DrawControlBox(osg::ref_ptr<osg::Group> aVectorGroup,
                       const double             aX,
                       const double             aY,
                       const double             aBoxW,
                       const double             aBoxH,
                       const double             aScale);

   /** Draw the aircraft top view.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at (center of drawing)
    * @param aY y position to draw at (center of drawing)
    * @param aScale scale of drawing.
    */
   void DrawAircraftTopView(osg::ref_ptr<osg::Group> aVectorGroup, const double aX, const double aY, const double aScale);

   /** Draw the aircraft side view.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at (center of drawing)
    * @param aY y position to draw at (center of drawing)
    * @param aW width of the drawing.
    * @param aH height of the drawing.
    * @param aScale scale of drawing.
    */
   void DrawAircraftSideView(osg::ref_ptr<osg::Group> aVectorGroup,
                             const double             aX,
                             const double             aY,
                             const double             aW,
                             const double             aH,
                             const double             aScale);

   FCS_Data mData; ///< Data for this page.
};
} // namespace HDD
#endif // WKFHDD_PAGEFCS_HPP
