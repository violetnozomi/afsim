// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2019 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "VaOverlayQt.hpp"

vespa::VaOverlayQt::VaOverlayQt(const std::string& aOverlayName,
                                OverlayType        aOverlayType /*= cOVERLAY_WINDOW*/,
                                bool               aCreateUniqueName /*= false*/)
   : VaOverlay(aOverlayName, aOverlayType, aCreateUniqueName)
{
   SetType<VaOverlayQt>();
}
