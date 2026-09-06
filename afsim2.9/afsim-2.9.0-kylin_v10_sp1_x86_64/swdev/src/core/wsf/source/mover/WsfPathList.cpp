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

#include "WsfPathList.hpp"

#include <cassert>

#include "UtDCM.hpp"
#include "UtEarth.hpp"
#include "UtEntity.hpp"
#include "UtLatPos.hpp"
#include "UtLog.hpp"
#include "UtLonPos.hpp"
#include "UtMat3.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtUnitTypes.hpp"
#include "UtVec2.hpp"

//
// WsfPathList provides a way of defining a path of a platform.  A path is composed of a list of
// segments.  The segments compute the state of the entity given an offset time.
//
// Most segments implemented here operate using rotations.  For a straight segment, an
// arc is constructed using a rotation around an axis that passes through the center of the earth.  For
// turn segments, a rotation is made around an axis that passes through the center of the turn.  For any
// segment, a state along the segment can be computed directly given the offset time.  As a result,
// this code is incapable of computing certain dynamic paths, in particular, any turn with a variable turn
// rate.  Therefore, if a turn path is made with linear acceleration the radius remains constant.
//

namespace
{

void ConvertWCSVectorToNED(double alat, double alon, const double aWCSVector[3], double aNED[3])
{
   double trans[3][3];

   double lat    = alat * UtMath::cRAD_PER_DEG;
   double lon    = alon * UtMath::cRAD_PER_DEG;
   double sinLat = sin(lat);
   double cosLat = cos(lat);
   double sinLon = sin(lon);
   double cosLon = cos(lon);

   trans[0][0] = -sinLat * cosLon;
   trans[0][1] = -sinLat * sinLon;
   trans[0][2] = cosLat;

   trans[1][0] = -sinLon;
   trans[1][1] = cosLon;
   trans[1][2] = 0.0;

   trans[2][0] = -cosLat * cosLon;
   trans[2][1] = -cosLat * sinLon;
   trans[2][2] = -sinLat;
   UtMat3d::Transform(aNED, trans, aWCSVector);
}

// Shared class used in computing the state along a segment.
struct DynamicsComputation
{
   double  mSpeed;
   double  mVerticalDistance;
   double  mArcDistance;
   double  mHorizontalAcceleration;
   double  mVerticalAcceleration;
   double  mVerticalSpeed;
   double  mGroundSpeed;
   UtVec3d mLocationWCS;
   UtVec3d mVelocityDirectionWCS;
   void    ComputeLevelDynamics(double                           aRelativeSimTime,
                                const WsfPathList::Segment&      aSegment,
                                const WsfPathList::RotationData& aRotation)
   {
      mGroundSpeed = mSpeed   = UtVec3d::Magnitude(aSegment.mInitialState.mVelocityNED);
      mVerticalSpeed          = 0.0;
      mHorizontalAcceleration = 0.0;
      mVerticalAcceleration   = 0.0;
      mArcDistance            = mSpeed * aRelativeSimTime;
      mVerticalDistance       = 0.0;
   }

   void ComputeDynamics(double                           aRelativeSimTime,
                        const WsfPathList::Segment&      aSegment,
                        const WsfPathList::DynamicsData& aDynamics,
                        const WsfPathList::RotationData& aRotation)
   {
      double initialSpeed     = UtVec3d::Magnitude(aSegment.mInitialState.mVelocityNED);
      mSpeed                  = initialSpeed + aRelativeSimTime * aDynamics.mLinearAcceleration;
      double sinFlightAngle   = std::sin(aDynamics.mFlightAngle);
      double cosFlightAngle   = std::cos(aDynamics.mFlightAngle);
      mGroundSpeed            = sqrt(mSpeed * mSpeed - aDynamics.mClimbRate * aDynamics.mClimbRate) * cosFlightAngle;
      mVerticalSpeed          = mSpeed * sinFlightAngle + aDynamics.mClimbRate;
      mHorizontalAcceleration = aDynamics.mLinearAcceleration;
      mVerticalAcceleration   = 0.0;

      // Compute vertical and arc distance
      if (aDynamics.mClimbRate != 0)
      {
         double v0    = sqrt(initialSpeed * initialSpeed - aDynamics.mClimbRate * aDynamics.mClimbRate);
         mArcDistance = v0 * aRelativeSimTime + aDynamics.mLinearAcceleration * aRelativeSimTime * aRelativeSimTime / 2.0;
         mVerticalDistance = aDynamics.mClimbRate * aRelativeSimTime;
      }
      else
      {
         double v0 = initialSpeed * cosFlightAngle;
         mHorizontalAcceleration *= cosFlightAngle;
         mVerticalAcceleration = aDynamics.mLinearAcceleration * sinFlightAngle;
         mArcDistance          = v0 * aRelativeSimTime +
                        aDynamics.mLinearAcceleration * cosFlightAngle * aRelativeSimTime * aRelativeSimTime * .5;
         double distance =
            initialSpeed * aRelativeSimTime + aRelativeSimTime * aRelativeSimTime * aDynamics.mLinearAcceleration * .5;
         mVerticalDistance = sinFlightAngle * distance;
      }
   }

   void ComputeArcRotation(double                           aRelativeSimTime,
                           const WsfPathList::Segment&      aSegment,
                           const WsfPathList::RotationData& aRotation,
                           WsfPathState&                    aState)
   {
      double rotationAngle = mArcDistance / aRotation.mRotationRadius;

      UtDCM dcm(-rotationAngle, aRotation.mRotationAxis.GetData());
      mLocationWCS          = dcm.Transform(aRotation.mInitialLocationWCS);
      mVelocityDirectionWCS = dcm.Transform(aRotation.mInitialDirectionWCS);

      // rot2.ReverseRotate(aRotation.mInitialLocationWCS.GetData(), mLocationWCS.GetData());
      // rot2.ReverseRotate(aRotation.mInitialDirectionWCS.GetData(), mVelocityDirectionWCS.GetData());

      UtEntity::ConvertWCSToLLA(mLocationWCS.GetData(), aState.mLatitude, aState.mLongitude, aState.mAltitude);
      aState.mAltitude = aSegment.mInitialState.mAltitude + mVerticalDistance;

      UtVec3d finalDirectionNED;
      ConvertWCSVectorToNED(aState.mLatitude, aState.mLongitude, mVelocityDirectionWCS.GetData(), finalDirectionNED.GetData());

      double horizontalCorrection = mGroundSpeed / UtVec2d::Magnitude(finalDirectionNED.GetData());
      aState.mOrientationNED[0]   = atan2(finalDirectionNED[1], finalDirectionNED[0]);
      aState.mOrientationNED[2]   = 0.0;

      aState.mVelocityNED[0] = horizontalCorrection * finalDirectionNED[0];
      aState.mVelocityNED[1] = horizontalCorrection * finalDirectionNED[1];
      aState.mVelocityNED[2] = -mVerticalSpeed;
   }

   void SetPitch(const WsfPathList::Segment& aSegment, const WsfPathList::DynamicsData& aDynamics, WsfPathState& aState)
   {
      if (aDynamics.mClimbRate != 0.0)
      {
         aState.mOrientationNED[1] = atan2(-aState.mVelocityNED[2], mGroundSpeed);
      }
      else
      {
         aState.mOrientationNED[1] = aDynamics.mFlightAngle;
      }
   }
   void SetRoll(double aRelativeSimTime, const WsfPathList::Segment& aSegment, double aTargetRoll, WsfPathState& aState)
   {
      double& roll = aState.mOrientationNED[2];
      if (aSegment.mRollRate != 0 && aSegment.mInitialState.mOrientationNED[2] != aTargetRoll)
      {
         double rollChange = aTargetRoll - aSegment.mInitialState.mOrientationNED[2];
         double timeAtRoll = fabs(rollChange / aSegment.mRollRate);
         if (aRelativeSimTime > timeAtRoll)
         {
            roll = aTargetRoll;
         }
         else
         {
            roll = UtMath::Sign(rollChange) * aSegment.mRollRate * aRelativeSimTime +
                   aSegment.mInitialState.mOrientationNED[2];
         }
      }
      else
      {
         roll = aTargetRoll;
      }
   }
   void SetAcceleration(const WsfPathList::RotationData& aRotation, WsfPathState& aState)
   {
      // Compute the 'radial' aspect of the acceleration
      double radialAcceleration = mSpeed * mSpeed / aRotation.mRotationRadius;
      if (radialAcceleration > 0.01 || radialAcceleration < -0.01)
      {
         UtVec3d accelWCS;
         accelWCS.CrossProduct(mVelocityDirectionWCS, aRotation.mRotationAxis);
         accelWCS *= radialAcceleration / accelWCS.Magnitude();
         ConvertWCSVectorToNED(aState.mLatitude, aState.mLongitude, accelWCS.GetData(), aState.mAccelerationNED);
      }
      else
      {
         UtVec3d::Set(aState.mAccelerationNED, 0.0);
      }
      // Add in the vertical and forward/backward acceleration
      aState.mAccelerationNED[2] += mVerticalAcceleration;
      if (mHorizontalAcceleration > 0.01 || mHorizontalAcceleration < -0.01)
      {
         double yawSin = std::sin(aState.mOrientationNED[0]);
         double yawCos = std::cos(aState.mOrientationNED[0]);
         aState.mAccelerationNED[0] += yawCos * mHorizontalAcceleration;
         aState.mAccelerationNED[1] += yawSin * mHorizontalAcceleration;
      }
   }
};
} // namespace

void WsfPathList::Segment::Print(ut::log::MessageStream& aStream)
{
   aStream.AddNote() << "Duration: " << mDuration << " ";
   aStream.AddNote() << "Lat: " << UtLatPos(mInitialState.mLatitude);
   aStream.AddNote() << "Lon: " << UtLonPos(mInitialState.mLongitude);
   aStream.AddNote() << "Alt: " << UtLengthValue(mInitialState.mAltitude);
   aStream.AddNote() << "Velocity: " << mInitialState.mVelocityNED[0] << " m/s N, " << mInitialState.mVelocityNED[1]
                     << " m/s E";
}

void WsfPathList::RotationArcSegment::ComputeState(double aRelativeSimTime, WsfPathState& aState)
{
   DynamicsComputation computation;
   RotationData&       rotation = (RotationData&)*this;
   computation.ComputeLevelDynamics(aRelativeSimTime, *this, rotation);
   computation.ComputeArcRotation(aRelativeSimTime, *this, rotation, aState);
   aState.mOrientationNED[1] = 0.0;
   computation.SetRoll(aRelativeSimTime, *this, mRoll, aState);
   computation.SetAcceleration(rotation, aState);
}

void WsfPathList::RotationArcSegment::Print(ut::log::MessageStream& aStream)
{
   aStream.AddNote() << "Rotation";
   Segment::Print(aStream);
   bool isTurn = mRotationRadius < (UtSphericalEarth::cEARTH_RADIUS / 2.0);
   if (isTurn)
   {
      aStream.AddNote() << "Turn Radius: " << mRotationRadius << " m";
   }
   else
   {
      aStream.AddNote() << "Earth Radius: " << mRotationRadius << " m";
   }
}

void WsfPathList::RotationSegment::ComputeState(double aRelativeSimTime, WsfPathState& aState)
{
   DynamicsComputation computation;
   DynamicsData&       dynamics = (DynamicsData&)*this;
   RotationData&       rotation = (RotationData&)*this;
   computation.ComputeDynamics(aRelativeSimTime, *this, dynamics, rotation);
   computation.ComputeArcRotation(aRelativeSimTime, *this, rotation, aState);
   computation.SetPitch(*this, dynamics, aState);
   computation.SetRoll(aRelativeSimTime, *this, mRoll, aState);
   computation.SetAcceleration(rotation, aState);
}

void WsfPathList::RotationSegment::Print(ut::log::MessageStream& aStream)
{
   aStream.AddNote() << "Rotation";
   Segment::Print(aStream);
   bool isTurn = mRotationRadius < (UtSphericalEarth::cEARTH_RADIUS / 2.0);
   if (isTurn)
   {
      aStream.AddNote() << "Turn Radius: " << mRotationRadius;
   }
   else
   {
      aStream.AddNote() << "Earth Radius: " << mRotationRadius;
   }
   aStream.AddNote() << "Acceleration: " << mLinearAcceleration;
   if (mClimbRate != 0)
   {
      aStream.AddNote() << "Climb Rate: " << mClimbRate;
   }
   if (mFlightAngle != 0)
   {
      aStream.AddNote() << "Flight Angle: " << mFlightAngle;
   }
}

void WsfPathList::PauseSegment::Print(ut::log::MessageStream& aStream)
{
   ut::log::info() << "Pause";
   Segment::Print(aStream);
}
WsfPathList::WsfPathList()
   : mDuration(0.0)
   , mLastSegmentIndex(0)
   , mLastSegmentStartTime(0.0)
   , mSegments()
   , mEndState()
   , mEndStateValid(false)
{
}

WsfPathList::WsfPathList(const WsfPathList& aSrc)
   : mDuration(aSrc.mDuration)
   , mLastSegmentIndex(aSrc.mLastSegmentIndex)
   , mLastSegmentStartTime(aSrc.mLastSegmentStartTime)
   , mSegments(aSrc.mSegments)
   , mEndState(aSrc.mEndState)
   , mEndStateValid(aSrc.mEndStateValid)
{
   for (size_t i = 0; i < mSegments.size(); ++i)
   {
      mSegments[i] = mSegments[i]->Clone();
   }
}

WsfPathList::~WsfPathList()
{
   Reset();
}

void WsfPathList::Append(Segment* aSegmentPtr)
{
   double duration = aSegmentPtr->GetDuration();
   mDuration += duration;
   mSegments.push_back(aSegmentPtr);
   mEndStateValid = false;
}

void WsfPathList::Append(Segment* aSegmentPtr, const WsfPathState& aEndState)
{
   Append(aSegmentPtr);
   mEndState      = aEndState;
   mEndStateValid = true;
}

void WsfPathList::Append(double aDuration, WsfPathList& aPathList)
{
   double durationLeft = aDuration;
   for (size_t i = 0; i < aPathList.mSegments.size() && durationLeft > 0; ++i)
   {
      Segment* segmentPtr = aPathList.mSegments[i]->Clone();

      double duration = segmentPtr->GetDuration();
      if (duration > durationLeft)
      {
         segmentPtr->SetDuration(durationLeft);
      }
      mSegments.push_back(segmentPtr);
      durationLeft -= segmentPtr->GetDuration();
   }
   mDuration += aDuration;
}

//! Returns the sim-time required to move along the path
double WsfPathList::GetDuration() const
{
   double duration = mDuration;
   if (mSegments.empty())
   {
      duration = -1.0E-9;
   }
   return duration;
}

const WsfPathState& WsfPathList::GetEndState()
{
   if (!mEndStateValid)
   {
      GetStateP(GetDuration(), mEndState);
   }
   return mEndState;
}

void WsfPathList::GetState(double aRelativeSimTime, WsfPathState& aState)
{
   assert(!mSegments.empty());
   if (aRelativeSimTime >= mDuration)
   {
      aState = GetEndState();
      return;
   }
   GetStateP(aRelativeSimTime, aState);
}

void WsfPathList::GetStateP(double aRelativeSimTime, WsfPathState& aState)
{
   Segment* segmentPtr = mSegments[mLastSegmentIndex];

   if (aRelativeSimTime < mLastSegmentStartTime)
   {
      mLastSegmentIndex     = 0;
      mLastSegmentStartTime = 0.0;
      segmentPtr            = mSegments.front();
   }
   while (mLastSegmentStartTime + segmentPtr->GetDuration() < aRelativeSimTime &&
          mLastSegmentIndex < int(mSegments.size()) - 1)
   {
      mLastSegmentStartTime += segmentPtr->GetDuration();
      segmentPtr = mSegments[++mLastSegmentIndex];
   }
   segmentPtr->ComputeState(aRelativeSimTime - mLastSegmentStartTime, aState);
}

void WsfPathList::PrintPath(ut::log::MessageStream& aStream)
{
   aStream.AddNote() << "Duration: " << mDuration;
   for (size_t i = 0; i < mSegments.size(); ++i)
   {
      Segment* segmentPtr  = mSegments[i];
      auto     segmentNote = aStream.AddNote() << "Segment:";
      segmentPtr->Print(segmentNote);
   }
}
void WsfPathList::Reset()
{
   for (size_t i = 0; i < mSegments.size(); ++i)
   {
      delete mSegments[i];
   }
   mSegments.clear();
   mLastSegmentIndex     = 0;
   mLastSegmentStartTime = 0.0;
   mDuration             = 0.0;
   mEndStateValid        = false;
}

void WsfPathList::PopBack()
{
   if (!mSegments.empty())
   {
      mDuration -= mSegments.back()->GetDuration();
      delete mSegments.back();
      mSegments.pop_back();
      mEndStateValid        = false;
      mLastSegmentStartTime = mLastSegmentIndex = 0;
   }
}


void WsfPathList::RotationData::InitializeRotation(UtVec3d aInitialLocationWCS, UtVec3d aRotationAxisWCS)
{
   mInitialLocationWCS = aInitialLocationWCS;
   mRotationAxis       = aRotationAxisWCS;
   mRotationAxis.Normalize();

   mInitialDirectionWCS.CrossProduct(mInitialLocationWCS, mRotationAxis);
   mInitialDirectionWCS.Normalize();
}
