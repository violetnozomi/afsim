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

#ifndef UTOEPHEMERISMOON_HPP
#define UTOEPHEMERISMOON_HPP

#include "utilosg_export.h"

#include <osg/PositionAttitudeTransform>

#include "UtCalendar.hpp"

namespace Uto
{
class UTILOSG_EXPORT EphemerisMoon : public osg::PositionAttitudeTransform
{
public:
   EphemerisMoon(bool aForCelestialSphere, const std::string& aResourcePath);
   ~EphemerisMoon() override = default;

   void SetTime(const UtCalendar& aTime);
};
} // namespace Uto

#endif
