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

#ifndef UTOEPHEMERISNODE_HPP
#define UTOEPHEMERISNODE_HPP

#include "utilosg_export.h"

#include <osg/Group>
#include <osg/PositionAttitudeTransform>

#include "UtCalendar.hpp"
#include "UtoEphemerisMoon.hpp"
#include "UtoEphemerisStars.hpp"
#include "UtoEphemerisSun.hpp"

namespace Uto
{
class UTILOSG_EXPORT EphemerisNode : public osg::Group
{
public:
   EphemerisNode(const std::string& aResourcePath);
   ~EphemerisNode() override = default;

   void      SetTime(const UtCalendar& aTime);
   osg::Vec3 GetSunVector();

private:
   void BuildAtmosphere();
   void BuildMoon(const std::string& aResourceDir);
   void BuildSun();
   void BuildStars(const std::string& resourceDir);

   osg::ref_ptr<EphemerisMoon>     mMoon;
   osg::ref_ptr<UtoEphemerisSun>   mSun;
   osg::ref_ptr<UtoEphemerisStars> mStars;
};
} // namespace Uto

#endif
