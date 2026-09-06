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

#include "MTT_ActiveTrack.hpp"

#include "MTT.hpp"
#include "UtCovariance.hpp"
#include "UtEllipsoidalEarth.hpp"
#include "WsfAttributeContainer.hpp"
#include "WsfLocalTrack.hpp"
#include "WsfMTT_Interface.hpp"

MTT_ActiveTrack::MTT_ActiveTrack()
{
   mDataPtr = new MTT_ActiveTrack::Data();
}

MTT_ActiveTrack::MTT_ActiveTrack(const MTT_ActiveTrack& aSrc)
   : MTT_Track(aSrc)
{
}

void MTT_ActiveTrack::SetData(const Data& aData)
{
   if (mDataPtr != nullptr)
   {
      delete mDataPtr;
   }
   mDataPtr = new Data(aData);
}

MTT_ActiveTrack::Data& MTT_ActiveTrack::GetData() const
{
   return *(static_cast<MTT_ActiveTrack::Data*>(mDataPtr));
}

void MTT_ActiveTrack::CopyFrom(double aSimTime, const WsfTrack& aTrack) {}

void MTT_ActiveTrack::CopyAuxDataTo(WsfLocalTrack& aTrack) const
{
   MTT_ActiveTrack::Data& data = GetData();

   if (mVerticalChannelActive)
   {
      // Save off the original data, in case this track is passed to another node with MTT.
      aTrack.GetAuxData().Assign("MTT_VerticalCovariance", data.mVerticalInfoMatrix);
   }

   // Convert the filter states.
   // Currently saving the x-y accelerations (5-6), for use in other MTT trackers.
   double eastAcceleration  = data.mFilterStates(4);
   double northAcceleration = data.mFilterStates(5);
   aTrack.GetAuxData().Assign("eastAcceleration", eastAcceleration);
   aTrack.GetAuxData().Assign("northAcceleration", northAcceleration);
   // Allow the covar to be retrieved "as-is" if it is later input to another MTT as a processed track.
   aTrack.GetAuxData().Assign("MTT_Covariance", data.mHorizontalInfoMatrix);
   // cout << "Active track: Assigning mtt_covariance to " << aTrack.GetTrackId() << endl;
}

void MTT_ActiveTrack::CopyTo(double aSimTime, const WsfMTT_ReferencePoint& aReferencePoint, WsfLocalTrack& aTrack) const
{
   // Copy any active track data that will be used later as aux data in the local track:
   CopyAuxDataTo(aTrack);

   MTT_ActiveTrack::Data& data = GetData();

   // TODO NOTE JAJ - mttexe.f:MTTALT used 0.0 as the default altitude.
   double    down    = -aTrack.GetDefaultAltitude();
   double    downVel = 0.0;
   UtMatrixd vCovar(2, 2); // may not use if vert. channel not active

   bool verticalChannelActive = mVerticalChannelActive;

   if (verticalChannelActive)
   {
      // TODO HACK JAJ - The vertical channel may be marked as 'active' when in fact it isn't
      // TODO HACK JAJ - (this is due to a bug that starts with fusing 2D and 3D embryonic tracks).
      // TODO HACK JAJ - This gets around the problem for now.
      try
      {
         vCovar.Invert2x2(data.mVerticalInfoMatrix);
         UtMatrixd vertData(2, 1);
         vertData.Multiply(vCovar, data.mVerticalState);
         down    = -vertData(0);
         downVel = -vertData(1);
      }
      catch (std::exception&)
      {
         verticalChannelActive = false;
         down                  = -aTrack.GetDefaultAltitude();
         downVel               = 0.0;
      }
   }

   double locWCS[3];
   double locNED[3] = {data.mFilterStates(1), data.mFilterStates(0), down};
   aReferencePoint.ConvertNEDToWCS(locNED, locWCS);
   double originatorLocWCS[3];
   aTrack.GetOriginatorLocationWCS(originatorLocWCS);
   double velNED[3] = {data.mFilterStates(3), data.mFilterStates(2), downVel};
   double velWCS[3];
   aReferencePoint.ConvertNEDVectorToWCS(velWCS, velNED);
   aTrack.SetLocationWCS(locWCS);
   aTrack.SetWCS_LocationValid(true);
   aTrack.SetVelocityWCS(velWCS);
   aTrack.SetVelocityValid(true);
   aTrack.Set3D(verticalChannelActive);

   // Invalidate local rbe
   aTrack.SetRangeValid(false);
   aTrack.SetBearingValid(false);
   aTrack.SetElevationValid(false);
   aTrack.SetRangeRateValid(false);

   // Now for the covariance; the diagonal elements should be:
   // e - 0
   // n - 1
   // evel - 2
   // nvel - 3
   // eacc - 4
   // nacc - 5
   // Copy to a UtMatrix
   UtMatrixd covar(6, 6);
   covar.CopyFrom(data.mHorizontalInfoMatrix, 0, 0, 4, 4);

   // Now move indices around so they are correct for WSF:
   covar.SwapIndices(0, 1);
   covar.SwapIndices(2, 3);

   covar.SwapIndices(3, 4); // swap Vy with zero
   covar.SwapIndices(2, 3); // swap Vx with zero

   UtMatrixd ENU_Acceleration(3);

   if (verticalChannelActive)
   {
      // Now copy the vertical info.
      covar(2, 2) = vCovar(0, 0);
      covar(2, 5) = vCovar(0, 1);
      covar(5, 2) = vCovar(1, 0); // should be same as 0,1
      covar(5, 5) = vCovar(1, 1);
   }
   else
   {
      // Don't know; set the variances to high values:
      covar(2, 2) = 1.0e+12;
      covar(5, 5) = 1.0e+6;
   }

   // Convert to ENU:
   double lat, lon, alt;
   UtEntity::ConvertWCSToLLA(originatorLocWCS, lat, lon, alt);
   double WCS_ToNED_Transform[3][3];
   UtEllipsoidalEarth::ComputeNEDTransform(lat, lon, alt, WCS_ToNED_Transform, originatorLocWCS);

   // Finally, convert to WCS
   UtCovariance::ConvertFromFrameA_ToFrameB(covar, WCS_ToNED_Transform);

   covar.Symmetrize();

   // And save in the track:
   aTrack.SetStateCovariance(aSimTime, covar);
   aTrack.SetStateCovarianceValid(true);

   /*
      //test
            WsfPlatform* targetPtr = WsfSimulation::GetPlatformByIndex(aTrack.GetTargetIndex());
            if (targetPtr != 0)
            {
               double truthLoc[3];
               targetPtr->GetLocationWCS(truthLoc);
               double diffLoc[3];
               UtVec3d::Subtract(diffLoc, truthLoc, locWCS);
               double distDiff = UtVec3d::Magnitude(diffLoc);
               double truthVel[3];
               targetPtr->GetVelocityWCS(truthVel);
               double diffVel[3];
               UtVec3d::Subtract(diffVel, truthVel, velWCS);
               double speedDiff = UtVec3d::Magnitude(diffVel);
               cout << "   : Post-fusion gTrack-Truth difference, dist: " << distDiff << "; speed: " << speedDiff << endl;
            }
      //end test
   */
   // test2 - Convert to a perceived cluster and then to an active track
}
