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

#ifndef WKFHDD_PAGEHUD_HPP
#define WKFHDD_PAGEHUD_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_Page.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageHUD : public Page
{
public:
   struct HUD_Data
   {
      double heading_deg{0.0}; ///< Heading in degrees.
      double ktas{0.0};        ///< KTAS.
      double altitude_ft{0.0}; ///< Altitude in ft.
   };

   PageHUD(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~PageHUD() override = default;

   /** Set the heading of this page.
    * @param aInput heading to set.
    */
   void SetHeading_deg(const double aInput) { mData.heading_deg = aInput; }

   /** Set the altitude of this page.
    * @param aInput altitude to set.
    */
   void SetAltitude_ft(const double aInput) { mData.altitude_ft = aInput; }

   /** Set the ktas of this page.
    * @param aInput ktas to set.
    */
   void SetKTAS(const double aInput) { mData.ktas = aInput; }

private:
   /** Draw this page. */
   void Draw() override;

   /* Draw a circle with dots.
    * @param aVectorGroup group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aRadius radius of the circle.
    * @param aNeedleAngleRad angle of the needle (0-1)
    * @param aDotScale scale of the osg::Points.
    * @param aScale scale of the drawing.
    */
   void DrawCircleWithDots(osg::ref_ptr<osg::Group> aVectorGroup,
                           const double             aX,
                           const double             aY,
                           const double             aRadius,
                           const double             aNeedleAngleRad,
                           const double             aDotScale,
                           const double             aScale);

   HUD_Data mData; ///< Data for this page.
};
} // namespace HDD
#endif // WKFHDD_PAGEHUD_HPP
