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

#ifndef WKFHDD_ARROWBUTTON_HPP
#define WKFHDD_ARROWBUTTON_HPP

#include "wkf_air_combat_common_export.h"

#include <QRect>

#include "hdd/WkfHDD_Button.hpp"

namespace vespa
{
class VaViewer;
}

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT ArrowButton : public Button
{
public:
   enum ArrowDir
   {
      Up = 0,
      Right,
      Down,
      Left
   };

   explicit ArrowButton(const std::string& aOverlayName,
                        const ArrowDir     aDirection,
                        vespa::VaViewer*   aViewerPtr,
                        const QRectF&      aRect = QRect(0, 0, 0, 0));
   ~ArrowButton() = default;

   /** Draw the arrow button. */
   void Draw() override;

   /** Set the direction of the arrow.
    * @param aDirection direction to set.
    */
   void SetDirection(const ArrowDir aDirection) { mDirection = aDirection; }

private:
   ArrowDir mDirection; ///< Direction of the arrow to draw.
};
} // namespace HDD
#endif // WKFHDD_ARROWBUTTON_HPP
