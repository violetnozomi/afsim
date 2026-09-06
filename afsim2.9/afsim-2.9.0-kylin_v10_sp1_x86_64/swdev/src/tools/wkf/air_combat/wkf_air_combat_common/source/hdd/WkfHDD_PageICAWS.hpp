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

#ifndef WKFHDD_PAGEICAWS_HPP
#define WKFHDD_PAGEICAWS_HPP

#include "wkf_air_combat_common_export.h"

#include "WkfHDD_Page.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT PageICAWS : public Page
{
public:
   struct ICAWS_Data
   {
      bool engineFire{false};  ///< True if the engine is on fire, false otherwise.
      bool fuelLow{false};     ///< True if fuel is low, false otherwise.
      bool stalling{true};     ///< True if aircraft is stalling, false otherwise.
      bool icawsFailure{true}; ///< True if the icaws system is failing, false otherwise.
   };

   PageICAWS(vespa::VaViewer* aViewerPtr, const size_t aPosition, QObject* aParentPtr);
   ~PageICAWS() override = default;

private:
   /** Draw this page. */
   void Draw() override;

   /* Draw the no weapon selected box.
    * @param aVectorGroup group to add geometry to.
    * @param aText text to draw in the box.
    * @param aX x position to draw at.
    * @param aY y position to draw at.
    * @param aW width of the box.
    * @param aY height of the box.
    * @param aScale scale of the drawing.
    * @param aState state of the box (true/false).
    */
   void DrawBoxWithText(osg::ref_ptr<osg::Group> aVectorGroup,
                        const std::string&       aText,
                        const double             aX,
                        const double             aY,
                        const double             aW,
                        const double             aH,
                        const double             aScale,
                        const bool               aState);

   ICAWS_Data mData; ///< Data for this page.
};
} // namespace HDD
#endif // WKFHDD_PAGEICAWS_HPP
