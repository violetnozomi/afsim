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

#include "WsfTSPI_Mover.hpp"

#include <cstdio>  // for std::remove()
#include <cstdlib> // for system()
#include <fstream>
#include <sstream>

#include "UtDCM.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "UtEntity.hpp"
#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtMath.hpp"
#include "UtQuaternion.hpp"
#include "UtSphericalEarth.hpp"
#include "UtVec3.hpp"
#include "UtVec3dX.hpp"
#include "WsfPlatform.hpp"
#include "WsfScenario.hpp"
#include "WsfSimulation.hpp"
#include "WsfSystemLog.hpp"

//-------------------------------------------------------------------------------------------------
WsfTSPI_Mover::WsfTSPI_Mover(WsfScenario& aScenario)
   : WsfMover(aScenario)
   , mInputStreamPtr(nullptr)
   , mFileName()
   , mIsOpen(false)
   , mEOF(false)
   , mIsInitialized(false)
   , mTranslate(false)
   , mStartAtInitialTime(false)
   , mDeleteOnDestruct(false)
   , mIsExtrapolating(false)
   , mAtEndOfPath(cEP_EXTRAPOLATE)
   , mStartTime(0.0)
   , mTSPI_StartTime(0.0)
   , mCurrentUpdateTime(0.0)
   , mDeltaLat(0.0)
   , mDeltaLon(0.0)
   , mRefLat(0.0)
   , mRefLon(0.0)
   , mLastTSPI()
   , mCurrentTSPI()
   , mReadTSPI()
   , mMovement()
{
}

//-------------------------------------------------------------------------------------------------
// virtual
WsfTSPI_Mover::~WsfTSPI_Mover()
{
   CloseFile();
   delete mInputStreamPtr;
}

bool WsfTSPI_Mover::SetFileName(const std::string& aName)
{
   if (!mIsOpen)
   {
      mFileName = GetScenario().GetInput().LocateFile(aName);
      return true;
   }
   return false;
}

//-------------------------------------------------------------------------------------------------
// virtual
bool WsfTSPI_Mover::ProcessInput(UtInput& aInput)
{
   bool        ok = true;
   std::string command;
   aInput.GetCommand(command);

   if ((command == "filename") || (command == "TSPI_filename") || (command == "tspi_filename"))
   {
      std::string fn;
      aInput.ReadValue(fn);
      SetFileName(fn); // will provide variable substitution
   }
   else if (command == "start_time")
   {
      aInput.ReadValueOfType(mStartTime, UtInput::cTIME);
      mStartAtInitialTime = false;
   }
   else if (command == "start_at_initial_time")
   {
      mStartAtInitialTime = true;
   }
   else if (command == "delete_on_destruct")
   {
      mDeleteOnDestruct = true;
   }
   else if (command == "extrapolation")
   {
      bool extrapolation;
      aInput.ReadValue(extrapolation);
      mAtEndOfPath = (extrapolation) ? cEP_EXTRAPOLATE : cEP_STOP;
   }
   else if (command == "translate_to")
   {
      mTranslate = true;
      aInput.ReadValueOfType(mRefLat, UtInput::cLATITUDE);
      aInput.ReadValueOfType(mRefLon, UtInput::cLONGITUDE);
   }
   else if (command == "relocate_and_rotate")
   {
      ok &= ProcessInputRelocateAndRotate(aInput);
   }
   else if (command == "at_end_of_path")
   {
      std::string endOfPathOption;
      aInput.ReadValue(endOfPathOption);
      if (endOfPathOption == "extrapolate")
      {
         mAtEndOfPath = cEP_EXTRAPOLATE;
      }
      else if (endOfPathOption == "stop")
      {
         mAtEndOfPath = cEP_STOP;
      }
      else if (endOfPathOption == "remove")
      {
         mAtEndOfPath = cEP_REMOVE;
      }
      else
      {
         throw UtInput::BadValue(aInput);
      }
   }
   else
   {
      ok = WsfMover::ProcessInput(aInput);
   }

   if (!ok)
   {
      ok = mReadTSPI.ProcessInput(aInput); // Used to read from file.
   }
   return ok;
}

bool WsfTSPI_Mover::ProcessInputRelocateAndRotate(UtInput& aInput)
{
   bool providedLL  = false;
   bool providedRot = false;

   UtInputBlock block(aInput);
   while (block.ReadCommand())
   {
      UtInput&    input      = block.GetInput();
      std::string newCommand = input.GetCommand();
      if (newCommand == "great_circle_heading_at_anchor_point")
      {
         if (providedRot)
         {
            throw UtInput::BadValue(aInput, "Cannot provide more than one rotation.");
         }
         input.ReadValueOfType(mMovement.mGC_AnchorHeadingRad.emplace(), UtInput::cANGLE);
         providedRot = true;
      }
      else if (newCommand == "align_to_great_circle_through")
      {
         if (providedRot)
         {
            throw UtInput::BadValue(aInput, "Cannot provide more than one rotation.");
         }
         input.ReadValueOfType(mMovement.mGreatCircleLat.emplace(), UtInput::cLATITUDE);
         input.ReadValueOfType(mMovement.mGreatCircleLon.emplace(), UtInput::cLONGITUDE);
         providedRot = true;
      }
      else if ((newCommand == "terminal_endpoint_anchor") || (newCommand == "initial_endpoint_anchor"))
      {
         if (providedLL)
         {
            throw UtInput::BadValue(aInput, "Must not provide more than one *_endpoint_anchor.");
         }
         input.ReadValueOfType(mRefLat, UtInput::cLATITUDE);
         input.ReadValueOfType(mRefLon, UtInput::cLONGITUDE);
         providedLL                    = true;
         mMovement.mStartPointIsAnchor = (newCommand == "initial_endpoint_anchor");
      }
      else
      {
         throw UtInput::UnknownCommand(input);
      }
   }

   mMovement.mTransformIsNeeded = providedLL || providedRot;
   mMovement.mTranslateIsNeeded = providedLL;

   return true;
}

//-------------------------------------------------------------------------------------------------
// virtual
bool WsfTSPI_Mover::Initialize(double aSimTime)
{
   // Initialize base
   bool ok = WsfMover::Initialize(aSimTime);

   // Initialize TSPI file and data
   mInputStreamPtr = new std::ifstream();
   OpenFile(mFileName);
   *mInputStreamPtr >> mReadTSPI;
   mCurrentTSPI    = mReadTSPI;
   mTSPI_StartTime = mCurrentTSPI.mTime();

   if (mMovement.mTransformIsNeeded)
   {
      // Completely rewrite the trajectory file, translating and rotating to have
      // the terminal endpoint at a desired location.  Swap the stream to a new
      // file that we rewrite during the call below:
      TranslateAndRotateToNewFile();
   }

   GetPlatform()->SetLocationLLA(mCurrentTSPI.mLat(), mCurrentTSPI.mLon(), mCurrentTSPI.mAlt());
   double speed          = mCurrentTSPI.mSpeed();
   double heading        = mCurrentTSPI.mHeading();
   double velocityNED[3] = {speed * std::cos(heading), speed * std::sin(heading), 0.0};
   GetPlatform()->SetVelocityNED(velocityNED);
   GetPlatform()->SetOrientationNED(mCurrentTSPI.mHeading(), mCurrentTSPI.mPitch(), mCurrentTSPI.mRoll());

   if (mStartAtInitialTime)
   {
      mStartTime = mTSPI_StartTime;
   }
   else
   {
      mStartTime += aSimTime;
   }

   if (mTranslate)
   {
      mDeltaLat = mRefLat - mCurrentTSPI.mLat();
      mDeltaLon = mRefLon - mCurrentTSPI.mLon();
   }

   // Wait until the event is triggered to Update
   SetUpdateInterval(0.0);

   // Schedule the initialization event
   GetSimulation()->AddEvent(ut::make_unique<InitializeMoverEvent>(mStartTime, this));

   return ok;
}

//-------------------------------------------------------------------------------------------------
void WsfTSPI_Mover::TranslateAndRotateToNewFile()
{
   // Procedure:
   // 1. Read in each point from the TSPI File.
   // 2. Compute the (multi-rotation) translation needed to put trajectory end point at desired end point location.
   // 3. Compute the compass rotation about the end point needed to put start in the GC heading direction.
   // 4. Write out a new (translated and rotated) TSPI file.
   // 5. Hijack the stream pointer to point to the new TSPI file, versus the old one.

   // ==== ALL TRANSLATIONS ARE APPROXIMATE, AS LOCAL DOWN ON AN OBLATE EARTH MESSES THINGS UP ====
   // Note that in some (all ?) instances, the platform's location is not yet valid, so we cannot use it.)

   std::vector<Point> points = {mCurrentTSPI};

   // Read all the TSPI points out of the file that is open, and store them.
   // (Fetching a TSPI always places it into mCurrentTSPI.)
   while (Fetch_TSPI())
   {
      points.emplace_back(mCurrentTSPI);
   }

   // Compute the delta longitude (Z rotation) needed to move the original anchor
   // point to the desired new anchor location in the XZ plane (y= 0.0, or Lon = 0).
   std::vector<Point>::iterator anchorIter    = points.begin();
   std::vector<Point>::iterator nonAnchorIter = points.begin();

   if (mMovement.mStartPointIsAnchor)
   {
      nonAnchorIter = points.end();
      --nonAnchorIter;
   }
   else
   {
      anchorIter = points.end();
      --anchorIter;
   }

   const UtVec3dX& anchorPtWCS    = anchorIter->mOrigLocWCS_SL;
   const UtVec3dX& nonAnchorPtWCS = nonAnchorIter->mOrigLocWCS_SL;

   const UtDCM translationMatrix =
      mMovement.mTranslateIsNeeded ? ComputeTranslationTransform(anchorPtWCS, mRefLat, mRefLon) : UtDCM::cIDENTITY;

   const UtDCM rotationMatrix =
      ComputeRotationTransform(anchorPtWCS * translationMatrix, nonAnchorPtWCS * translationMatrix, mMovement);
   const UtDCM fullTransform = rotationMatrix * translationMatrix;

   TransformPoints(points, fullTransform);

   // Write a new TSPI file to replace the old one:
   // Must use the platform index to assure the file name is unique.
   // string outName("./temp_out.txt"); // Old Name
   std::ostringstream oss;
   oss << "./tspi_relocate_and_rotate_out_" << GetPlatform()->GetIndex() << ".txt";
   std::string outName = oss.str();

   std::ofstream outputStream{outName};
   if (outputStream.is_open())
   {
      for (const auto& point : points)
      {
         outputStream << point.mMovedTSPI;
      }
   }
   outputStream.close();

   // Initialize the TSPI file and data.
   //  (Hijack old mInputStreamPtr)
   mInputStreamPtr->close();
   delete mInputStreamPtr;
   mInputStreamPtr = nullptr;
   mInputStreamPtr = new std::ifstream();
   OpenFile(outName);
   *mInputStreamPtr >> mReadTSPI;
   mCurrentTSPI    = mReadTSPI;
   mTSPI_StartTime = mCurrentTSPI.mTime();
}

// static
UtDCM WsfTSPI_Mover::ComputeTranslationTransform(const UtVec3dX& aAnchor, double aTargetLat, double aTargetLon)
{
   double toLat, toLon, toAlt;
   UtEllipsoidalEarth::ConvertEllipsoidalToSphericalLLA(aTargetLat, aTargetLon, 0.0, toLat, toLon, toAlt);

   // Compute rotation angle needed to move aAnchor to prime meridian.
   const double Rz = std::atan2(aAnchor.Get(1), aAnchor.Get(0));
   const UtDCM  firstRot(Rz, 0.0, 0.0);

   // Recompute the new translated anchor point:
   // Now from there compute delta latitude needed to move the final point to the desired latitude.
   auto newAnchor = aAnchor * firstRot;

   const double Ry = -toLat * UtMath::cRAD_PER_DEG + std::asin(newAnchor.Get(2) / newAnchor.MagXZ());
   const UtDCM  secondRot(0.0, -Ry, 0.0);

   // Now the next rotation places the impact point at the correct (final) longitude.
   const double Rz2 = toLon * UtMath::cRAD_PER_DEG;
   const UtDCM  thirdRot(-Rz2, 0.0, 0.0);

   return thirdRot * secondRot * firstRot;
}

// static
UtDCM WsfTSPI_Mover::ComputeRotationTransform(const UtVec3dX& aAnchor, const UtVec3dX& aNonAnchor, const PathMovement& aMovement)
{
   // Now determine what rotation is needed to place all in the right compass orientation.
   // There were two ways to set the heading, either a) Heading is set,
   // or b) the mGreatCircleLat, Lon were overridden.  We need a valid mAnchorHeadingRad.

   // GCC 4.8.5 Release builds complain about uninitialized variables if anchorHeadingRad is initialized during construction.
   ut::optional<double> anchorHeadingRad;
   anchorHeadingRad = aMovement.mGC_AnchorHeadingRad;

   double anchorLat, anchorLon, anchorAlt, GC_DistNotUsed;
   UtEntity::ConvertWCSToLLA(aAnchor.GetData(), anchorLat, anchorLon, anchorAlt);

   if (aMovement.mGreatCircleLat && aMovement.mGreatCircleLon)
   {
      // Compute anchor point heading, given the mGreatCircleLat, Lon not on the anchor point:
      double oppositeDesiredAnchorHeadingDeg;
      UtSphericalEarth::GreatCircleHeadingAndDistance(anchorLat,
                                                      anchorLon,
                                                      *aMovement.mGreatCircleLat,
                                                      *aMovement.mGreatCircleLon,
                                                      oppositeDesiredAnchorHeadingDeg,
                                                      GC_DistNotUsed);
      anchorHeadingRad = UtMath::NormalizeAngleMinusPi_Pi((oppositeDesiredAnchorHeadingDeg + 180) * UtMath::cRAD_PER_DEG);
   }

   if (anchorHeadingRad)
   {
      // Now find out what the current heading actually is, in order to compute rotation needed:
      double nonAnchorLat, nonAnchorLon, nonAnchorAlt;
      UtEntity::ConvertWCSToLLA(aNonAnchor.GetData(), nonAnchorLat, nonAnchorLon, nonAnchorAlt);

      double inverseCurrentHeadingDeg;
      UtSphericalEarth::GreatCircleHeadingAndDistance(anchorLat,
                                                      anchorLon,
                                                      nonAnchorLat,
                                                      nonAnchorLon,
                                                      inverseCurrentHeadingDeg,
                                                      GC_DistNotUsed);
      const double currentHdg = (inverseCurrentHeadingDeg + 180) * UtMath::cRAD_PER_DEG;

      const double desiredRotation = UtMath::NormalizeAngleMinusPi_Pi(*anchorHeadingRad - currentHdg);

      // Use a quaternion as the way to get an arbitrary rotation about a unit vector.
      // (This ability in a quaternion just totally impresses me!)
      return UtDCM(UtQuaternion(desiredRotation, aAnchor));
   }

   return UtDCM::cIDENTITY;
}

// static
void WsfTSPI_Mover::TransformPoints(std::vector<Point>& aPoints, const UtDCM& aTransform)
{
   for (auto& point : aPoints)
   {
      // Now traverse each TSPI point and convert from
      // the orig to moved LL, via FullRotation.

      // First convert the XYZ, assuming (slightly incorrectly) spherical earth.
      // (We already ignored the altitude.)
      point.mMovedLocWCS_SL = point.mOrigLocWCS_SL * aTransform;

      // Now convert the orientation:
      point.mMovedFRDinXYZ = point.mOrigFRDinXYZ * aTransform;

      // Convert from mMovedLocWCS to moved LL, spherical earth.
      double movedLatDeg, movedLonDeg, altShouldBeZeroNotUsed;
      UtEntity::ConvertWCSToLLA(point.mMovedLocWCS_SL.GetData(), movedLatDeg, movedLonDeg, altShouldBeZeroNotUsed);

      // (Altitude has already been set same as original path.)
      point.mMovedTSPI.mLat.Set(movedLatDeg);
      point.mMovedTSPI.mLon.Set(movedLonDeg);

      // Now recover the attribute in the mMovedTSPI that has not been properly set yet,
      // which is the local compass heading.  First we need the WCStoNED DCM:
      // (Get is in degrees.)
      UtEntity tempEntity;
      tempEntity.SetLocationLLA(point.mMovedTSPI.mLat.Get(), point.mMovedTSPI.mLon.Get(), 0.0);
      UtDCM tempDcmWCStoNED = tempEntity.GetDcmWCStoNED();

      // Now that we have the WCStoNED DCM, take the moved/rotated body pointing vector,
      // and convert to NED, which will give us a heading value.  Then tweak this by the
      // original heading value that is path relative:
      UtVec3dX bodyPointingVectorNED = point.mMovedFRDinXYZ * tempDcmWCStoNED;
      double   localHeadingRad       = 0.0;
      if (bodyPointingVectorNED.MagXY() > 0.0001)
      {
         localHeadingRad = std::atan2(bodyPointingVectorNED[1], bodyPointingVectorNED[0]);
      }
      // double localHeadingDeg = localHeadingRad * UtMath::cDEG_PER_RAD;

      point.mMovedTSPI.mHeading.Set(localHeadingRad); // * UtMath::cDEG_PER_RAD);
   }
}

WsfTSPI_Mover::InitializeMoverEvent::InitializeMoverEvent()
   : WsfEvent()
   , mMoverPtr(nullptr)
{
}

WsfTSPI_Mover::InitializeMoverEvent::InitializeMoverEvent(double aSimTime, WsfTSPI_Mover* aMoverPtr)
   : WsfEvent(aSimTime)
   , mMoverPtr(aMoverPtr)
{
}

//-------------------------------------------------------------------------------------------------
// virtual
WsfEvent::EventDisposition WsfTSPI_Mover::InitializeMoverEvent::Execute()
{
   if (mMoverPtr != nullptr)
   {
      mMoverPtr->InitializeMover(GetTime());
   }
   return WsfEvent::cDELETE;
}

//-------------------------------------------------------------------------------------------------
//! Initialize the mover (called by InitializeMoverEvent::Execute()).
//! @note: The mover can't actually be initialized until mStartTime >= aSimTime.
bool WsfTSPI_Mover::InitializeMover(double aSimTime)
{
   mCurrentUpdateTime = mStartTime;
   ReadTSPI();
   mIsInitialized = true;
   return true;
}

//-------------------------------------------------------------------------------------------------
//! Fix Latitude and Longitude within conventional limits.
void WsfTSPI_Mover::NormalizeLatLonDeg(double& aLatitudeDeg, double& aLongitudeDeg)
{
   // Wrap lat/lon if we go over the boundaries.
   if (aLongitudeDeg > 180.0)
   {
      aLongitudeDeg -= 180.0;
   }
   else if (aLongitudeDeg < -180.0)
   {
      aLongitudeDeg += 180.0;
   }
   if (aLatitudeDeg > 90.0)
   {
      aLatitudeDeg = 180.0 - aLatitudeDeg;
      if (aLongitudeDeg < 0.0)
      {
         aLongitudeDeg += 180.0;
      }
      else
      {
         aLongitudeDeg -= 180.0;
      }
   }
   else if (aLatitudeDeg < -90.0)
   {
      aLatitudeDeg = -180.0 + aLatitudeDeg;
      if (aLongitudeDeg < 0.0)
      {
         aLongitudeDeg += 180.0;
      }
      else
      {
         aLongitudeDeg -= 180.0;
      }
   }
}

//-------------------------------------------------------------------------------------------------
// private
void WsfTSPI_Mover::TranslateLocation()
{
   // Translate the TSPI position relative to the user-provided reference
   double lat = mCurrentTSPI.mLat() + mDeltaLat;
   double lon = mCurrentTSPI.mLon() + mDeltaLon;

   NormalizeLatLonDeg(lat, lon);

   mCurrentTSPI.mLat = lat;
   mCurrentTSPI.mLon = lon;
}

//-------------------------------------------------------------------------------------------------
// private
void WsfTSPI_Mover::ReadTSPI()
{
   *mInputStreamPtr >> mReadTSPI;
   mEOF = mInputStreamPtr->eof();
   if (!mEOF)
   {
      mLastTSPI    = mCurrentTSPI;
      mCurrentTSPI = mReadTSPI;

      if (mTranslate)
      {
         TranslateLocation();
      }

      // Calculate when to expect the next update.
      mLastUpdateTime    = mCurrentUpdateTime;
      mCurrentUpdateTime = mStartTime + (mCurrentTSPI.mTime() - mTSPI_StartTime);
   }
}

//-------------------------------------------------------------------------------------------------
// private
bool WsfTSPI_Mover::Fetch_TSPI()
{
   *mInputStreamPtr >> mReadTSPI;
   if (!mInputStreamPtr->eof())
   {
      mLastTSPI    = mCurrentTSPI;
      mCurrentTSPI = mReadTSPI;
      return true;
   }
   return false;
}

//-------------------------------------------------------------------------------------------------
// Implementation of the base class method.
void WsfTSPI_Mover::Update(double aSimTime)
{
   static const double zero[3] = {0.0, 0.0, 0.0};

   if (mIsInitialized)
   {
      // Schedule the next update
      while ((mCurrentUpdateTime < aSimTime) && (!mEOF))
      {
         ReadTSPI();
         if (!mEOF)
         {
            SetUpdateInterval(mCurrentUpdateTime - aSimTime);
         }
      }

      if (!mEOF)
      {
         UpdateMover(aSimTime);
      }
      else
      {
         if (mAtEndOfPath == cEP_REMOVE)
         {
            GetPlatform()->SetLocationLLA(mCurrentTSPI.mLat(), mCurrentTSPI.mLon(), mCurrentTSPI.mAlt());
            GetSimulation()->DeletePlatform(aSimTime, GetPlatform());
         }
         else if (mAtEndOfPath == cEP_STOP)
         {
            GetPlatform()->SetLocationLLA(mCurrentTSPI.mLat(), mCurrentTSPI.mLon(), mCurrentTSPI.mAlt());
            GetPlatform()->SetVelocityWCS(zero);
            GetPlatform()->SetAccelerationWCS(zero);
         }
         else
         {
            // Perform a great circle extrapolation from the last point...
            double speed    = GetPlatform()->GetSpeed();
            double distance = speed * (aSimTime - mLastUpdateTime);
            if (distance > 0.0)
            {
               double curLat, curLon, curAlt;
               GetPlatform()->GetLocationLLA(curLat, curLon, curAlt);
               double newLat, newLon;
               double heading, pitch, roll;
               GetPlatform()->GetOrientationNED(heading, pitch, roll);
               UtSphericalEarth::ExtrapolateGreatCirclePosition(curLat,
                                                                curLon,
                                                                heading * UtMath::cDEG_PER_RAD,
                                                                distance,
                                                                newLat,
                                                                newLon);
               GetPlatform()->SetLocationLLA(newLat, newLon, curAlt);

               // Get the great circle heading from the new point to the current point, and then reverse
               // to get the new heading.

               UtSphericalEarth::GreatCircleHeadingAndDistance(newLat, newLon, curLat, curLon, heading, distance);
               heading *= UtMath::cRAD_PER_DEG;
               heading += UtMath::cPI;
               double velocityNED[3] = {speed * cos(heading), speed * sin(heading), 0.0};
               GetPlatform()->SetVelocityNED(velocityNED);
               GetPlatform()->SetAccelerationNED(zero);
               GetPlatform()->SetOrientationNED(heading, 0.0, 0.0);
            }
         }
         mLastUpdateTime = aSimTime;
      }
   }
}

//-------------------------------------------------------------------------------------------------
// Implementation of the base class method.
// virtual
bool WsfTSPI_Mover::IsStopped() const
{
   return mEOF && (GetPlatform()->GetSpeed() == 0.0);
}

//-------------------------------------------------------------------------------------------------
// Implementation of the base class method.
// virtual
bool WsfTSPI_Mover::IsPaused() const
{
   return IsStopped();
}

//-------------------------------------------------------------------------------------------------
// Implementation of the base class method.
// virtual
bool WsfTSPI_Mover::IsExtrapolating() const
{
   // For a TSPI mover, there are two possible indications of extrapolation.
   bool firstIndication = mEOF && (GetPlatform()->GetSpeed() > 0.0);
   return firstIndication || mIsExtrapolating;
}

WsfMover::KinematicModel* WsfTSPI_Mover::CreateKinematicModel(double       aSimTime,
                                                              WsfStringId  aModelTypeId,
                                                              const double aLocationWCS[3],
                                                              const double aVelocityWCS[3],
                                                              WsfMover*    aMoverPtr,
                                                              WsfDraw*     aDrawPtr) const
{
   return new TSPI_Model(aSimTime, SimStartTime(), TSPI_StartTime(), aModelTypeId, this, aDrawPtr);
}

//-------------------------------------------------------------------------------------------------
//! Update the mover, based on the current data from the TSPI input file
//! @param aSimTime The current simulation time.
void WsfTSPI_Mover::UpdateMover(double aSimTime)
{
   double timeUntilCurrentUpdate = mCurrentUpdateTime - aSimTime;
   if (timeUntilCurrentUpdate < 0.0)
   {
      timeUntilCurrentUpdate = -timeUntilCurrentUpdate;
   }
   bool onTime = (timeUntilCurrentUpdate < 0.0001);

   // Velocity vector comes from dR/dt, but use the speed value since it is available at the current time.
   double R1[3];
   double R2[3];
   UtEntity::ConvertLLAToWCS(mCurrentTSPI.mLat(), mCurrentTSPI.mLon(), mCurrentTSPI.mAlt(), R2);
   UtEntity::ConvertLLAToWCS(mLastTSPI.mLat(), mLastTSPI.mLon(), mLastTSPI.mAlt(), R1);

   // Compute delta-r (dr) == (R2 - R2)
   double dr[3];
   UtVec3d::Subtract(dr, R2, R1);

   // Normalize, then multiply by the speed to get the velocity.
   UtVec3d::Normalize(dr);
   double velWCS[3];

   if (!onTime)
   {
      double timeSinceLastUpdate = aSimTime - mLastUpdateTime;
      if (mCurrentTSPI.mSpeed() != mLastTSPI.mSpeed())
      {
         double weight = timeSinceLastUpdate / (mCurrentUpdateTime - mLastUpdateTime);
         UtVec3d::Multiply(velWCS, dr, (weight * mCurrentTSPI.mSpeed() + (1.0 - weight) * mLastTSPI.mSpeed()));
      }
      else
      {
         UtVec3d::Multiply(velWCS, dr, mCurrentTSPI.mSpeed());
      }
      // Delta position comes from v*dt
      double deltaLocationWCS[3];
      UtVec3d::Multiply(deltaLocationWCS, velWCS, timeSinceLastUpdate);

      double locationWCS[3];
      UtVec3d::Add(locationWCS, R1, deltaLocationWCS);
      GetPlatform()->SetLocationWCS(locationWCS);
   }
   else
   {
      UtVec3d::Multiply(velWCS, dr, mCurrentTSPI.mSpeed());
      GetPlatform()->SetLocationLLA(mCurrentTSPI.mLat(), mCurrentTSPI.mLon(), mCurrentTSPI.mAlt());
   }

   GetPlatform()->SetVelocityWCS(velWCS);
   GetPlatform()->SetOrientationNED(mCurrentTSPI.mHeading(), mCurrentTSPI.mPitch(), mCurrentTSPI.mRoll());
}

//-------------------------------------------------------------------------------------------------
// virtual
WsfMover* WsfTSPI_Mover::Clone() const
{
   return new WsfTSPI_Mover(*this);
}

//-------------------------------------------------------------------------------------------------
//! Close the TSPI input file.
void WsfTSPI_Mover::CloseFile()
{
   if (mIsOpen)
   {
      mIsOpen = false;
      mInputStreamPtr->close();
      if (mDeleteOnDestruct)
      {
         remove(mFileName.c_str());
      }
   }
}

//-------------------------------------------------------------------------------------------------
//! Open the TSPI input file.
//! This will throw an OpenError exception if the file cannot be opened or if a file is already opened.
void WsfTSPI_Mover::OpenFile(const std::string& aFileName)
{
   mInputStreamPtr->open(aFileName);
   if (mInputStreamPtr->good())
   {
      mIsOpen = true;
      GetScenario().GetSystemLog().WriteLogEntry("file " + mFileName);
   }
   else
   {
      mInputStreamPtr->close();
      auto out = ut::log::error() << "Cannot open TSPI file.";
      out.AddNote() << "File: " << aFileName;
      throw OpenError(); // Could not open the file
   }
}

//-------------------------------------------------------------------------------------------------
// virtual
void WsfTSPI_Mover::TriggerExtrapolation()
{
   mIsExtrapolating = true; // surrogate flag for "burnout"
}

// ===============================================================================================
WsfTSPI_Mover::Point::Point(const WsfTSPI& aTSPI)
   : mOrigTSPI(aTSPI)
   , mOrigLocWCS_SL()
   , mOrigFRDinXYZ()
   , mMovedTSPI(aTSPI) // Copy all TSPI attributes.  Will override some of them.
   , mMovedLocWCS_SL()
   , mMovedFRDinXYZ()
{
   // Convert Orig LL to Orig WCS (ignore altitude):
   UtEntity tempEntity;
   // (Here, the Get() is in Degrees.)
   tempEntity.SetLocationLLA(mOrigTSPI.mLat.Get(), mOrigTSPI.mLon.Get(), 0.0);
   tempEntity.GetLocationWCS(mOrigLocWCS_SL.GetData());
   UtDCM WCStoNED = tempEntity.GetDcmWCStoNED();
   // Now save off the inertial orientation:
   // (Here, a Get() is in RADIANS, not degrees.)
   UtDCM NEDtoECS(mOrigTSPI.mHeading.Get(), mOrigTSPI.mPitch.Get(), mOrigTSPI.mRoll.Get());
   mOrigFRDinXYZ = NEDtoECS.GetRow(0) * WCStoNED.Transpose();
}


// =================================================================================================
// Nested class 'TSPI_Model'
// =================================================================================================
WsfTSPI_Mover::TSPI_Model::TSPI_Model(double               aSimTime,
                                      double               aStartTime,
                                      double               aTSPI_StartTime,
                                      WsfStringId          aModelTypeId,
                                      const WsfTSPI_Mover* aTSPI_MoverPtr,
                                      WsfDraw*             aDrawPtr)
   : WsfMover::KinematicModel(aStartTime, aModelTypeId)
   , mTSPI_StartTime(aTSPI_StartTime)
   , mPoints()
{
   // Open the TSPi file and tabulate all points:
   std::string   filePath = aTSPI_MoverPtr->FileName();
   std::ifstream inputStream{filePath};

   if (inputStream.good())
   {
      double apogeeTime          = 0.0;
      double apogeeRadiusSquared = 0.0;
      double thisTime            = 9999999;
      double thisLocWCS[3];
      double thisLocSpher[3];

      WsfTSPI tspiPoint;

      while (!inputStream.eof())
      {
         inputStream >> tspiPoint;
         thisTime            = tspiPoint.mTime();
         double thisEllipAlt = tspiPoint.mAlt();
         UtEntity::ConvertLLAToWCS(tspiPoint.mLat.Get(), tspiPoint.mLon.Get(), thisEllipAlt, thisLocWCS);
         UtEllipsoidalEarth::ConvertEllipsoidalToSpherical(thisLocWCS, thisLocSpher);
         double thisSpherAlt = UtVec3d::Magnitude(thisLocSpher) - UtSphericalEarth::cEARTH_RADIUS;
         double rSquared     = UtVec3d::MagnitudeSquared(thisLocSpher);
         if (rSquared > apogeeRadiusSquared)
         {
            apogeeRadiusSquared = rSquared;
            apogeeTime          = tspiPoint.mTime();
         }
         Add(Point(tspiPoint.mTime(), thisLocSpher));
         double prevSpherAlt = thisSpherAlt;
         double prevTime     = thisTime;

         if ((thisSpherAlt < prevSpherAlt) && // descending
             (prevSpherAlt > 0.0) &&          // was above sea level
             (thisSpherAlt <= 0.0))           // dropped below sea level
         {
            // Crossed over earth radius value.
            // Interpolate for exact impact time.
            double frac     = (0.0 - prevSpherAlt) / (thisSpherAlt - prevSpherAlt);
            mTimeAtLocation = prevTime + frac * (thisTime - prevTime);
         }
      } // end while

      mApogeeTime = apogeeTime;

      if (mTimeAtLocation <= 0.0)
      {
         mTimeAtLocation = thisTime;
      }

      // convert mTimeAtLocation from the fixed TSPI time to simulation time
      mTimeAtLocation = mTimeAtLocation - mTSPI_StartTime + mStartTime;

   } // end if good()
}

// =================================================================================================
bool WsfTSPI_Mover::TSPI_Model::GetLocationXYZ(double aTime, double aLocXYZ[3]) const
{
   // The TSPI trajectory points are stored with their own time stamp that
   // is independent of simulation time.  So first translate to a common
   // time basis, and then interpolate aSimTime for the current XYZ location.
   auto   it1      = mPoints.begin();
   auto   it2      = mPoints.begin();
   double tspiTime = aTime - mStartTime + mTSPI_StartTime;
   while (it2 != mPoints.end())
   {
      it1 = it2;
      ++it2;
      if (it2 != mPoints.end() && it2->mTimeStamp >= tspiTime)
      {
         UtVec3dX pt1(it1->mLocXYZ);
         UtVec3dX pt2(it2->mLocXYZ);
         double   t1   = it1->mTimeStamp;
         double   t2   = it2->mTimeStamp;
         double   frac = (tspiTime - t1) / (t2 - t1);
         UtVec3dX loc  = pt1 + frac * (pt2 - pt1);
         UtVec3d::Set(aLocXYZ, loc.GetData());
         return true;
      }
   }

   return false;
}

// =================================================================================================
bool WsfTSPI_Mover::TSPI_Model::GetVelocityXYZ(double aTime, double aVelXYZ[3]) const
{
   // The TSPI trajectory points are stored with their own time stamp that
   // is independent of simulation time.  So first translate to a common
   // time basis, and then interpolate aSimTime for the current XYZ velocity.
   auto   it1      = mPoints.begin();
   auto   it2      = mPoints.begin();
   double tspiTime = aTime - mStartTime + mTSPI_StartTime;
   while (it2 != mPoints.end())
   {
      it1 = it2;
      ++it2;
      if (it2 != mPoints.end() && it2->mTimeStamp >= tspiTime)
      {
         UtVec3dX pt1(it1->mVelXYZ);
         UtVec3dX pt2(it2->mVelXYZ);
         double   t1   = it1->mTimeStamp;
         double   t2   = it2->mTimeStamp;
         double   frac = (tspiTime - t1) / (t2 - t1);
         UtVec3dX vel  = pt1 + frac * (pt2 - pt1);
         UtVec3d::Set(aVelXYZ, vel.GetData());
         return true;
      }
   }

   return false;
}

// =================================================================================================
WsfTSPI_Mover::TSPI_Model::Point::Point(double aTimeStamp, const double aLocXYZ[3])
   : mTimeStamp(aTimeStamp)
{
   static const double cZERO[] = {0.0, 0.0, 0.0};
   UtVec3d::Set(mLocXYZ, aLocXYZ);
   UtVec3d::Set(mVelXYZ, cZERO);
}

// =================================================================================================
void WsfTSPI_Mover::TSPI_Model::Add(const Point& aPoint)
{
   // Estimate a velocity for the previous point:
   if (!mPoints.empty())
   {
      const Point& prev = mPoints.back();
      double       dt   = aPoint.mTimeStamp - prev.mTimeStamp;
      UtVec3dX     prevLoc(prev.mLocXYZ);
      UtVec3dX     thisLoc(aPoint.mLocXYZ);
      UtVec3dX     diff  = thisLoc - prevLoc;
      double       ds    = diff.Magnitude();
      double       speed = 0.0;
      if (dt > 0.0)
      {
         speed = ds / dt;
      }
      UtVec3dX vel = diff;
      vel.Normalize();
      vel *= speed;
      UtVec3d::Set(mPoints.back().mVelXYZ, vel.GetData());
   }
   mPoints.push_back(aPoint);
}
