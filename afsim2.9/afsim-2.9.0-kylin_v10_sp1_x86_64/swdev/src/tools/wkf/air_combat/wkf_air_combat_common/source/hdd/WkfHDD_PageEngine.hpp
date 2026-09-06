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

#ifndef WKFHDD_PAGEENGINE_HPP
#define WKFHDD_PAGEENGINE_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_Page.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageEngine : public Page
{
public:
   struct EngineData
   {
      double thrustNormalized{0.0}; ///< Thrust normalized value (0-2).
      double throttleInput{0.0};    ///< Throttle input value (0-2).
      bool   throttleValid{true};   ///< True if the throttle input is valid.
      double fuelFlow_pph{0.0};     ///< Fuel flow in pph.
   };

   PageEngine(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~PageEngine() override = default;

   /** Set the fuel flow in pph.
    * @param aInput fuel flow to set.
    */
   void SetFuelFlow(const double aInput) { mData.fuelFlow_pph = aInput; }

   /** Set the thrust and throttle values.
    * @param aThrust thrust value to set (0-2).
    * @param aThrottleValid true if the throttle value is valid.
    * @param aThrottle throttle value to set (0-2).
    */
   void SetThrustThrottle(const double aThrust, const bool aThrottleValid, const double aThrottle);

private:
   /** Draw this page. */
   void Draw() override;

   /** Draw a gauge at a position, with text below it.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aR radius of the gauge.
    * @param aScale scale to use.
    * @param aText text to draw.
    * @param aValue value of the hand on the gauge.
    */
   void DrawGauge(osg::ref_ptr<osg::Group> aVectorGroup,
                  const double             aX,
                  const double             aY,
                  const double             aR,
                  const double             aScale,
                  const std::string&       aText,
                  const double             aValue);

   /** Draw the fuel flow box.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawFuelFlow(osg::ref_ptr<osg::Group> aVectorGroup,
                     const double             aX,
                     const double             aY,
                     const double             aW,
                     const double             aH,
                     const double             aScale);

   /** Small page specific - Draw text for thrust, throttle, and fuel flow.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawSmallEngText(osg::ref_ptr<osg::Group> aVectorGroup,
                         const double             aX,
                         const double             aY,
                         const double             aW,
                         const double             aH,
                         const double             aScale);

   EngineData mData; ///< Data for this page.
};
} // namespace HDD
#endif // WKFHDD_PAGEENGINE_HPP
