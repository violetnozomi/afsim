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

#ifndef WSFCOVERAGEGRIDPOINT_HPP
#define WSFCOVERAGEGRIDPOINT_HPP

#include "wsf_coverage_export.h"

#include <cstdint>

#include "UtLLAPos.hpp"
#include "UtVec3.hpp"

namespace wsf
{
namespace coverage
{

class Grid;

//! PointId is used as an index for grid points.
//!
//! Points in a given Grid will have an identifier that is unique
//! among the points in the same Grid.
using PointId = std::uint64_t;

//! Zero is used everywhere to indicate an invalid point Id.
constexpr PointId cINVALID_POINT_ID{0};

//! A point in a coverage grid.
class WSF_COVERAGE_EXPORT Point
{
public:
   Point(PointId aPointId, double aLatitudeDeg, double aLongitudeDeg, double aAltitude);
   Point(const Point& aOther) = default;
   Point(Point&& aOther)      = default;
   ~Point()                   = default;

   Point& operator=(const Point& aOther) = default;
   Point& operator=(Point&& aOther) = default;

   //! Return the ID of the Point.
   PointId GetID() const { return mId; }

   //! Return the latitude in degrees of the point.
   double GetLatitude() const { return mLocationLLA.GetLat(); }

   //! Return the longitude in degrees of the point.
   double GetLongitude() const { return mLocationLLA.GetLon(); }

   //! Return the altitude in meters of the point.
   double GetAltitude() const { return mLocationLLA.GetAlt(); }

   //! Return the point's LLA position.
   const UtLLAPos& GetLLA() const { return mLocationLLA; }

   void SetLLA(double aLatitudeDeg, double aLongitudeDeg, double aAltitude);
   void SetLLA(const UtLLAPos& aLocationLLA);

private:
   PointId  mId;          //! The unique ID of the point in its grid.
   UtLLAPos mLocationLLA; //! The location of the grid point.
};

} // namespace coverage
} // namespace wsf

#endif // WSFCOVERAGEGRIDPOINT_HPP
