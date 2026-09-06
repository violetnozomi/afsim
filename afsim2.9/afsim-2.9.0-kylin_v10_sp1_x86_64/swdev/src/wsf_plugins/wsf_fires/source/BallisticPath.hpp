// ****************************************************************************
// CUI//REL TO USA ONLY
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2003-2015 The Boeing Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#ifndef DESIM_BALLISTIC_PATH
#define DESIM_BALLISTIC_PATH

#include "wsf_fires_export.h"

#include <map>
#include <string>

class UtInput;
#include "UtScriptClassDefine.hpp"
class UtScriptContext;
class WsfAero;
class WsfDraw;
#include "FiresMover.hpp"
#include "UtEntity.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"

namespace Fires
{
class FiresTables;

class WSF_FIRES_EXPORT BallisticPath
{
public:
   explicit BallisticPath(WsfSimulation* aSimPtr);
   BallisticPath(const BallisticPath& aSrc);
   virtual ~BallisticPath();

   BallisticPath& operator=(const BallisticPath& aSrc) = delete;

   bool IsTargetType(const std::string& aType);

   virtual bool Update(const WsfTrack& aTrack);

   virtual void Update(WsfPlatform* aPlatform);

   virtual bool IsValid() const { return mIsInitialized; }

   void Draw(WsfDraw& aDraw) const;

   //! Set the integration interval to be used for trajectory extrapolation.
   void SetIntegrationInterval(double aInterval) { mIntegrationInterval = aInterval; }

   //! Get the time at which the trajectory is at its apogee.
   double GetApogeeTime() const { return mApogeeTime; }

   //! Get the calculated impact time.
   double GetImpactTime() const { return mImpactTime; }

   //! Get the start time that the trajectory was calculated.
   double GetStartTime() const { return mStartTime; }


   bool GetLocationWCS(double aSimTime, double aLocWCS[3]) const;

   bool GetVelocityWCS(double aSimTime, double aVelWCS[3]) const;

   static void
   GetRangeAndAltitude(const double aOriginLocWCS[3], const double aTargetLocWCS[3], double& aRange, double& aAltitude);

   bool IsInitialized() const { return mIsInitialized; }

   const WsfGeoPoint& GetImpactPoint() const { return mImpactPoint; }

protected:
   virtual void Integrate(double aElapsedTime, double aLocWCS[3], double aVelWCS[3]);

private:
   void UpdateFires(double aTime, FiresMover& aFiresMover, const double aLocWCS[3], const double aVelWCS[3]);

   void UpdateSpherical(double aTime, const double aLocWCS[3], const double aVelWCS[3]);

   bool WillImpact(double aAltitude);

   void ComputeImpact(double aAlt1, double aAlt2);

   WsfSimulation*     mSimPtr;
   const WsfScenario& mScenario;
   FiresTables*       mFiresTablesPtr;

   UtEntity   mTargetState;
   UtStringId mType;
   double     mMass;
   WsfAero*   mTargetAeroPtr;
   double     mIntegrationInterval;
   bool       mIsInitialized;
   bool       mUseFiresExtrapolation;

   // The following are spherical Earth values. Integration is done is done in the spherical
   // frame to correspond with the frame used by WsfGuidedMover. Results are converted to
   // ellipsoidal earth values for external use.
   double mLocWCS[3];
   double mVelWCS[3];
   double mWCS_ToNED_Transform[3][3];

   //! Approximate Impact lat, lon alt
   WsfGeoPoint mImpactPoint;

   //! The simulation time of the first point in the trajectory.
   double mStartTime;

   // ! The simulation time at which apogee occurs.
   double mApogeeTime;

   //! The simulation time at which the target hits the ground.
   //! This is NOT the time at the last point... that point is one time-step past ground impact.
   double mImpactTime;

   struct Point
   {
      double mLocWCS[3];
      double mVelWCS[3];
   };

   //! The points that define the trajectory.
   std::vector<Point> mPoints;
};

class WSF_FIRES_EXPORT ScriptBallisticPathClass : public UtScriptClass
{
public:
   ScriptBallisticPathClass(const std::string& aClassName, UtScriptTypes* aTypesPtr);

   ~ScriptBallisticPathClass() override;

   // See the base class for a description of Create, Clone, and Destroy.

   void* Create(const UtScriptContext& aContext) override;
   void* Clone(void* aObjectPtr) override;
   void  Destroy(void* aObjectPtr) override;

   // Each of the exported methods are defined as function objects.

   UT_DECLARE_SCRIPT_METHOD(Update);
   UT_DECLARE_SCRIPT_METHOD(HasData);
   UT_DECLARE_SCRIPT_METHOD(IsBallisticType);
   UT_DECLARE_SCRIPT_METHOD(Draw);
   UT_DECLARE_SCRIPT_METHOD(ImpactPoint);
   UT_DECLARE_SCRIPT_METHOD(ImpactTime);
   UT_DECLARE_SCRIPT_METHOD(PointAtTime);
};
}; // namespace Fires

#endif
