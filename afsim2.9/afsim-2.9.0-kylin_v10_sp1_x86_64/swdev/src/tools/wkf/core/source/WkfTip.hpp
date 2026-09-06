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
#ifndef WKFTIP_HPP
#define WKFTIP_HPP

#include "wkf_export.h"

#include <QString>

namespace wkf
{
struct WKF_EXPORT Tip
{
   Tip() = default;

   Tip(const QString& aTitle, const QString& aText, const QString& aDocLocation, const QString& aDocDescription)
      : title(aTitle)
      , text(aText)
      , docLocation(aDocLocation)
      , docDescription(aDocDescription)
   {
   }

   QString title;
   QString text;
   QString docLocation;
   QString docDescription;
};
} // namespace wkf

#endif
