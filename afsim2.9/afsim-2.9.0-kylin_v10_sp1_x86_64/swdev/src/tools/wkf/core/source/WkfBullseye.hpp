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

#ifndef WkfBullseye_HPP
#define WkfBullseye_HPP

#include "wkf_export.h"

#include <set>

#include "WkfPointOfInterest.hpp"

namespace wkf
{
class WKF_EXPORT Bullseye : public PointOfInterest
{
public:
   Bullseye();
   ~Bullseye() override = default;
};

using BullseyeList = std::set<wkf::Bullseye*>;
} // namespace wkf

VA_DECLARE_OBJECT_TYPE(wkf::Bullseye)

#endif
