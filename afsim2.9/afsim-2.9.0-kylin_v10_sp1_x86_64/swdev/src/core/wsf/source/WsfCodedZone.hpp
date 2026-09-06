// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef WSFCODEDZONE_HPP
#define WSFCODEDZONE_HPP

#include "wsf_export.h"

#include "UtSpan.hpp"
#include "WsfZoneDefinition.hpp"

//! A class which removes some encapsulation to enable hand-creating a Zone in C++ code.
//! This implementation DOES ASSUME a Polygonal Zone defined of clockwise (Lat, Lon) points.

class WSF_EXPORT WsfCodedZone : public WsfZoneDefinition
{
public:
   WsfCodedZone() = default;
   WsfCodedZone(ut::span<const Point> aLL_Points);
   ~WsfCodedZone() override = default;

   WsfZone* Clone() const override;

   bool ProcessInput(UtInput& aInput) override;

protected:
   //! Copy constructor (for Clone()).
   WsfCodedZone(const WsfCodedZone& aSrc) = default;
};

#endif
