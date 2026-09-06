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
// ****************************************************************************
// Updated by Infoscitex, a DCS Company.
// ****************************************************************************

#ifndef UTLINESEGMENT_HPP
#define UTLINESEGMENT_HPP

#include "ut_export.h"

/**
   This class models a line oriented in three-dimensional space, constrained by two endpoints.
   Methods are available to calculate the closest approach points between two non-parallel
   lines, and various distance values.
*/

#include "UtVec3.hpp"

class UT_EXPORT UtLineSegment
{
public:
   UtLineSegment();

   UtLineSegment(const double aBeginPoint[3], const double aEndPoint[3]);

   UtLineSegment(const UtLineSegment& aSrc);

   UtLineSegment& operator=(const UtLineSegment& aRHS);

   bool operator==(const UtLineSegment& rhs) const;

   //! Returns the shortest distance from this line segment to a Point
   /*!
    *  This algorithm finds a point along this line segment that is closest
    *  to aPoint.  The caller can optionally treat this line segment as
    *  an infinite line, or as a segment limited by the start and end points.
    *
    *  \param[in] aPoint     3-D point to measure shortest distance to
    *  \param[in] aLimitToEndpoints
    *                        If true, treat this as a line segment, else
    *                        treat as an infinite line.
    *  \return distance between the point and this line segment.
    */
   double DistanceBetween(const double aPoint[3], const bool aLimitToEndpoints = false) const;

   //! Returns the shortest distance from this line segment to another line
   //! segment
   /*!
    *  This algorithm finds a point along this line segment that is closest
    *  to the closest point on aOtherLine.  The caller can optionally treat
    *  the line segments as an infinite lines, or as segments limited by
    *  the start and end points.
    *
    *  \param[in]  aOtherLine      3-D line segment to measure distance to
    *  \param[out] aDistance       Shortest distance to the line segment.
    *  \param[in]  aLimitToEndpoints
    *                        If true, treat this as a line segment, else
    *                        treat as an infinite line.
    *  \return Always return true
    *
    *  \note An older implementation had 'invalid' cases, which are now
    *        solved for.  The bool return value was maintained for backwards
    *        compatibility.
    */
   bool DistanceBetween(const UtLineSegment& aOtherLine, double& aDistance, const bool aLimitToEndpoints = false) const;
   //! Returns the shortest distance from this line segment to another line
   //! segment
   /*!
    *  This algorithm finds a point along this line segment that is closest
    *  to the closest point on aOtherLine.  The caller can optionally treat
    *  the line segments as an infinite lines, or as segments limited by
    *  the start and end points.
    *
    *  \param[in] aOtherLine      3-D line segment to measure distance to
    *  \param[in] aLimitToEndpoints
    *                        If true, treat this as a line segment, else
    *                        treat as an infinite line.
    *  \return Shortest distance to the line segment
    */
   double DistanceBetween(const UtLineSegment& aOtherLine, const bool aLimitToEndpoints = false) const;

   //! Returns true if the point is within floating point error of being
   //! exactly on the line segment
   /*!
    *  \param[in] aPoint     3-D point to measure shortest distance to
    *  \param[in] aLimitToEndpoints
    *                        If true, treat this as a line segment, else
    *                        treat as an infinite line.
    *  \return true if the point is on the line segment
    */
   bool PointOnLineSegment(const double aPoint[3], const bool aLimitToEndpoints = true) const;

   //! Calculates the closest two points on this line segment and another
   /*!
    *  This algorithm finds a point along this line segment that is closest
    *  to the closest point on aOtherLine.  The caller can optionally treat
    *  the line segments as infinite lines, or as segments limited by the
    *  start and end points.
    *
    *  \param[in] aOtherLine      3-D line segment to measure distance to
    *  \param[out] aThisClosestPoint Closest point on this line.
    *  \param[out] aThisModulus      A value that goes from [0-1] for the
    *                                location along this segment, where
    *                                < 0 is before the start point,
    *                                0 is the start point,
    *                                > 0 && < 1 is on the line segment
    *                                and > 1 is beyond the end point.
    *  \param[out] aOtherClosestPoint Closest point on aOtherLine.
    *  \param[out] aOtherModulus      A value that goes from [0-1] for the
    *                                 location along this segment, where
    *                                 < 0 is before the start point,
    *                                 0 is the start point,
    *                                 > 0 && < 1 is on the line segment
    *                                 and > 1 is beyond the end point.
    *  \param[in] aLimitToEndpoints
    *                        If true, treat lines as line segments, else
    *                        treat as infinite lines.
    *  \return Always returns true
    *
    *  \note An older implementation had 'invalid' cases, which are now
    *        solved for.  The bool return value was maintained for backwards
    *        compatibility.
    */
   bool PointOfClosestApproach(const UtLineSegment& aOtherLine,
                               double               aThisClosestPoint[3],
                               double&              aThisModulus,
                               double               aOtherClosestPoint[3],
                               double&              aOtherModulus,
                               const bool           aLimitToEndpoints = false) const;
   //! Calculates the closest point on this line segment to another point
   /*!
    *  This algorithm finds a point along this line segment that is closest
    *  to aOtherPoint.  The caller can optionally treat this line segment as
    *  an infinite line, or as a segment limited by the start and end points.
    *
    *  \param[in] aPoint     3-D point to measure shortest distance to
    *  \param[out] aClosestPoint Closest point on this line.
    *  \param[out] aModulus      A value that goes from [0-1] for the
    *                            location along this segment, where
    *                            < 0 is before the start point,
    *                            0 is the start point,
    *                            > 0 && < 1 is on the line segment
    *                            and > 1 is beyond the end point.
    *  \param[in] aLimitToEndpoints
    *                        If true, treat this as a line segment, else
    *                        treat as an infinite line.
    *  \return Always returns true
    *
    *  \note An older implementation had 'invalid' cases, which are now
    *        solved for.  The bool return value was maintained for backwards
    *        compatibility.
    */
   bool PointOfClosestApproach(const double aOtherPoint[3],
                               double       aClosestPoint[3],
                               double&      aThisModulus,
                               const bool   aLimitToEndpoints = false) const;

   void GetBeginPoint(double aBeginLoc[]) const;

   void GetEndPoint(double aEndLoc[]) const;

   void GetPointAtModulus(double aModulus, double aPoint[3]) const;

   double MagnitudeSquared() const;

   double Magnitude() const;

   void StepTo(const double aNewLoc[3]);

   void SetBeginPoint(const double aLoc[3]);

   void SetEndPoint(const double aLoc[3]);

   /** Use Cautiously:  Points to the 3-D vector of the location of the Begin point. */
   double*       GetBeginPtr() { return mLoc1; }
   const double* GetBeginPtr() const { return mLoc1; }
   /** Use Cautiously:  Points to the 3-D vector of the location of the End point. */
   double*       GetEndPtr() { return mLoc2; }
   const double* GetEndPtr() const { return mLoc2; }

   enum LINE_CLASSIFICATION
   {
      PARALLEL,           // two lines are parallel
      LINES_INTERSECT,    // two lines intersect beyond their given endpoints
      COLLINEAR,          // the two lines overlap each other
      SEGMENTS_INTERSECT, // the segments intersect each other with their given endpoints
      A_BISECTS_B,
      B_BISECTS_A
   };

   // given two lines check for an intersection between them and return the relationship between them.
   // If the lines do intersect, return the intersection point as well
   LINE_CLASSIFICATION Intersects(const UtLineSegment* aOtherLinePtr, UtVec3d* aIntersectionPointPtr) const;

   //! Returns true the lines are 'Collinear'
   /*!
    *  If endpoint limits are not set, will return true if all four
    *  endpoints of both this line and aOtherLine are collinear.
    *  Otherwise will return true if both endpoints of the shorter of
    *  this line and aOtherLine lie on the longer of these two lines.
    */
   bool IsCollinear(const UtLineSegment& aOtherLine, const bool aLimitToEndpoints = true);

   //! Sets the Direction Vector for this class
   /*!
    *  The only use of the Direction vector is to set the Normal Vector
    *
    *  Sets the 'Direction' of the line segment class to the vector from
    *  aLocB to aLocA
    *
    *  \warn If you pass in any value for aLocA and aLocB other than
    *        GetBeginPoint() and GetEndPoint(), the direction vector has
    *        nothing to do with the endpoints of this Line Segment
    */
   void SetDirection(const double aLocA[3], const double aLocB[3]);
   //! Sets a 'Normal' vector to be returned by GetNormal()
   /*!
    *  The 'Normal' vector is a unit vector in the direction of the direction
    *  vector.
    *
    *  If the Direction vector was not set by SetDirection, then the
    *  direction of the Normal vector is from GetBeginPoint() to
    *  GetEndPoint()
    */
   void SetNormal();
   //! Returns the 'Normal' vector calculated by SetNormal()
   /*!
    *  \note The 'Normal' vector is only normal to this line if a call to
    *        SetDirection() is made with aLocB -> aLocA being normal to this
    *        line segment.
    *
    *  \warn If SetNormal() has not been set, the value of the 'Normal'
    *        is in an undefined direction with an undefined length (NOT
    *        a Unit vector!)
    */
   void GetNormal(double aNormal[]) const;

protected:
   double mLoc1[3];
   double mLoc2[3];
   double mDirectionVector[3];
   double mNormal[3];
   bool   mDirectionSet;
};

#endif
