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

#include "FiresPath.hpp"

#include "FiresTables.hpp"
#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtMath.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"

namespace Fires
{

namespace
{
double g = UtEarth::cACCEL_OF_GRAVITY;
}

//! Constructor used by the FiresMover.
FiresPath::FiresPath(WsfScenario& aScenario)
   : mPropagationType(cUNDEFINED)
   , mSimplePropagationLookupType(cUNDEFINED)
   , mFiresType(nullptr)
   , mFiresTablesPtr(nullptr)
   , mSimulationPtr(nullptr)
   , mRemoveOnImpact(true)
   , mImpactRange(0.0)
   , mMaxHeight(0.0)
   , mElevationAngle(0.0)
   , mImpactTime(0.0)
   , mInitialLocation()
   , mImpactLocation()
   , mHasImpactLocation(false)
   , mConstrainPropagation(false)
   , mInitialBearing(0.0)
   , mBearingError(0.0)
   , mElevationError(0.0)
   , mDeltaAlt(0.0)
   , mSinB(0.0)
   , mCosB(0.0)
   , mRoll(0.0)
   , mPitch(0.0)
   , mHeading(0.0)
   , mSimpleG(g)
   , v0x(0.0)
   , v0z(0.0)
   , mTimeConstant(75.0)
   , // Initial value
   mInitialTime(0.0)
   , mDebug(false)
{
   UtVec3d::Set(mLocWCS, 0.0);
   UtVec3d::Set(mVelWCS, 0.0);
   UtVec3d::Set(mVelNED, 0.0);
   UtVec3d::Set(mAccNED, 0.0);
   mFiresTablesPtr = static_cast<FiresTables*>(aScenario.FindExtension("wsf_fires"));
}

//! Constructor to be used independently of the FiresMover.
FiresPath::FiresPath(WsfSimulation& aSimulation,
                     UtStringId     aType,
                     WsfGeoPoint&   aInitialLocation,
                     double&        aTargetRange,
                     double&        aTargetBearing,
                     double         aAzimuthError, // = 0.0
                     double         aElevationError)       // = 0.0
   : mPropagationType(cUNDEFINED)
   , mSimplePropagationLookupType(cUNDEFINED)
   , mFiresType(aType)
   , mFiresTablesPtr(nullptr)
   , mSimulationPtr(&aSimulation)
   , mRemoveOnImpact(true)
   , mImpactRange(aTargetRange)
   , mMaxHeight(0.0)
   , mElevationAngle(0.0)
   , mImpactTime(0.0)
   , mInitialLocation(aInitialLocation)
   , mImpactLocation()
   , mHasImpactLocation(false)
   , mConstrainPropagation(false)
   , mInitialBearing(aTargetBearing)
   , mBearingError(aAzimuthError)
   , mElevationError(aElevationError)
   , mDeltaAlt(0.0)
   , mSinB(0.0)
   , mCosB(0.0)
   , mRoll(0.0)
   , mPitch(0.0)
   , mHeading(0.0)
   , mSimpleG(g)
   , v0x(0.0)
   , v0z(0.0)
   , mTimeConstant(100.0)
   , // temp
   mInitialTime(0.0)
   , mDebug(false)
{
   mFiresTablesPtr = static_cast<FiresTables*>(aSimulation.GetScenario().FindExtension("wsf_fires"));
}

FiresPath::FiresPath(const FiresPath& aSrc)
   : mPropagationType(aSrc.mPropagationType)
   , mSimplePropagationLookupType(aSrc.mSimplePropagationLookupType)
   , mFiresType(aSrc.mFiresType)
   , mFiresTablesPtr(aSrc.mFiresTablesPtr)
   , mSimulationPtr(aSrc.mSimulationPtr)
   , mRemoveOnImpact(aSrc.mRemoveOnImpact)
   , mImpactRange(aSrc.mImpactRange)
   , mMaxHeight(aSrc.mMaxHeight)
   , mElevationAngle(aSrc.mElevationAngle)
   , mImpactTime(aSrc.mImpactTime)
   , mInitialLocation(aSrc.mInitialLocation)
   , mImpactLocation(aSrc.mImpactLocation)
   , mHasImpactLocation(false)
   , mConstrainPropagation(aSrc.mConstrainPropagation)
   , mInitialBearing(aSrc.mInitialBearing)
   , mBearingError(aSrc.mBearingError)
   , mElevationError(aSrc.mElevationError)
   , mDeltaAlt(aSrc.mDeltaAlt)
   , mSinB(aSrc.mSinB)
   , mCosB(aSrc.mCosB)
   , mGeom(aSrc.mGeom)
   , mRoll(aSrc.mRoll)
   , mPitch(aSrc.mPitch)
   , mHeading(aSrc.mHeading)
   , mSimpleG(aSrc.mSimpleG)
   , v0x(aSrc.v0x)
   , v0z(aSrc.v0z)
   , mTimeConstant(aSrc.mTimeConstant)
   , mInitialTime(aSrc.mInitialTime)
   , mDebug(aSrc.mDebug)
{
   UtVec3d::Set(mLocWCS, aSrc.mLocWCS);
   UtVec3d::Set(mVelWCS, aSrc.mVelWCS);
   UtVec3d::Set(mVelNED, aSrc.mVelNED);
   UtVec3d::Set(mAccNED, aSrc.mAccNED);
}

// virtual
bool FiresPath::Initialize(double aSimTime)
{
   mInitialTime = aSimTime;
   if (mFiresTablesPtr->UsesMaxOrd(mFiresType))
   {
      if (mPropagationType != cSIMPLE)
      {
         mPropagationType = cMAX_ORD_TOF;
      }
      mSimplePropagationLookupType = cMAX_ORD_TOF; // if using simple propagation
   }
   else
   {
      if (mPropagationType != cSIMPLE)
      {
         mPropagationType = cELEVATION_TOF;
      }
      mSimplePropagationLookupType = cELEVATION_TOF; // if using simple propagation
   }
   bool ok = (mPropagationType != cUNDEFINED);
   ok &= ComputeTrajectoryParameters();
   return ok;
}

//! Retrieve the state (position and velocity) at the given time.
//!@param aSimTime The time at which the state is desired.
//!@param aLocWCS The location at the given time.
//!@param aVelWCS The velocity at the given time.
void FiresPath::GetState(double aSimTime, double aLocWCS[3], double aVelWCS[3])
{
   UpdateState(aSimTime);
   UtVec3d::Set(aLocWCS, mLocWCS);
   UtVec3d::Set(aVelWCS, mVelWCS);
}

//! Update the position and velocity of the munition to the given time.
//! @param aSimTime The time to which the state is to be updated.
void FiresPath::UpdateState(double aSimTime)
{
   double dt = aSimTime - mInitialTime;
   if (dt < 0.0)
   {
      dt = 0.0;
   }
   double x, z, vx, vz;

   if (mPropagationType == cSIMPLE)
   {
      // Find the NED location relative to start.
      vx = v0x;
      z  = v0z * dt - 0.5 * mSimpleG * dt * dt;
      x  = vx * dt;
      vz = v0z - mSimpleG * dt;
   }
   else
   {
      // Find the NED location relative to start.
      double term2 = exp(-dt / mTimeConstant);
      double term3 = 1.0 - term2;
      vx           = v0x * term2;
      vz           = v0z * term2 - mTimeConstant * g * term3;
      x            = mTimeConstant * v0x * term3;
      z            = -mTimeConstant * g * dt + mTimeConstant * (v0z + mTimeConstant * g) * (1.0 - term2);
   }

   double locNED[3] = {x * mCosB, x * mSinB, -z};
   mGeom.ConvertNEDToWCS(locNED, mLocWCS);
   UtVec3d::Set(mVelNED, vx * mCosB, vx * mSinB, -vz);
   mGeom.ConvertNEDVectorToWCS(mVelWCS, mVelNED);
   UtVec3d::Set(mAccNED, 0.0, 0.0, g - vz / mTimeConstant);

   // Align the orientation with the velocity vector.
   double speed = UtVec3d::Magnitude(mVelNED);
   if (speed > 0.001)
   {
      mHeading = atan2(mVelNED[1], mVelNED[0]);
      mPitch   = asin(-mVelNED[2] / speed);
      mRoll    = 0.0;
   }
}

// private
//! Assuming this object was initialized with enough data, compute the remaining parameters to
//! define the full trajectory for this munition.
bool FiresPath::ComputeTrajectoryParameters()
{
   bool ok = false;
   if (mImpactRange != 0.0)
   {
      // Compute the impact point
      double angleB = mInitialBearing + mBearingError;
      mSinB         = sin(angleB);
      mCosB         = cos(angleB);

      if (!mHasImpactLocation)
      {
         double targetVecNED[3] = {mImpactRange * mCosB, mImpactRange * mSinB, 0.0};
         double lat, lon, alt;
         mGeom.ConvertNEDToLLA(targetVecNED, lat, lon, alt);
         if (mSimulationPtr->GetTerrainInterface()->IsEnabled())
         {
            float targetAlt;
            mSimulationPtr->GetTerrainInterface()->GetGeodeticElev(lat, lon, targetAlt);
            mDeltaAlt = targetAlt - mInitialLocation.GetAlt();
         }
         else
         {
            mDeltaAlt = alt;
         }
      }

      ok = PerformTrajectoryLookup();

      if (ok)
      {
         if (mPropagationType == cSIMPLE)
         {
            v0z = mSimpleG * mImpactTime / 2.0;
            v0x = mImpactRange / mImpactTime;

            if (mElevationError != 0)
            {
               // initial v is the same, but we adjust v0x and v0z to match the new elevation angle.
               double v        = sqrt(v0z * v0z + v0x * v0x);
               mElevationAngle = UtMath::Limit(atan2(v0z, v0x) + mElevationError, 0.0, UtMath::cPI_OVER_2);
               double sinE     = sin(mElevationAngle);
               double cosE     = cos(mElevationAngle);
               v0z             = v * sinE;
               v0x             = v * cosE;

               // recompute other values
               mImpactTime  = 2.0 * v0z / mSimpleG;
               mImpactRange = v0x * mImpactTime;
            }
         }
         else
         {
            if (mPropagationType == cMAX_ORD_TOF)
            {
               ComputeTimeConstantFromMaxOrd(0.0);
            }
            else
            {
               ComputeTimeConstantFromElevationAngle(
                  0.0); // (Any elevation error has already been added during initialization).
            }

            double maxTableRange     = mFiresTablesPtr->GetTable(mFiresType)->GetMaximumRange();
            double impactGroundRange = mImpactRange;
            double zp                = 0.0;
            int    numIterations     = 1;
            double cosE              = cos(mElevationAngle);

            while (true)
            {
               double delz = mDeltaAlt - zp;
               if ((fabs(delz) < 0.5) || (numIterations > 100))
               {
                  // done, we've mathed the trajectory to within a half meter.
                  break;
               }

               mImpactRange += delz * fabs(cosE);
               if (delz > 0.0)
               {
                  if (mImpactRange > maxTableRange)
                  {
                     // no solution.
                     ok = false;
                     break;
                  }
               }

               PerformTrajectoryLookup();
               if (mPropagationType == cMAX_ORD_TOF)
               {
                  ComputeTimeConstantFromMaxOrd(0.0);
               }
               else
               {
                  ComputeTimeConstantFromElevationAngle(0.0);
               }

               // Compute the time at which the projectile is at the initially provided impact ground range;
               // this is the time at which it should be at mDeltaAlt as well.
               double t1 = -mTimeConstant * log(1.0 - impactGroundRange / (v0x * mTimeConstant));
               zp = (v0z * mTimeConstant + g * mTimeConstant * mTimeConstant) * (1.0 - exp(-t1 / mTimeConstant)) -
                    g * t1 * mTimeConstant;
               double term2 = exp(-t1 / mTimeConstant);
               double term3 = 1.0 - term2;
               double vx    = v0x * term2;
               double vz    = v0z * term2 - mTimeConstant * g * term3;
               cosE         = vx / sqrt(vx * vx + vz * vz);
               ++numIterations;
            }

            // Adjust for elevation error.
            if (mElevationError != 0)
            {
               // Compute a new impact range based on simple ballistics and elevation angle change.
               double sinE             = sin(mElevationAngle);
               double cosE             = cos(mElevationAngle);
               double v0               = sqrt(v0z * v0z + v0x * v0x);
               double deltaImpactRange = (2.0 * v0 / mSimpleG) * (v0x * cosE - v0z * sinE) * mElevationError;
               mImpactRange += deltaImpactRange;
               if (mImpactRange <= 0.0)
               {
                  mImpactRange = 10.0; // might want to print a warning here
               }

               // Perform a second lookup based on the new impact range.
               if (mPropagationType == cMAX_ORD_TOF)
               {
                  ok = mFiresTablesPtr->GetMaxOrdAndTOF(mFiresType, mImpactRange, mMaxHeight, mImpactTime);
                  ComputeTimeConstantFromMaxOrd(mDeltaAlt);
               }
               else if (mPropagationType == cELEVATION_TOF)
               {
                  ok = mFiresTablesPtr->GetElevationAngleAndTOF(mFiresType, mImpactRange, mElevationAngle, mImpactTime);
                  ComputeTimeConstantFromElevationAngle(
                     mDeltaAlt); // (Any elevation error has already been added during initialization).
               }
            }
         }

         bool isPhysicalSolution = (mSimpleG <= g);
         if (!isPhysicalSolution)
         {
            if (mConstrainPropagation)
            {
               mPropagationType = cSIMPLE; // This guarantees a solution that uses the proper parameters
            }
         }
      }
      if (ok && mDebug)
      {
         double initialAngle = atan2(v0z, v0x);
         auto   logDebug     = ut::log::debug() << "FiresPath::ComputeTrajectoryParameters Data Report:";
         auto   logNote      = logDebug.AddNote() << mFiresType << ":";
         logNote.AddNote() << "Initial velocity: " << sqrt(v0x * v0x + v0z * v0z);
         logNote.AddNote() << "Initial angle: " << initialAngle * UtMath::cDEG_PER_RAD << " deg ("
                           << initialAngle * 6400 / UtMath::cTWO_PI << ") mils; ";
         logNote.AddNote() << "Target range: " << mImpactRange;
         logNote.AddNote() << "Impact time: " << mImpactTime;
         logNote.AddNote() << "Time constant: " << mTimeConstant;
         logNote.AddNote() << "Max height: " << mMaxHeight;
         logNote.AddNote() << "v0x (calc, no drag): " << v0x << ", " << mImpactRange / mImpactTime;
         logNote.AddNote() << "Simple g: " << mSimpleG;
         auto logSimplePropagation = logNote.AddNote();
         if (mPropagationType != cSIMPLE)
         {
            logSimplePropagation << "not ";
         }
         logSimplePropagation << "using simple propagation.";
      }
   }
   return ok;
}

bool FiresPath::PerformTrajectoryLookup()
{
   bool ok = false;
   if ((mPropagationType == cMAX_ORD_TOF) || (mSimplePropagationLookupType == cMAX_ORD_TOF))
   {
      ok = mFiresTablesPtr->GetMaxOrdAndTOF(mFiresType, mImpactRange, mMaxHeight, mImpactTime);
   }
   else if ((mPropagationType == cELEVATION_TOF) || (mSimplePropagationLookupType == cELEVATION_TOF))
   {
      ok = mFiresTablesPtr->GetElevationAngleAndTOF(mFiresType, mImpactRange, mElevationAngle, mImpactTime);
   }
   return ok;
}

void FiresPath::ComputeInitialVelocity(double aDeltaAlt)
{
   double term0 = exp(-mImpactTime / mTimeConstant);
   double term1 = (1.0 - term0);
   v0x          = mImpactRange / mTimeConstant / term1;
   v0z          = g * mImpactTime / term1 - mTimeConstant * g + aDeltaAlt / mTimeConstant / term1;
}

void FiresPath::ComputeTimeConstantFromMaxOrd(double aDeltaAlt)
{
   // Find the NED location relative to start.
   // Iterative solution for time constant.
   int                 iterations = 0;
   static const double maxTC      = 500.0;
   static const double minTC      = 10.0;

   while (true)
   {
      ComputeInitialVelocity(aDeltaAlt);
      double th = mTimeConstant * log1p(v0z / (g * mTimeConstant));

      double term2  = exp(-th / mTimeConstant);
      double term3  = 1.0 - term2;
      double height = -mTimeConstant * g * th + mTimeConstant * (v0z + mTimeConstant * g) * term3;
      if (height < 0.0)
      {
         height = 0.0;
      }
      double delh = mMaxHeight - height;
      if (fabs(delh) <= 0.1)
      {
         break;
      }
      // Change time constant proportional to the differential of z
      // with time constant.  Iterate until z = z (max ord).
      double dzda = (v0z + 2.0 * g * mTimeConstant) * term3 - v0z * th / mTimeConstant * term2 - g * th * (1.0 + term2);
      double dela = delh / dzda; // Difference equation to find change in time constant
      mTimeConstant += dela;
      ++iterations;
      if ((iterations > 200) || (mTimeConstant > maxTC) || (mTimeConstant < minTC))
      {
         if (mTimeConstant > maxTC)
         {
            mTimeConstant = maxTC;
         }
         else if (mTimeConstant < minTC)
         {
            mTimeConstant = minTC;
         }
         if (mDebug)
         {
            auto logWarning = ut::log::warning() << "Fires mover solution convergence in Height."
                                                 << " continuing...";
            logWarning.AddNote() << "Height: " << delh;
         }
         break;
      }
   }

   ComputeInitialVelocity(aDeltaAlt);

   // Calculate the (imaginary) reduced value of g
   mSimpleG = 8.0 * mMaxHeight / mImpactTime / mImpactTime;

   // There is no exact solution if there is no-drag case that can't be met.
   if (mDebug && (mSimpleG > UtEarth::cACCEL_OF_GRAVITY))
   {
      auto logWarning = ut::log::warning() << "FiresTable: No physical ballistic solution for Type.";
      logWarning.AddNote() << "Type: " << mFiresType.GetString();
      logWarning.AddNote() << "Range: " << mImpactRange;
   }

   // Compute the initial elevation angle.
   mElevationAngle = atan2(v0z, v0x);
}

void FiresPath::ComputeTimeConstantFromElevationAngle(double aDeltaAlt)
{
   double              tanTrueE = tan(mElevationAngle);
   static const double maxTC    = 500.0;
   static const double minTC    = 10.0;

   // Iterative solution for time constant.
   int iterations = 0;
   while (true)
   {
      double term0       = exp(-mImpactTime / mTimeConstant);
      double term1       = (1.0 - term0);
      v0z                = g * mImpactTime / term1 - mTimeConstant * g + aDeltaAlt / mTimeConstant / term1;
      v0x                = v0z / tanTrueE;
      double impactRange = v0x * mTimeConstant * term1;
      double dRdTc       = v0x * term1 - v0x * mImpactTime / mTimeConstant * term0;
      double dR          = mImpactRange - impactRange;
      if (fabs(dR) < 0.1)
      {
         break;
      }
      double dTc = dR / dRdTc;
      mTimeConstant += dTc;

      ++iterations;
      if ((iterations > 200) || (mTimeConstant > maxTC) || (mTimeConstant < minTC))
      {
         if (mTimeConstant > maxTC)
         {
            mTimeConstant = maxTC;
         }
         else if (mTimeConstant < minTC)
         {
            mTimeConstant = minTC;
         }
         if (mDebug)
         {
            double v0xc = mImpactRange / mTimeConstant / term1;
            double delE = fabs(mElevationAngle - atan2(v0z, v0xc));
            ut::log::warning() << "Fires mover solution convergence in elevation angle: " << delE
                               << " in 200 iterations... continuing.";
         }
         break;
      }
   }

   ComputeInitialVelocity(aDeltaAlt);
   double tmo = mTimeConstant * log1p(v0z / (g * mTimeConstant));
   mMaxHeight = (v0z * mTimeConstant + g * mTimeConstant * mTimeConstant) * (1.0 - exp(-tmo / mTimeConstant)) -
                g * mTimeConstant * tmo;

   // Calculate the (imaginary) reduced value of g
   mSimpleG = 8.0 * mMaxHeight / mImpactTime / mImpactTime;
}

} // namespace Fires
