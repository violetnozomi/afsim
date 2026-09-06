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

#include "WsfEM_Attenuation.hpp"

#include "UtInput.hpp"
#include "UtMath.hpp"
#include "UtMeasurementUtil.hpp"
#include "WsfEM_Interaction.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfEM_XmtrRcvr.hpp"
#include "WsfEnvironment.hpp"
#include "WsfPlatform.hpp"

// =================================================================================================
WsfEM_Attenuation::WsfEM_Attenuation()
   : WsfObject()
   , mDebugEnabled(false)
   , mSortEndPoints(true)
{
}

// =================================================================================================
WsfEM_Attenuation::WsfEM_Attenuation(const WsfEM_Attenuation& aSrc)
   : WsfObject(aSrc)
   , mDebugEnabled(aSrc.mDebugEnabled)
   , mSortEndPoints(aSrc.mSortEndPoints)
{
}

// =================================================================================================
// virtual
WsfEM_Attenuation::~WsfEM_Attenuation() {}

// =================================================================================================
//! Initialize the attenuation object.
//! @param aXmtrRcvrPtr The transmitter/receiver to which the attenuation object is attached.
// virtual
bool WsfEM_Attenuation::Initialize(WsfEM_XmtrRcvr* aXmtrRcvrPtr)
{
   return true;
}

// =================================================================================================
//! Initialize the attenuation object.
//! @param aXmtrPtr The transmitter to which the attenuation object is attached.
//! @note This method is deprecated and is provided simply so existing models will continue to
//! function. All new models should use the form that accepts a WsfEM_XmtrRcvr.
// virtual
bool WsfEM_Attenuation::Initialize(WsfEM_Xmtr* aXmtrPtr)
{
   return Initialize(static_cast<WsfEM_XmtrRcvr*>(aXmtrPtr));
}

// =================================================================================================
// virtual
bool WsfEM_Attenuation::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "debug")
   {
      mDebugEnabled = true;
   }
   else
   {
      myCommand = false;
   }
   return myCommand;
}

// =================================================================================================
//! Compute the attenuation loss factor along a specified path segment.
//!
//! @param aInteraction The WsfEM_Interaction object containing all geometry needed to perform the calculation.
//! @param aEnvironment The environment object (for future use to determine inhomogeneous conditions).
//! @param aGeometry    Indicates which segment of the path is to be computed (Xmtr->Tgt, Tgt->Rcvr or Xmtr->Rcvr).
//! @returns The fraction of the signal that remains at the end of the segment, in the range [0, 1].
//!
//! @note This is the public interface to the attenuation calculations that is called by WsfEM_Interaction.
//! A derived class may implement this method for the most general capabilities, or may implement the
//! protected 4-argument form which is called by the base class implementation of this form.
// virtual
double WsfEM_Attenuation::ComputeAttenuationFactor(WsfEM_Interaction&          aInteraction,
                                                   WsfEnvironment&             aEnvironment,
                                                   WsfEM_Interaction::Geometry aGeometry)
{
   double range, elevation, altitude;
   GetRangeElevationAltitude(aInteraction, aGeometry, range, elevation, altitude);

   double atten = 1.0;
   if (range > 1.0)
   {
      double frequency = 0.0;
      if ((aInteraction.GetReceiver() != nullptr) &&
          (aInteraction.GetReceiver()->GetFunction() != WsfEM_Rcvr::cRF_PASSIVE_SENSOR))
      {
         frequency = aInteraction.GetReceiver()->GetFrequency();
      }
      else if (aInteraction.GetTransmitter() != nullptr)
      {
         frequency = aInteraction.GetTransmitter()->GetFrequency();
      }
      atten = ComputeAttenuationFactorP(range, elevation, altitude, frequency);
   }
   return atten;
}

// =================================================================================================
//! Compute the attenuation loss factor along a specified path.
//!
//! @param aRange     The range over which the signal will travel (meters)
//! @param aElevation The elevation angle above the horizontal plane that is tangent to the Earth's surface (radians)
//! @param aAltitude  The altitude of the lowest point between the two points under consideration.  This altitude must
//! be lowest as some models assume it.
//! @param aFrequency The frequency of the signal (hertz).
//! @returns The fraction of the signal that remains at the end of the path, in the range [0, 1].
//!
//! @note This form is NOT called directly by the routines in WsfEM_Interaction (they call the 3-argument form).
//! The base class 3-argument form calls this form, which as a signature that is typical for many RF models.
//!
//! @note This form is protected and can ONLY be called by the base class 3-argument form.
// protected virtual
double WsfEM_Attenuation::ComputeAttenuationFactorP(double aRange, double aElevation, double aAltitude, double aFrequency)
{
   return 1.0;
}

//! Get the altitudes and ground range of the objects in an interaction.
//!
//! @param aInteraction The interaction object.
//! @param aGeometry    An enum representing the participants.
//! @param aAltitude1   [output] The altitude of the observer object (see note below)
//! @param aAltitude2   [output] The altitude of the observee object (see note below)
//! @param aGroundRange [output] The ground range between the participants.
//!
//! @note The geometry enumeration specifies the signal path with the first point being the emitting
//! source (i.e.: the transmitter or the radiating or reflecting target). In this context, we have a
//! slightly different view. If the interaction involves a reciever then the receiver is considered
//! the observer and the other object is considered the observee. If the interation is transmitter-to-
//! target then the transmitter is the observer.
//!
//! This is not important if 'sort_end_points' is 'true' as the first altitude will always be the
//! lower of the two altitudes. 'sort_end_points' is typically false only by explicit user-input in
//! WsfTabularAttenuation when the does not assume that paths are symmetic.
void WsfEM_Attenuation::GetAltitudesAndGroundRange(WsfEM_Interaction&          aInteraction,
                                                   WsfEM_Interaction::Geometry aGeometry,
                                                   double&                     aAltitude1,
                                                   double&                     aAltitude2,
                                                   double&                     aGroundRange)
{
   if (aGeometry == WsfEM_Interaction::cXMTR_TO_TARGET)
   {
      aAltitude1   = aInteraction.mXmtrLoc.mAlt;
      aAltitude2   = aInteraction.mTgtLoc.mAlt;
      aGroundRange = UtMeasurementUtil::GroundRange(aInteraction.mXmtrLoc.mLocWCS, aInteraction.mTgtLoc.mLocWCS);
   }
   else if (aGeometry == WsfEM_Interaction::cTARGET_TO_RCVR)
   {
      aAltitude1   = aInteraction.mRcvrLoc.mAlt;
      aAltitude2   = aInteraction.mTgtLoc.mAlt;
      aGroundRange = UtMeasurementUtil::GroundRange(aInteraction.mRcvrLoc.mLocWCS, aInteraction.mTgtLoc.mLocWCS);
   }
   else if (aGeometry == WsfEM_Interaction::cXMTR_TO_RCVR)
   {
      aAltitude1   = aInteraction.mRcvrLoc.mAlt;
      aAltitude2   = aInteraction.mXmtrLoc.mAlt;
      aGroundRange = UtMeasurementUtil::GroundRange(aInteraction.mRcvrLoc.mLocWCS, aInteraction.mXmtrLoc.mLocWCS);
   }
   else
   {
      throw UtException("WsfEM_Attenuation::ComputeAltitudesAndGroundRange: invalid geometrical relation.");
   }

   // Exchange the altitudes if end point sorting was requested and the first point is higher.
   if (mSortEndPoints && (aAltitude1 > aAltitude2))
   {
      std::swap(aAltitude1, aAltitude2);
   }
}

// =================================================================================================
//! Get the slant range, elevation and altitude between an observer and another object.
//!
//! @param aInteraction The interaction object.
//! @param aGeometry    An enum representing the participants.
//! @param aRange       [output] The slant range between the objects.
//! @param aElevation   [output] The elevation angle from the observer to the object being observed.
//! @param aAltitude    [output] The altitude of the observer.
//!
//! @note The geometry enumeration specifies the signal path with the first point being the emitting
//! source (i.e.: the transmitter or the radiating or reflecting target). In this context, we have a
//! slightly different view. If the interaction involves a reciever then the receiver is considered
//! the observer and the other object is considered the observee. If the interation is transmitter-to-
//! target then the transmitter is the observer.
//!
//! This is not important if 'sort_end_points' is 'true' as the altitude will always be the lower of
//! the two altitudes and the elevation angle will be computed relative to object at the lower altitude.
//! 'sort_end_points' is typically false only by explicit user-input in WsfTabularAttenuation when the
//! table does not assume paths are symmetric.
// protected
#include <iostream>

void WsfEM_Attenuation::GetRangeElevationAltitude(WsfEM_Interaction&          aInteraction,
                                                  WsfEM_Interaction::Geometry aGeometry,
                                                  double&                     aRange,
                                                  double&                     aElevation,
                                                  double&                     aAltitude)
{
   if (aGeometry == WsfEM_Interaction::cXMTR_TO_TARGET)
   {
      GetRangeElevationAltitude(aInteraction.GetTransmitter()->GetPlatform(),
                                aInteraction.GetTarget(),
                                aInteraction.mXmtrLoc,
                                aInteraction.mTgtLoc,
                                aInteraction.mXmtrToTgt,
                                aInteraction.mTgtToXmtr,
                                aRange,
                                aElevation,
                                aAltitude);
   }
   else if (aGeometry == WsfEM_Interaction::cTARGET_TO_RCVR)
   {
      GetRangeElevationAltitude(aInteraction.GetReceiver()->GetPlatform(),
                                aInteraction.GetTarget(),
                                aInteraction.mRcvrLoc,
                                aInteraction.mTgtLoc,
                                aInteraction.mRcvrToTgt,
                                aInteraction.mTgtToRcvr,
                                aRange,
                                aElevation,
                                aAltitude);
   }
   else if (aGeometry == WsfEM_Interaction::cXMTR_TO_RCVR)
   {
      GetRangeElevationAltitude(aInteraction.GetTransmitter()->GetPlatform(),
                                aInteraction.GetReceiver()->GetPlatform(),
                                aInteraction.mXmtrLoc,
                                aInteraction.mRcvrLoc,
                                aInteraction.mXmtrToRcvr,
                                aInteraction.mRcvrToXmtr,
                                aRange,
                                aElevation,
                                aAltitude);
   }
   else
   {
      throw UtException("WsfEM_Attenuation::ComputeRangeElevationAltitude: invalid geometrical relation.");
   }
}

// =================================================================================================
// private
void WsfEM_Attenuation::GetRangeElevationAltitude(WsfPlatform*                           aSrcPlatformPtr,
                                                  WsfPlatform*                           aTgtPlatformPtr,
                                                  const WsfEM_Interaction::LocationData& aSrcLoc,
                                                  const WsfEM_Interaction::LocationData& aTgtLoc,
                                                  const WsfEM_Interaction::RelativeData& aSrcToTgt,
                                                  const WsfEM_Interaction::RelativeData& aTgtToSrc,
                                                  double&                                aRange,
                                                  double&                                aElevation,
                                                  double&                                aAltitude)
{
   // Determine the elevation angle above/below the horizontal plane that is tangent to the Earths' surface.
   //
   // NOTE: There is no need to worry about antenna heights here because we're transforming the pointing
   //       vector and not the location.

   double otherLocNED[3];
   double referenceAlt = aSrcLoc.mAlt;
   if ((aSrcLoc.mAlt <= aTgtLoc.mAlt) || (!mSortEndPoints))
   {
      aSrcPlatformPtr->ConvertWCSVectorToNED(otherLocNED, aSrcToTgt.mUnitVecWCS);
   }
   else
   {
      referenceAlt = aTgtLoc.mAlt;
      aTgtPlatformPtr->ConvertWCSVectorToNED(otherLocNED, aTgtToSrc.mUnitVecWCS);
   }

   double elevation = UtMath::cPI_OVER_2; // Assume other point is directly overhead.
   double neDist    = sqrt((otherLocNED[0] * otherLocNED[0]) + (otherLocNED[1] * otherLocNED[1]));
   if (neDist > 0.0)
   {
      elevation = atan2(-otherLocNED[2], neDist);
   }
   aElevation = elevation;
   aRange     = aSrcToTgt.mRange;
   aAltitude  = referenceAlt;
}

// =================================================================================================
