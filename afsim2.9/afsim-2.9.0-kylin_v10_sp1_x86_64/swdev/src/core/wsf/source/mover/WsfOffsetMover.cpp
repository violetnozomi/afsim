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

#include "WsfOffsetMover.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtQuaternion.hpp"
#include "UtVec3.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"

namespace
{
inline std::ostream& operator<<(std::ostream& aOut, const UtVec3d& aVec)
{
   aOut << " [ " << aVec[0] << ' ' << aVec[1] << ' ' << aVec[2] << " ] ";
   return aOut;
}
} // namespace

WsfOffsetMover::WsfOffsetMover(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , mReferencePtr(nullptr)
   , mReferenceNameId(nullptr)
   , mAttachmentType(cAT_TETHER)
   , mReferenceAttachmentTime(-1.0)
   , mOffsetMagnitude(0.0)
   , mOrphanAction(cOA_STOP)
   , mReferenceDamageFactor(-1.0)
   , mLocalUpdateActive(false)
   , mReferenceUpdateActive(false)
   , mIsLockedOn(true)
   , mOldRefTime(-1.0)
{
   UtVec3d::Set(mOffsetFromReferenceECS, 0.0);
   UtVec3d::Set(mOldRefLocWCS, 0.0);
   UtVec3d::Set(mOldRefVelWCS, 0.0);

   // This mover is not thread-safe because it calls another mover.
   mThreadSafe = false;
}

WsfOffsetMover::WsfOffsetMover(const WsfOffsetMover& aSrc)
   : WsfMover(aSrc)
   , mReferencePtr(nullptr)
   , mReferenceNameId(aSrc.mReferenceNameId)
   , mAttachmentType(aSrc.mAttachmentType)
   , mReferenceAttachmentTime(-1.0)
   , mOffsetMagnitude(aSrc.mOffsetMagnitude)
   , mOrphanAction(aSrc.mOrphanAction)
   , mReferenceDamageFactor(-1.0)
   , mLocalUpdateActive(false)
   , mReferenceUpdateActive(false)
   , mIsLockedOn(true)
   , mOldRefTime(-1.0)

{
   UtVec3d::Set(mOffsetFromReferenceECS, aSrc.mOffsetFromReferenceECS);
   UtVec3d::Set(mOldRefLocWCS, 0.0);
   UtVec3d::Set(mOldRefVelWCS, 0.0);
}

// virtual
WsfOffsetMover::~WsfOffsetMover()
{
   if (mReferencePtr != nullptr)
   {
      mReferencePtr->DetachObserver(this);
   }
}

// virtual
WsfMover* WsfOffsetMover::Clone() const
{
   return new WsfOffsetMover(*this);
}

// virtual
WsfSpatialDomain WsfOffsetMover::GetSpatialDomain() const
{
   if (mReferencePtr != nullptr)
   {
      return mReferencePtr->GetSpatialDomain();
   }
   return WsfMover::GetSpatialDomain();
}

// virtual
bool WsfOffsetMover::Initialize(double aSimTime)
{
   bool ok = WsfMover::Initialize(aSimTime);
   if (mAttachmentType == cAT_TETHER)
   {
      auto out = ut::log::warning() << "'attachment_type tether' is still in development.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Mover: " << GetName();
      out.AddNote() << "Use at your own risk.";
   }
   if (mReferenceNameId == nullptr)
   {
      auto out = ut::log::error() << "Reference platform not specified.";
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Mover: " << GetName();
      ok = false;
   }

   if (ok)
   {
      // Attach to the leader - if it exists
      WsfPlatform* referencePtr = GetSimulation()->GetPlatformByName(mReferenceNameId);
      if (referencePtr != nullptr)
      {
         // If this is a result of a SwapMover call then set leave the initial position as-is and let Update handle
         // things. Otherwise, set the initial position based on the current location of the reference.
         if (aSimTime == GetPlatform()->GetCreationTime())
         {
            // Ensure the reference platform is current.
            referencePtr->Update(aSimTime);
            CopyStateFromReference(aSimTime, referencePtr);
         }

         mReferenceAttachmentTime = aSimTime;
         mReferencePtr            = referencePtr;
         mReferencePtr->AttachObserver(this);
      }
   }
   mLastUpdateTime = aSimTime;
   return ok;
}

// virtual
void WsfOffsetMover::SetPlatform(WsfPlatform* aPlatformPtr)
{
   if ((aPlatformPtr == nullptr) && (mReferencePtr != nullptr))
   {
      mReferencePtr->DetachObserver(this);
      mReferencePtr = nullptr;
   }
   WsfMover::SetPlatform(aPlatformPtr);
}

// virtual
void WsfOffsetMover::PlatformAdded(double aSimTime, WsfPlatform* aPlatformPtr)
{
   // Establish the attachment if platform being added is my leader.
   if ((aPlatformPtr->GetNameId() == mReferenceNameId) &&
       (mReferencePtr != aPlatformPtr)) // Don't establish the attachment twice, if already attached
   {
      mReferencePtr            = aPlatformPtr;
      mReferenceAttachmentTime = aSimTime;
      mOldRefTime              = -1.0;
      mReferencePtr->AttachObserver(this);
   }
}

// virtual
void WsfOffsetMover::PlatformDeleted(WsfPlatform* aPlatformPtr)
{
   // Detach if the platform be deleted is my leader.
   if (aPlatformPtr == mReferencePtr)
   {
      mReferencePtr          = nullptr;
      mReferenceDamageFactor = aPlatformPtr->GetDamageFactor();
   }
}

// virtual
bool WsfOffsetMover::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "attachment_type")
   {
      std::string attachmentType;
      aInput.ReadValue(attachmentType);
      if (attachmentType == "rigid")
      {
         mAttachmentType = cAT_RIGID;
      }
      else if (attachmentType == "tether")
      {
         mAttachmentType = cAT_TETHER;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else if (command == "reference_platform")
   {
      std::string leaderName;
      aInput.ReadValue(leaderName);
      SetReference(0.0, leaderName);
   }
   else if (command == "offset_from_reference")
   {
      double offsetECS[3];
      aInput.ReadValueOfType(offsetECS[0], UtInput::cLENGTH);
      aInput.ReadValueOfType(offsetECS[1], UtInput::cLENGTH);
      aInput.ReadValueOfType(offsetECS[2], UtInput::cLENGTH);
      SetOffsetFromReference(0.0, offsetECS);
   }
   else if (command == "orphan_action")
   {
      std::string orphanOption;
      aInput.ReadValue(orphanOption);
      if (orphanOption == "stop")
      {
         mOrphanAction = cOA_STOP;
      }
      else if (orphanOption == "extrapolate")
      {
         mOrphanAction = cOA_EXTRAPOLATE;
      }
      else if (orphanOption == "remove")
      {
         mOrphanAction = cOA_REMOVE;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else
   {
      myCommand = WsfMover::ProcessInput(aInput);
   }
   return myCommand;
}

//! Establish the specified platform as the leader.
//! @param aSimTime       The current simulation time.
//! @param aReferenceName The name of the platform to which this platform will tether
// virtual
void WsfOffsetMover::SetReference(double aSimTime, const std::string& aReferenceName)
{
   mReferenceNameId = aReferenceName;
   if ((GetPlatform() != nullptr) &&     // The mover is part of a platform (i.e.: not a 'mover type')
       (GetPlatform()->GetIndex() != 0)) // And is part of the simulation.
   {
      // Detach from existing leader.
      if (mReferencePtr != nullptr)
      {
         mReferencePtr->DetachObserver(this);
         mReferencePtr = nullptr;
      }

      // Locate new leader and attach to it (if it exists).
      mReferencePtr = GetSimulation()->GetPlatformByName(mReferenceNameId);
      if (mReferencePtr != nullptr)
      {
         mOldRefTime              = -1.0;
         mReferenceAttachmentTime = aSimTime;
         mReferencePtr->AttachObserver(this);
         GetPlatform()->Update(aSimTime);
      }
   }
}

//! Define my offset from the leader.
//! @param aSimTime                The current simulation time.
//! @param aOffsetFromReferenceECS The ECS offset from the leader to my location.
// virtual
void WsfOffsetMover::SetOffsetFromReference(double aSimTime, const double aOffsetFromReferenceECS[3])
{
   UtVec3d::Set(mOffsetFromReferenceECS, aOffsetFromReferenceECS);
   mOffsetMagnitude = UtVec3d::Magnitude(mOffsetFromReferenceECS);
   if (mReferencePtr != nullptr)
   {
      GetPlatform()->Update(aSimTime);
   }
}

//! The method that is called by virtue of being an observer of the leader platform.
//!
//! This method is called whenever the leader platform (the subject) updates itself.
//! We simply update our kinematic state with that of the leader.
// virtual
void WsfOffsetMover::OnPlatformUpdated(double aSimTime, WsfPlatform* aPlatformPtr)
{
   if (!mLocalUpdateActive)
   {
      mReferenceUpdateActive = true;
      GetPlatform()->Update(mReferencePtr->GetLastUpdateTime());
      mReferenceUpdateActive = false;
   }
}

// virtual
void WsfOffsetMover::Update(double aSimTime)
{
   mLocalUpdateActive = true;
   if (mReferencePtr != nullptr)
   {
      // This is called either directly from the simulation for a normal update or indirectly from when the
      // platform to which we are attached is updated (via OnPlatformUpdated).
      //
      // In the first case, we want to tell the leader to first update its position and then we'll update our position.
      // In the second case, the leader has just completed an update and we don't want to tell it again to update.
      //
      // Do not be tempted to bypass the way this is done. This ensures observers of both platforms get called correctly.

      if (!mReferenceUpdateActive)
      {
         mReferencePtr->Update(aSimTime);
      }

      // Update my position based on the position of the reference platform.
      if (mAttachmentType == cAT_TETHER)
      {
         UpdateTetheredState(aSimTime);
      }
      else
      {
         CopyStateFromReference(aSimTime, mReferencePtr);
      }
   }
   else if (mReferenceAttachmentTime >= 0.0)
   {
      // An attachment existed but has now been lost. Decide what should be done.
      if (mOrphanAction == cOA_EXTRAPOLATE)
      {
         ExtrapolateState(aSimTime);
      }
      else if (mOrphanAction == cOA_REMOVE)
      {
         GetPlatform()->SetDamageFactor(mReferenceDamageFactor);
         GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
      }
      else // should be cOA_STOP
      {
         double zeroVec[3] = {0.0, 0.0, 0.0};
         GetPlatform()->SetVelocityWCS(zeroVec);
         GetPlatform()->SetAccelerationWCS(zeroVec);
         mLastUpdateTime = aSimTime;
      }
   }
   else
   {
      // An attachment has never been made. This platform may have been added to the simulation prior
      // to the host platform. Just wait around for it to come into existence.

      // It may be nice to have some sort of timeout (maybe...)
      mLastUpdateTime = aSimTime;
   }
   mLocalUpdateActive = false;
}

//! Copy the kinematic state from leader.
// private
void WsfOffsetMover::CopyStateFromReference(double aSimTime, WsfPlatform* aReferencePtr)
{
   double locWCS[3];
   double offsetWCS[3];
   aReferencePtr->GetLocationWCS(locWCS);
   aReferencePtr->ConvertECSVectorToWCS(offsetWCS, mOffsetFromReferenceECS);
   UtVec3d::Add(locWCS, locWCS, offsetWCS);
   GetPlatform()->SetLocationWCS(locWCS);

   double velWCS[3];
   aReferencePtr->GetVelocityWCS(velWCS);
   GetPlatform()->SetVelocityWCS(velWCS);

   double aclWCS[3];
   aReferencePtr->GetAccelerationWCS(aclWCS);
   GetPlatform()->SetAccelerationWCS(aclWCS);

   double psi, theta, phi;
   aReferencePtr->GetOrientationWCS(psi, theta, phi);
   GetPlatform()->SetOrientationWCS(psi, theta, phi);

   mLastUpdateTime = aSimTime;
}

//! Extrapolate the position forward in time.
//! This is called when the reference platform has been removed from the simulation
//! and we have been told to extrapolate from our last known position.
// private
void WsfOffsetMover::ExtrapolateState(double aSimTime)
{
   // TODO
   mLastUpdateTime = aSimTime;
}

// private
void WsfOffsetMover::UpdateTetheredState(double aSimTime)
{
   double deltaTime = aSimTime - mLastUpdateTime;
   if (deltaTime < 1.0E-5)
   {
      return;
   }

   double refSpeed = mReferencePtr->GetSpeed();
   if (refSpeed < 0.01)
   {
      return; // Barely moving (~ 0.02 mi/hr)
   }

   // Get the ECS location relative to the reference of the desired target point.
   double tgtLocECS[3];
   UtVec3d::Set(tgtLocECS, mOffsetFromReferenceECS);

   // Get the ECS location relative to the reference of my current location.
   double curLocWCS[3];
   double curLocECS[3];
   GetPlatform()->GetLocationWCS(curLocWCS);
   mReferencePtr->ConvertWCSToECS(curLocWCS, curLocECS);

   // Compute the distance to the desired target point. If the distance is small, exit without changing
   // anything. This prevents radical changes in orientation due to numerical imprecision

   double delLocECS[3];
   UtVec3d::Subtract(delLocECS, tgtLocECS, curLocECS);
   double distToTgtSq = UtVec3d::MagnitudeSquared(delLocECS);
   if (distToTgtSq < 0.01)
   {
      return;
   }
   double distToTgt = sqrt(distToTgtSq);

   // If the target point could be reasonable reached in this timestep, accept the target point as-is.

   if (fabs(deltaTime * refSpeed - distToTgt) <= std::max(1.0, 0.1 * distToTgt))
   {
      if ((!mIsLockedOn) && DebugEnabled())
      {
         auto out = ut::log::debug() << "Platform is now ON track.";
         out.AddNote() << "T = " << aSimTime;
         out.AddNote() << "Platform: " << GetPlatform()->GetName();
      }
      mIsLockedOn = true;
      double tgtLocWCS[3];
      mReferencePtr->ConvertECSToWCS(tgtLocECS, tgtLocWCS);
      GetPlatform()->SetLocationWCS(tgtLocWCS);
      double tgtVelWCS[3];
      mReferencePtr->GetVelocityWCS(tgtVelWCS);
      GetPlatform()->SetVelocityWCS(tgtVelWCS);
      double tgtAclWCS[3];
      mReferencePtr->GetAccelerationWCS(tgtAclWCS);
      GetPlatform()->SetAccelerationWCS(tgtAclWCS);
      double tgtPsi, tgtTheta, tgtPhi;
      mReferencePtr->GetOrientationWCS(tgtPsi, tgtTheta, tgtPhi);
      GetPlatform()->SetOrientationWCS(tgtPsi, tgtTheta, tgtPhi);
      mLastUpdateTime = aSimTime;
      return;
   }

   if (DebugEnabled())
   {
      double pitch, roll, curHeading, refHeading;
      GetPlatform()->GetOrientationNED(curHeading, pitch, roll);
      mReferencePtr->GetOrientationNED(refHeading, pitch, roll);
      curHeading = UtMath::NormalizeAngle0_TwoPi(curHeading);
      refHeading = UtMath::NormalizeAngle0_TwoPi(refHeading);
      auto out   = ut::log::debug() << "Planning heading adjustment.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Current Heading: " << curHeading * UtMath::cDEG_PER_RAD;
      out.AddNote() << "Reference Heading: " << refHeading * UtMath::cDEG_PER_RAD;
   }
   mIsLockedOn = false;

   // It has been determined that the target point cannot be reached trivially in this timestep, so we need
   // to determine in which way to turn, how much, and how fast to go.  One thing we will NEVER do is to turn
   // away from the heading of the reference platform.

   double curVelWCS[3];
   GetPlatform()->GetVelocityWCS(curVelWCS);
   double curSpeed = UtVec3d::Magnitude(curVelWCS);
   double curVelECS[3];
   mReferencePtr->ConvertWCSVectorToECS(curVelECS, curVelWCS);

   double tgtVelECS[3] = {refSpeed, 0.0, 0.0};
   double intLocECS[3];
   UtVec3d::Set(intLocECS, tgtLocECS);

   // If the target point is behind us, move it forward so we don't turn around.
   if (curLocECS[0] < tgtLocECS[0])
   {
      intLocECS[0] = curLocECS[0] + (2.0 * curSpeed);
   }

   // Compute the line of sight rate of the 'target'.

   double delVelECS[3];
   UtVec3d::Subtract(delVelECS, tgtVelECS, curVelECS);
   UtVec3d::Subtract(delLocECS, intLocECS, curLocECS);
   double omegaECS[3];
   UtVec3d::CrossProduct(omegaECS, delLocECS, delVelECS);
   double rMag = UtVec3d::Magnitude(delLocECS);
   // double vMag  = UtVec3d::Magnitude(delVelECS);
   double rvMag = UtVec3d::Magnitude(omegaECS);
   double omega = rvMag / (rMag * rMag);
   double yaw   = omega * deltaTime;
   // double sinTheta = rvMag / (rMag * vMag);
   // double theta = asin(sinTheta);
   // if (omegaECS[2] < 0.0)
   //{
   //    // left turn
   // }
   // else
   //{
   //    // right turn
   //    theta = -theta;
   // }

   double dcm[3][3];
   UtVec3d::Normalize(dcm[0], curVelECS);
   UtVec3d::Normalize(dcm[2], omegaECS);
   UtVec3d::CrossProduct(dcm[1], dcm[2], dcm[0]);

   UtQuaternion q1(dcm);
   double       rXYZ[] = {0.0, 0.0, yaw};
   UtQuaternion q2     = q1.Propagate(rXYZ, deltaTime);

   double newLocECS[3];
   double dcm2[3][3];
   q2.Get(dcm2);
   UtVec3d::Multiply(newLocECS, dcm2[0], distToTgt);
   UtVec3d::Add(newLocECS, curLocECS, newLocECS);
   double newSpeed = refSpeed;

   // Calculate the new speed...

   UtVec3d::Subtract(delLocECS, newLocECS, curLocECS);
   double distToNew = UtVec3d::Magnitude(delLocECS);

   double maxAccel = 2.0;
   if ((curSpeed * deltaTime) < distToNew)
   {
      // We'll fall behind the point if we continue at the current speed.
      newSpeed = distToNew / deltaTime;
      if ((newSpeed - curSpeed) / deltaTime > maxAccel)
      {
         newSpeed = curSpeed + deltaTime * maxAccel;
      }
   }
   else
   {
      // We'll overshoot the point if we continue at the current speed.
      newSpeed = distToNew / deltaTime;
      if ((newSpeed - curSpeed) / deltaTime < -maxAccel)
      {
         newSpeed = curSpeed - deltaTime * maxAccel;
      }
   }

   newSpeed = std::min(newSpeed, 1.5 * refSpeed);
   newSpeed = std::max(newSpeed, 0.5 * refSpeed);

   // Compute new location using speed and vector to target location....
   UtVec3d::Normalize(delLocECS);
   UtVec3d::Multiply(delLocECS, newSpeed * deltaTime);
   UtVec3d::Add(newLocECS, curLocECS, delLocECS);

   // Update my location with the new location.
   double newLocWCS[3];
   mReferencePtr->ConvertECSToWCS(newLocECS, newLocWCS);
   GetPlatform()->SetLocationWCS(newLocWCS);

   // Update my velocity. The new velocity vector will be aligned with the direction vector to the new point.
   double newVelWCS[3];
   UtVec3d::Subtract(newVelWCS, newLocWCS, curLocWCS);
   UtVec3d::Normalize(newVelWCS);
   UtVec3d::Multiply(newVelWCS, newSpeed);
   GetPlatform()->SetVelocityWCS(newVelWCS);

   // Set the acceleration to zero.
   double newAclWCS[3] = {0.0, 0.0, 0.0};
   GetPlatform()->SetAccelerationWCS(newAclWCS);

   // Set the orientation so it aligns with the velocity vector.
   double newVelNED[3];
   GetPlatform()->ConvertWCSVectorToNED(newVelNED, newVelWCS);
   double heading = atan2(newVelNED[1], newVelNED[0]);
   double pitch   = asin(-newVelNED[2] / newSpeed);
   double roll    = 0.0;
   GetPlatform()->SetOrientationNED(heading, pitch, roll);
   if (DebugEnabled())
   {
      auto out = ut::log::debug() << "Updated velocity and heading.";
      out.AddNote() << "T = " << aSimTime;
      out.AddNote() << "Platform: " << GetPlatform()->GetName();
      out.AddNote() << "Time Since Update:" << deltaTime;
      out.AddNote() << "Distance To Target: " << distToTgt;
      out.AddNote() << "Velocity: " << newSpeed;
      out.AddNote() << "Heading: " << heading * UtMath::cDEG_PER_RAD;
      out.AddNote() << "Pitch: " << pitch * UtMath::cDEG_PER_RAD;
   }

   mLastUpdateTime = aSimTime;
}
