// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2021 Radiance Technologies. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef UT_COORDS_HPP
#define UT_COORDS_HPP

#include "UtVec3.hpp"

namespace ut
{
namespace coords
{
//! Vocabulary type representing a triple of latitude, longitude, and altitude.
//! Use a POD struct since we don't need complex features for this type.
struct UT_EXPORT LLA
{
   double mLat;
   double mLon;
   double mAlt;

   bool operator==(const LLA& aOther) const;
};

//! Vocabulary type representing a tupple of latitude, and longitude.
//! Use a POD struct since we don't need complex features for this type.
struct UT_EXPORT LL
{
   double mLat;
   double mLon;
};

struct UT_EXPORT Aspect
{
   double mAz;
   double mEl;
};

struct UT_EXPORT EulerAngles
{
   double mPsi;
   double mTheta;
   double mPhi;
};

namespace detail
{
struct WCSTag
{
};

struct ECITag
{
};

struct NEDTag
{
};

struct ENUTag
{
};

struct ECSTag
{
};

struct PCSTag
{
};

struct RSCSTag
{
};
} // namespace detail

using WCS  = ut::Vec3<double, detail::WCSTag>;
using ECEF = WCS;

using ECI = ut::Vec3<double, detail::ECITag>;

using NED = ut::Vec3<double, detail::NEDTag>;

using ENU = ut::Vec3<double, detail::ENUTag>;

using ECS = ut::Vec3<double, detail::ECSTag>;

using PCS = ut::Vec3<double, detail::PCSTag>;

using PCS = PCS;

using RSCS = ut::Vec3<double, detail::RSCSTag>;

} // namespace coords
} // namespace ut

#endif //! UT_COORDS_HPP
