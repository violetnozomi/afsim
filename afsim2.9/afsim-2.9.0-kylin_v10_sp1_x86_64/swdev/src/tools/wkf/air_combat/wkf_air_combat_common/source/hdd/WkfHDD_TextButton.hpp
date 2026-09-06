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

#ifndef WKFHDD_TEXTBUTTON_HPP
#define WKFHDD_TEXTBUTTON_HPP

#include "wkf_air_combat_common_export.h"

#include <QRect>

#include "hdd/WkfHDD_Button.hpp"

namespace vespa
{
class VaViewer;
}

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT TextButton : public Button
{
public:
   explicit TextButton(const std::string& aOverlayName,
                       const std::string& aText,
                       vespa::VaViewer*   aViewerPtr,
                       const QRectF&      aRect = QRect(0, 0, 0, 0));
   ~TextButton() = default;

   /** Draw the text button. */
   void Draw() override;

   /** Return the text for this button. */
   const std::string& GetText() const { return mText; }

private:
   std::string mText; ///< Text rendered by this button.
};
} // namespace HDD
#endif // WKFHDD_TEXTBUTTON_HPP
