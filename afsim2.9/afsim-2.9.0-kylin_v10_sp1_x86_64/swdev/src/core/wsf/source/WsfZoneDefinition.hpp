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

#ifndef WSFZONEDEFINITION_HPP
#define WSFZONEDEFINITION_HPP

#include "wsf_export.h"

#include <vector>

class UtEntity;
class UtInput;
class UtInputBlock;
#include "UtVec2.hpp"
#include "UtVec3.hpp"
class WsfDraw;
class WsfGeoPoint;
class WsfPlatform;
#include "WsfZone.hpp"

//! A class for defining a zone.

class WSF_EXPORT WsfZoneDefinition : public WsfZone
{
public:
   WsfZoneDefinition();
   WsfZoneDefinition(const WsfZoneDefinition& aSrc);
   WsfZoneDefinition& operator=(const WsfZoneDefinition&) = delete;
   ~WsfZoneDefinition() override                          = default;

   WsfZone* Clone() const override;

   bool PointIsInside(WsfSimulation* aSimulationPtr,
                      const double   aViewedPointWCS[3],
                      const double   aEyePointWCS[3],
                      const double   aLookHeading,
                      const double   aDeltaDownRange = 0.0) override;

   double CheckIntersections(const double aLLA1[3], const double aLLA2[3]) override;

   void Initialize(const WsfScenario& aScenario) override;

   bool ProcessInput(UtInput& aInput) override;

   enum RelativeToType
   {
      cOBSERVER, //!< The Zone is relative to the observer (movable to fit the current context).
      cINTERNAL  //!< The Zone is fixed at some location or relative to a reference platform.
   };

   enum ShapeType
   {
      cNO_SHAPE,   //!< The shape of this Zone has not yet been determined.
      cCIRCULAR,   //!< The shape of this Zone is a vertical cylinder.
      cELLIPTICAL, //!< The shape of this Zone is a vertical extruded ellipse.
      cPOLYGONAL,  //!< The shape of this Zone is a vertical extruded polygon.
      cSPHERICAL   //!< The shape of this Zone is a 3-dimensional sphere.
   };

   //! Update the geometric reference point for the zone from the simulation.
   void UpdateReference(const WsfSimulation& aSimulation);

   //! Set the geometric reference point for the zone.
   void SetReference(UtEntity& aReference);

   //! Set the geometric reference point for the zone.
   void SetReference(const WsfGeoPoint& aReference, double aReferenceHeading);

   //! Set the geometric reference point for the zone.
   void SetReference(double aReferenceWCS[3], double aReferenceHeading);

   //! Set the geodetic reference point for the zone.
   void SetReference(double aReferenceLat, double aReferenceLon, double aReferenceHeading);

   WsfGeoPoint Reference() override;

   void SetShapeType(ShapeType aType);
   void SetRelativeType(RelativeToType aType);

   void SetAltBounds(double aMinAlt, double aMaxAlt);

   void SetRadialBounds(double aMinRadius, double aMaxRadius);

   void SetAngleBounds(double aStartAngle, double aStopAngle);

   void SetEllipticalAxes(double aLatAxisLen, double aLonAxisLen);

   void SetPolyPoints(const std::vector<WsfGeoPoint>& aPoints);
   void SetPolyPointsRelative(const std::vector<UtVec3d>& aPoints);

   const std::vector<WsfZone::Point>& GetPoints() const { return mPoints; }
   void                               GetPoints(std::vector<WsfGeoPoint>& aPoints) const;

   ut::optional<UtColor> GetFillColor() const override;
   ut::optional<UtColor> GetLineColor() const override;

   ShapeType      GetShapeType() const { return mShapeType; }
   RelativeToType GetRelativeType() const { return mRelativeTo; }
   WsfStringId    GetReferencePlatformName() const { return mReferencePlatformNameId; }
   double         GetReferenceLat() const { return mReferenceLat; }
   double         GetReferenceLon() const { return mReferenceLon; }

   //! Returns true the zone is a 'negative' zone and returns the opposite value for PointIsInside()
   bool IsNegative() const { return mNegative; }

   bool HasAbsoluteLatLon() const { return mHasAbsoluteLatLon; }

   void SetPointsAreLatLon(bool aValue) { mPointsAreLatLon = aValue; }
   bool PointsAreLatLon() { return mPointsAreLatLon; }

   bool GetLatLonExtrema(double& aSouthernLat, double& aWesternLon, double& aNorthernLat, double& aEasternLon) const override;

   // For circular zones only.
   double GetRadius() const;
   double GetMinRadius() const;

   // For elliptical zones
   double GetReferenceHeading() const;
   double GetLatAxisLength() const;
   double GetLonAxisLength() const;
   void   GetAngleBounds(double& aStartAngle, double& aStopAngle) const;
   void   GetAltBounds(double& aMinAlt, double& aMaxAlt) const;

   WsfPlatform* GetReferencePlatform(WsfSimulation* aSimulationPtr) const;

   void VerticesIn(double aMinX, double aMinY, double aMaxX, double aMaxY, std::vector<Point>& aPointsVector) const;

   void Draw(WsfDraw& aDraw) override;
   void DrawRelative(WsfDraw& aDraw, const double aEyePointWCS[3], const double aLookHeading) override;
   void DrawCentroid(WsfDraw& aDraw) override;
   void DrawBounds(WsfDraw& aDraw) override;

   void DebugDrawZone(WsfDraw& aDraw) override;
   void DebugDrawZoneRelative(WsfDraw& aDraw, const double aEyePointWCS[3], const double aLookHeading) override;
   void DebugDrawCentroid(WsfDraw& aDraw) override;
   void DebugDrawBounds(WsfDraw& aDraw) override;

   bool WithinPolygonSides(const double aXorLatValue, const double aYorLonValue) const;

protected:
   // void ProcessUnitTestBlock(UtInputBlock& aInputBlock);

   void AddPoint(const Point& aPoint);

private:
   void CalculateArea();
   void CalculateCentroid();

   void ConvertWCSToLocalFrame(const double aWCSLocation[3],
                               const double aEyePointWCS[3],
                               const double aLookHeading,
                               double       aPointInMyFrame[3]) const;

   bool PointIsInsidePrivate(const double aViewedPointWCS[3],
                             const double aEyePointWCS[3],
                             const double aLookHeading,
                             const double aDeltaDownRange) const;

   void SetReference(double aReferenceLat, double aReferenceLon, double aReferenceHeading, double aReferenceWCS[3]);

   // These methods carve the larger Zone test into smaller sub-pieces
   // for efficiency of execution.  (Although some are valid only in
   // the context of a local frame.)  The following four should be done
   // (more or less) in order, as appropriate.
   bool WithinAltitudeBounds(const double aViewedPointWCS[3],
                             const double aEyePointWCS[3],
                             double&      aViewedLat,
                             double&      aViewedLon,
                             double&      aViewedAlt,
                             bool&        aLLAValid) const;

   bool WithinAngleBounds(const double aPointInMyFrame[3], double& aViewedAngle) const;

   bool WithinRadialBounds(const double aPointInMyFrame[3],
                           const double aViewedPointWCS[3],
                           const double aEyePointWCS[3],
                           double&      aViewedRadiusSquared) const;

   // Prevent use of operator= by declaring, but not defining.
   WsfZone& operator=(const WsfZone& aRhs) = delete;

   bool WithinAltitudeBounds(const double aViewedAlt) const;
   bool WithinGrossXYBounds(const double aXorLatValue, const double aYorLonValue) const;

   double ExtrudedZonePenetrationDistance(const double aLLA1[3], const double aLLA2[3]);

   double SphericalZonePenetrationDistance(const double aLLA1[3], const double aLLA2[3]);

   bool Penetration2D(const UtVec2d& aP0, const UtVec2d& aP1, UtVec2d& aI1, UtVec2d& aI2);

   bool PenetrationIntoPolygon(const UtVec2d& aP0, const UtVec2d& aP1, UtVec2d& aI1, UtVec2d& aI2);

   bool PenetrationIntoCircle(const UtVec2d& aP0, const UtVec2d& aP1, UtVec2d& aI1, UtVec2d& aI2);

   bool PenetrationIntoEllipse(const UtVec2d& aP0, const UtVec2d& aP1, UtVec2d& aI1, UtVec2d& aI2);

   // if mReferenceType == cSTATIONARY, mPoints are (lat,lon)
   // pairs, else they are local-frame (x,y) points.
   std::vector<Point> mPoints;

   // The first defined attributes are only in the context of a local frame:
   ShapeType mShapeType;
   double    mMinRadiusSq;
   double    mMaxRadiusSq;
   double    mLongitudinalAxis;
   double    mLateralAxis;
   double    mStartAngle;
   double    mStopAngle;
   bool      mPointsAreLatLon;
   bool      mPointsInputIsMGRS;
   bool      mPointsInputIsPolar;
   bool      mNegative;

   // The latter defined attributes place the Zone in some larger context,
   // and define the transforms needed.  The most "minimal" attribute set
   // would include only a Latitude, Longitude, Heading.  But to permit
   // use of UtEntity class for transformations, a full WCS location is
   // specified below.  But we will discard consideration of viewer altitude,
   // pitch, and roll angles.  The viewed entity or point may not fall
   // within the Zone because it is outside of allowed altitudes.  (The
   // class must encapsulate the WCS location to assure that it does indeed
   // lie at Mean Sea Level altitude.)
   RelativeToType mRelativeTo;
   double         mReferenceWCS[3];
   double         mReferenceLat;
   double         mReferenceLon;
   double         mReferenceHeading;
   bool           mHasAbsoluteLatLon;
   WsfStringId    mReferencePlatformNameId;
   size_t         mReferencePlatformIndex;

   //! Possible reference to another Zone
   WsfZoneDefinition* mRefZonePtr;
   int                mZoneIndex;

   //! Debug flag
   bool mDebugEnabled;
};

#endif
