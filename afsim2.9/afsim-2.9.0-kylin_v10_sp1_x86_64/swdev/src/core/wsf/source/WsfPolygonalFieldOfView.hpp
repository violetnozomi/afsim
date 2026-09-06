// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2016 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFPOLYGONALFIELDOFVIEW_HPP
#define WSFPOLYGONALFIELDOFVIEW_HPP

#include "wsf_export.h"

#include <vector>

class UtInput;
class WsfArticulatedPart;
#include "WsfFieldOfView.hpp"
#include "WsfZoneDefinition.hpp"

//! A field of view with Polygonal az-el extents.
class WSF_EXPORT WsfPolygonalFieldOfView : public WsfFieldOfView
{
public:
   WsfPolygonalFieldOfView();
   WsfPolygonalFieldOfView(const WsfPolygonalFieldOfView& aSrc);
   WsfPolygonalFieldOfView& operator=(const WsfPolygonalFieldOfView&) = delete;

   bool operator==(const WsfPolygonalFieldOfView& aRhs) const;
   bool operator!=(const WsfPolygonalFieldOfView& aRhs) const;

   bool        ProcessInput(UtInput& aInput) override;
   bool        Initialize(const WsfArticulatedPart& aArticulatedPart) override;
   const char* GetScriptClassName() const override { return "WsfPolygonalFieldOfView"; }

   WsfFieldOfView* Clone() const override { return new WsfPolygonalFieldOfView(*this); }

   bool WithinFieldOfView(double aThisToTgtAz, double aThisToTgtEl) override;

   using AzElPoints = std::vector<std::pair<double, double>>;
   const AzElPoints&        GetAzElPoints() const { return mAzElPoints; }
   const WsfZoneDefinition& GetPoints() const { return mZoneDefinition; }

private:
   AzElPoints        mAzElPoints;
   WsfZoneDefinition mZoneDefinition;
};

#endif
