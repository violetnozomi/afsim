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

#ifndef WKFHDD_VIEWER_HPP
#define WKFHDD_VIEWER_HPP

#include "wkf_air_combat_common_export.h"

#include "VaViewer.hpp"

namespace HDD
{
class WKF_AIR_COMBAT_COMMON_EXPORT Viewer : public vespa::VaViewer
{
public:
   Viewer(int aViewerType = HighlightViewer, int aViewerOptions = Standard);
   Viewer(const Viewer& aSrc); // Disable copy construction.
   ~Viewer() override = default;

protected:
   int WheelUp(int aDelta) override;
   int WheelDown(int aDelta) override;
   int WheelImp(int aDelta); // why is this private in VaViewer?
};
} // namespace HDD
VA_DECLARE_OBJECT_TYPE(HDD::Viewer)

#endif // WKFHDD_VIEWER_HPP
