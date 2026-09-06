// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2013 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************
#ifndef VaPosition_hpp
#define VaPosition_hpp

#include "vespatk_export.h"

// 'VaPosition' is used to represent the position of an object.  It should
// probably only be used to for the position of explicit user objects such as
// players, waypoints, etc. as it contains some higher level objects that
// maintain precision and units from the user input.
//
// Horizontal position can be represented with latitude and longitude or as
// an X-Y offset relative to a scenario center.  Vertical position can be
// specified as altitude above mean sea level (MSL).
//
// All distances are specified in meters.  All angles are in degrees with
// positive values for north and east.

#include <istream>
#include <map>
#include <ostream>
#include <vector>

#include "UtLatPos.hpp"
#include "UtLength.hpp"
#include "UtLonPos.hpp"
#include "UtVec3.hpp"
#include "VaCallbackHolder.hpp"

class MapProjection;
class UtEntity;
class UtLLAPos;
class UtoMapProjection;

namespace vespa
{
class VESPATK_EXPORT VaPosition
{
public:
   enum ProjectionMode
   {
      PmNone                  = 0,
      PmSupTransverseMercator = 1,
      PmSupOrthographic       = 2,
      PmEquidistant           = 3,
      PmIwars                 = 4
   };

   VaPosition();

   VaPosition(double aLat, double aLon, double aAlt);

   VaPosition(double aLat, double aLon, double aAlt, double aECEFx, double aECEFy, double aECEFz);

   VaPosition(double aX, double aY);

   VaPosition(const VaPosition& aSrc);

   VaPosition& operator=(const VaPosition& aPosition);

   virtual ~VaPosition() = default;

   bool operator==(const VaPosition& aRhs) const;
   bool operator!=(const VaPosition& aRhs) const;

   void DirtyPosition();

   // Accessors

   const UtLatPos& GetLat() const { return mLat; }
   const UtLonPos& GetLon() const { return mLon; }

   const UtLength& GetX() const { return mX; }
   const UtLength& GetY() const { return mY; }

   const UtLength& GetAlt() const { return mAlt; }

   // Returns 'true' if the position is an X-Y position.
   bool PositionIsXY() const { return mPositionIsXY; }
   void SetPositionIsXY(bool aValue) { mPositionIsXY = aValue; }

   // Other methods

   // Get the current latitude, longitude and altitude.  If the object
   // is defined with an X-Y position then the equivalent latitude and
   // longitude will be returned.

   void GetLLA(UtLLAPos& aLLA) const;
   void SetLLA(const UtLLAPos& aLLA);
   void SetLLA(const UtLatPos& aLat, const UtLonPos& aLon, const UtLength& aAlt);

   void MoveRelative(UtEntity& aRefBefore, UtEntity& aRefAfter, int aFeedbackFormat, double aFeedbackPrecision);

   void SetAlt(const UtLength& aAlt);
   void SetAlt(double aAlt);
   void SetAltFormat(int aFormat);
   void SetLat(const UtLatPos& aLat);
   void SetLat(double aLat);
   void SetLatFormat(int aFormat);
   void SetLon(const UtLonPos& aLon);
   void SetLon(double aLon);
   void SetLonFormat(int aFormat);
   void SetX(double aX);
   void SetY(double aY);
   void SetX(const UtLength& aX);
   void SetY(const UtLength& aY);

   void         GetECEF(double aECEF[3]) const;
   const double GetECEF_X() const;
   const double GetECEF_Y() const;
   const double GetECEF_Z() const;

   void CalculateGreatCircleHeadingAndDistance(const VaPosition& aPosition, double& aHeading, double& aDistance) const;
   void CalculateHeadingAndDistance(const VaPosition& aPosition, double& aHeading, double& aDistance) const;

   virtual void Translate(double aDeltaLat, double aDeltaLon);

   virtual void Rotate(const UtLLAPos& aCenterLLA, double aAngle);

   static MapProjection* GetMapProjection() { return mProjection; }

   static UtoMapProjection* GetSceneProjection(int aViewerId);

   static void TranslateProjectionCenter(double aDeltaLat, double aDeltaLon);

   static void SetProjectionMode(ProjectionMode aProjectionMode);
   static void SetProjectionCenter(const UtLatPos& aLat, const UtLonPos& aLon);

   static const UtLatPos& GetCenterLat() { return mCenterLat; }
   static const UtLonPos& GetCenterLon() { return mCenterLon; }

   // Reset the global data back to its initial state.

   static void Reset();

   static void SetDefaultPositionIsXY() { mDefaultPositionIsXY = false; }

   static void ConvertLLAToXYA(const UtLLAPos& aLLA, const UtLLAPos& aRefLLA, double aXYA[3]);
   static void ConvertLLAToXYA(const UtLLAPos& aLLA, const UtLLAPos& aRefLLA, UtVec3d& aXYA);

   static void ConvertXYAToLLA(const double aXYA[3], const UtLLAPos& aRefLLA, UtLLAPos& aLLA);
   static void ConvertXYAToLLA(const UtVec3d& aXYA, const UtLLAPos& aRefLLA, UtLLAPos& aLLA);

   void              GetSceneXYZ(double aXYZ[3], int aViewerId) const;
   static VaPosition GetPositionOfSceneXYZ(const double aXYZ[3], int aViewerId);

   void GetSceneUpVector(double aXYZ[3], int aViewerId) const;

   static void InitializeStaticSubscriptions();
   static void Shutdown();

   static void ChangeMapProjectionCB(int aViewerId, const UtoMapProjection* aMapProjection);
   static int  GetSceneProjectionType(int aViewerId);

   static VaPosition ComputeCentroid(const std::vector<VaPosition>& aPositionList);

private:
   void UpdateLatLon();
   void UpdateXY();

   static MapProjection* mProjection;

   // Center of the scenario for relative scenarios.

   static ProjectionMode mProjectionMode;
   static UtLatPos       mCenterLat;
   static UtLonPos       mCenterLon;

   // 'true' if newly created positions should be in X/Y format.

   static bool mDefaultPositionIsXY;

   // Latitude and longitude of the point in degrees.  If this a XY point
   // then this is the converted latitude and longitude.

   UtLatPos mLat;
   UtLonPos mLon;

   // For XY points, this is the location in meters from the center of
   // the scenario.

   UtLength mX;
   UtLength mY;

   UtLength mAlt;

   bool mPositionIsXY;

   mutable bool mElevIsInvalid;

   mutable bool   mECEF_Defined;
   mutable double mECEF[3];

   static VaCallbackHolder                 msCallbacks;
   static std::map<int, UtoMapProjection*> msSceneProjectionMap;
};
} // end namespace vespa
#endif
