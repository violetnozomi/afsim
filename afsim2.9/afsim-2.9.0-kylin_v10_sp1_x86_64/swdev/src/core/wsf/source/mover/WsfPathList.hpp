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

#ifndef WSFPATHLIST_HPP
#define WSFPATHLIST_HPP

#include "wsf_export.h"

#include <vector>

#include "UtEntity.hpp"
#include "UtLog.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "WsfPathState.hpp"

//! Defines a list of motions a platform could take
class WSF_EXPORT WsfPathList
{
public:
   //! Defines an entity's segment from one state to another over time
   class WSF_EXPORT Segment
   {
   public:
      Segment()
         : mInitialState()
         , mDuration(0.0)
         , mRollRate(0.0)
         , mIsApproximation(false)
      {
      }
      virtual ~Segment() = default;

      //! Computes a state along this segment
      //! @param aRelativeSimTime The length of simulation-time after the segment started
      //! @param aState The state at aRelativeSimTime along this segment
      virtual void ComputeState(double aRelativeSimTime, WsfPathState& aState) = 0;

      //! Returns a copy of the segment
      virtual Segment* Clone() const = 0;

      //! Initialize the basic members of the segment
      void Initialize(double aDuration, const WsfPathState& aState)
      {
         mDuration     = aDuration;
         mInitialState = aState;
      }

      void SetRollRate(double aRollRate) { mRollRate = aRollRate; }

      //! Returns the state at the start of the segment
      const WsfPathState& GetInitialState() const { return mInitialState; }

      //! Returns the length of time to complete this segment
      double GetDuration() const { return mDuration; }
      //! Sets the length of time to complete this segment
      void SetDuration(double aDuration) { mDuration = aDuration; }

      //! Appends data as notes to aStream.
      virtual void Print(ut::log::MessageStream& aStream);

      //! The initial state of the segment.  This should be equivalent to ComputeState(0.0)
      WsfPathState mInitialState;
      //! The length of simulation time this duration takes to complete
      double mDuration;
      //! The roll rate used when changing roll angle
      double mRollRate;
      //! Indicates this segment is an approximation
      bool mIsApproximation;
   };

   //! Contains data about the acceleration and altitude changes during a segment
   class DynamicsData
   {
   public:
      DynamicsData()
         : mLinearAcceleration()
         , mClimbRate()
         , mFlightAngle()
      {
      }
      void SetLinearAcceleration(double aAcceleration) { mLinearAcceleration = aAcceleration; }

      void SetClimbRate(double aClimbRate) { mClimbRate = aClimbRate; }

      void SetFlightAngle(double aFlightAngle) { mFlightAngle = aFlightAngle; }
      //! Linear acceleration applied to the duration of the segment
      double mLinearAcceleration;
      //! Climb rate applied to the duration of the segment
      double mClimbRate;
      //! Flight angle applied to the duration of the segment
      double mFlightAngle;
   };

   //! Contains data about a rotation.
   class RotationData
   {
   public:
      RotationData()
         : mRotationRadius(UtSphericalEarth::cEARTH_RADIUS)
      {
      }

      void InitializeRotation(UtVec3d aInitialLocationWCS, UtVec3d aRotationAxisWCS);

      void SetRotationRadius(double aEarthRadius) { mRotationRadius = aEarthRadius; }
      //! Radius of the rotation.  (radius of turn, or radius of earth)
      double mRotationRadius;
      //! Initial location in WCS.  This is rotated to find new position.
      UtVec3d mInitialLocationWCS;
      //! Axis of rotation.  May represent a turn, or straight flight (around a spherical earth)
      UtVec3d mRotationAxis;
      //! Initial direction in WCS.  This is rotated to find new direction.
      UtVec3d mInitialDirectionWCS;
   };

   //! Represents a rotation around an axis, using constant speed / altitude.
   class RotationArcSegment : public WsfPathList::Segment, public WsfPathList::RotationData
   {
   public:
      RotationArcSegment()
         : mRoll(0.0)
      {
      }
      Segment* Clone() const override { return new RotationArcSegment(*this); }
      void     ComputeState(double aRelativeSimTime, WsfPathState& aState) override;
      void     SetTargetRoll(double aRoll) { mRoll = aRoll; }
      void     Print(ut::log::MessageStream& aStream) override;

      double mRoll;
   };

   //! Represents a rotation around an axis.  Behaves using values in DynamicsData.
   class RotationSegment : public WsfPathList::Segment, public WsfPathList::RotationData, public WsfPathList::DynamicsData
   {
   public:
      RotationSegment()
         : mRoll(0.0)
      {
      }
      Segment* Clone() const override { return new RotationSegment(*this); }
      void     ComputeState(double aRelativeSimTime, WsfPathState& aState) override;
      void     SetTargetRoll(double aRoll) { mRoll = aRoll; }
      void     Print(ut::log::MessageStream& aStream) override;

      double mRoll;
   };

   //! Represents a length of time where the entity is not moving.  Duration may be zero
   //! to designate a new position.
   class WSF_EXPORT PauseSegment : public WsfPathList::Segment
   {
   public:
      Segment* Clone() const override { return new PauseSegment(*this); }
      void     ComputeState(double aRelativeSimTime, WsfPathState& aState) override { aState = mInitialState; }
      void     Print(ut::log::MessageStream& aStream) override;
   };

   typedef std::vector<Segment*> SegmentList;

   WsfPathList();
   WsfPathList(const WsfPathList& aSrc);
   ~WsfPathList();
   WsfPathList& operator=(const WsfPathList& aRhs) = delete;

   //! Returns the state at a time offset from the beginning of the path
   void GetState(double aRelativeSimTime, WsfPathState& aState);

   //! Removes all segments from the path
   void Reset();

   //! Appends a segment to the path.
   void Append(Segment* aSegmentPtr);

   //! Appends a segment to the path, and sets the related end state.
   void Append(Segment* aSegmentPtr, const WsfPathState& aEndState);

   void Append(double aDuration, WsfPathList& aPathList);

   void PopBack();

   double GetDuration() const;

   double GetEndStateValid() { return mEndStateValid; }

   //! Returns true if the path list is empty
   bool Empty() const { return mSegments.empty(); }

   //! Returns the state at the end of the path
   //! Not valid when Empty()
   const WsfPathState& GetEndState();

   //! Sets the final state at the end of the path.
   void SetEndState(WsfPathState& aState)
   {
      mEndState      = aState;
      mEndStateValid = true;
   }

   //! returns the list of segments in the path
   const SegmentList& GetSegments() const { return mSegments; }

   void PrintPath(ut::log::MessageStream& aStream);


private:
   void GetStateP(double aRelativeSimTime, WsfPathState& aState);

   double                mDuration;
   int                   mLastSegmentIndex;
   double                mLastSegmentStartTime;
   std::vector<Segment*> mSegments;
   WsfPathState          mEndState;
   bool                  mEndStateValid;
};

#endif
