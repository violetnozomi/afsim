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

#ifndef WKFHDD_PAGEFUEL_HPP
#define WKFHDD_PAGEFUEL_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_Page.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageFUEL : public Page
{
public:
   struct FuelData
   {
      // Bars
      double fuelFlow_pph{0.0};
      double fuelCapInt_lbs{0.0};
      double fuelCapExt_lbs{0.0};
      double fuelInternal_lbs{0.0};
      double fuelExternal_lbs{0.0};
      double joker_lbs{0.0};
      double bingo_lbs{0.0};
      double ktas{0.0};
      bool   fuelValid{false};
   };

   PageFUEL(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~PageFUEL() override = default;

   /** Set the fuel data for this page.
    * @param aInput fuel data to pull from.
    */
   void SetFuelData(const HDD::SimData::FuelData& aData);

   /** Set the ktas so distance can be calculated.
    * @param aInput ktas for the platform.
    */
   void SetKTAS(const double aKTAS) { mData.ktas = aKTAS; }

private:
   /** Draw this page. */
   void Draw() override;

   /** Draw a bar at a position, with text above, and filled to a certain percentage.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the bar.
    * @param aH height of the bar.
    * @param aScale scale to use.
    * @param aText text to draw.
    * @param aYFill how filled to draw the bar.
    */
   void DrawFuelBar(osg::ref_ptr<osg::Group> aVectorGroup,
                    const double             aX,
                    const double             aY,
                    const double             aW,
                    const double             aH,
                    const double             aScale,
                    const std::string&       aText,
                    const double             aYFill);

   /** Draw the box containing total, internal, and external fuel
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the box.
    * @param aH height of the box.
    * @param aScale scale to use.
    */
   void DrawFuelTankBox(osg::ref_ptr<osg::Group> aVectorGroup,
                        const double             aX,
                        const double             aY,
                        const double             aW,
                        const double             aH,
                        const double             aScale);

   /** Draw the box containing fuel flow, time to empty, and distance the aircraft can fly.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the box.
    * @param aH height of the box.
    * @param aScale scale to use.
    */
   void DrawFuelFlowBox(osg::ref_ptr<osg::Group> aVectorGroup,
                        const double             aX,
                        const double             aY,
                        const double             aW,
                        const double             aH,
                        const double             aScale);

   /** Draw the joker and bingo labels.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawJokerBingo(osg::ref_ptr<osg::Group> aVectorGroup,
                       const double             aX,
                       const double             aY,
                       const double             aW,
                       const double             aH,
                       const double             aScale);

   /** Small page specific - Draw text for total, internal, and external fuel, as well as joker, bingo, and fuel flow.
    * @param aVectorGroup vector group to add geometry to.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the region.
    * @param aH height of the region.
    * @param aScale scale to use.
    */
   void DrawSmallFuelText(osg::ref_ptr<osg::Group> aVectorGroup,
                          const double             aX,
                          const double             aY,
                          const double             aW,
                          const double             aH,
                          const double             aScale);

   /** Get the time to empty fuel formatted in hours:minutes.
    * @param aMinutes the minutes to convert into a time string.
    */
   std::string GetTimeToEmptyString(double aMinutes);

   FuelData mData; ///< Data for this page.
};
} // namespace HDD
#endif // WKFHDD_PAGEFUEL_HPP
