// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2017 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

#include "WsfCommComponentHW_Subsurface.hpp"

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "UtStringIdLiteral.hpp"
#include "WsfComm.hpp"
#include "WsfCommResult.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Rcvr.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfGeoPoint.hpp"
#include "WsfPlatform.hpp"
#include "WsfSimulation.hpp"
#include "WsfTerrain.hpp"

namespace wsf
{
namespace comm
{

// =================================================================================================
//! Find the instance of this component attached to the specified processor.
ComponentHW_Subsurface* ComponentHW_Subsurface::Find(const Comm& aParent)
{
   ComponentHW_Subsurface* componentPtr(nullptr);
   aParent.GetComponents().FindByRole<ComponentHW_Subsurface>(componentPtr);
   return componentPtr;
}

// =================================================================================================
//! Find the instance of this component attached to the specified processor,
//! and create it if it doesn't exist.
ComponentHW_Subsurface* ComponentHW_Subsurface::FindOrCreate(Comm& aParent)
{
   ComponentHW_Subsurface* componentPtr = Find(aParent);
   if (componentPtr == nullptr)
   {
      componentPtr = new ComponentHW_Subsurface;
      aParent.GetComponents().AddComponent(componentPtr);
   }
   return componentPtr;
}

// =================================================================================================
ComponentHW_Subsurface::ComponentHW_Subsurface()
   : ComponentHW()
   , mWaterAttenuation(0.0)
   , mMinimumHorizonAngle(-UtMath::cPI_OVER_2)
   , mIsVLFcomm(false)
   , mAllowedWaterPathRange(1000.0) // Set a 1000 meter buffer for horizon checks
   , mMaxCommunicationDepth(-DBL_MAX)
{
   GetEM_Xmtr(0).SetEarthRadiusMultiplier(1.0); // Default is 1.0 for default optical-LOS behavior

   // Disable checks that would otherwise stop comm when below zero alt
   GetEM_Xmtr(0).DisableMaskingCheck();
   GetEM_Rcvr(0).DisableMaskingCheck();
}

// =================================================================================================
ComponentHW_Subsurface::ComponentHW_Subsurface(const ComponentHW_Subsurface& aSrc)
   : ComponentHW(aSrc)
   , mWaterAttenuation(aSrc.mWaterAttenuation)
   , mMinimumHorizonAngle(aSrc.mMinimumHorizonAngle)
   , mIsVLFcomm(aSrc.mIsVLFcomm)
   , mAllowedWaterPathRange(aSrc.mAllowedWaterPathRange)
   , mMaxCommunicationDepth(aSrc.mMaxCommunicationDepth)
{
   GetEM_Xmtr(0).SetEarthRadiusMultiplier(1.0); // Optical-LOS behavior when not VLF

   // Disable checks that would otherwise stop comm when below zero alt
   GetEM_Xmtr(0).DisableMaskingCheck();
   GetEM_Rcvr(0).DisableMaskingCheck();
}

// =================================================================================================
WsfComponent* ComponentHW_Subsurface::CloneComponent() const
{
   return new ComponentHW_Subsurface(*this);
}

// =================================================================================================
WsfStringId ComponentHW_Subsurface::GetComponentName() const
{
   return UtStringIdLiteral("comm_component_subsurface_hw");
}

// =================================================================================================
const int* ComponentHW_Subsurface::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_COMM_HW_SUBSURFACE,
                               cWSF_COMPONENT_COMM_HW,
                               cWSF_COMPONENT_COMM,
                               cWSF_COMPONENT_NULL};

   return roles;
}

// =================================================================================================
void* ComponentHW_Subsurface::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_COMM_HW_SUBSURFACE)
   {
      return this;
   }
   if (aRole == cWSF_COMPONENT_COMM_HW)
   {
      return dynamic_cast<ComponentHW*>(this);
   }
   if (aRole == cWSF_COMPONENT_COMM)
   {
      return dynamic_cast<Component*>(this);
   }

   return nullptr;
}

// ==========================================================================
bool ComponentHW_Subsurface::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());
   if (command == "water_attenuation_factor")
   {
      double      value;
      std::string units;
      aInput.ReadValue(value);
      aInput.ReadValue(units);

      // Parse the units.
      std::string::size_type slashPos = units.find('/');
      if ((slashPos != std::string::npos) && (slashPos != 0) && ((slashPos + 1) != units.size()))
      {
         std::string ratioUnits(units.substr(0, slashPos));
         std::string lengthUnits(units.substr(slashPos + 1));
         double      ratioValue    = aInput.ConvertValue(value, ratioUnits, UtInput::cRATIO);
         double      ratioValue_dB = UtMath::LinearToDB(ratioValue);
         double      lengthFactor  = aInput.ConvertValue(1.0, lengthUnits, UtInput::cLENGTH);
         mWaterAttenuation         = ratioValue_dB / lengthFactor; // unit is dB/meter
      }
      else
      {
         throw UtInput::BadValue(aInput, "Unknown water attenuation units: " + units);
      }
   }
   else if (command == "minimum_horizon_angle")
   {
      aInput.ReadValueOfType(mMinimumHorizonAngle, UtInput::cANGLE);
      aInput.ValueInClosedRange(mMinimumHorizonAngle, -UtMath::cPI_OVER_2, UtMath::cPI_OVER_2);
   }
   else if (command == "set_VLF_comm") // ignores horizons
   {
      mIsVLFcomm = true;
   }
   else if (command == "unset_VLF_comm") // Default: Enables horizon checks
   {
      mIsVLFcomm = false;
   }
   else if (command == "max_underwater_range_filter")
   {
      aInput.ReadValueOfType(mAllowedWaterPathRange, UtInput::cLENGTH);
      aInput.ValueGreaterOrEqual(mAllowedWaterPathRange, 0.0);
   }
   else if (command == "max_communication_depth")
   {
      aInput.ReadValueOfType(mMaxCommunicationDepth, UtInput::cLENGTH);
      mMaxCommunicationDepth = -fabs(mMaxCommunicationDepth);
   }
   else
   {
      myCommand = ComponentHW::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
// virtual
bool ComponentHW_Subsurface::CanSendTo(double aSimTime, Comm* aRcvrPtr, Result& aResult)
{
   // If this is later modified to be called from someplace else, then inspect closely.
   if (GetComm()->DebugEnabled())
   {
      auto out = ut::log::debug() << "Checking transmitter.";
      out.AddNote() << "Platform: " << GetComm()->GetPlatform()->GetName();
      out.AddNote() << "Comm: " << GetComm()->GetName();
   }

   bool lCanSendTo = false;
   // Get the hardware component from the other comm device
   ComponentHW_Subsurface* rcvrHW_Ptr = ComponentHW_Subsurface::Find(*aRcvrPtr);
   if (rcvrHW_Ptr != nullptr)
   {
      WsfEM_Rcvr& rcvr = rcvrHW_Ptr->GetEM_Rcvr(0);
      rcvr.UpdatePosition(aSimTime); // Ensure the receiver position is current

      if (GetComm()->DebugEnabled())
      {
         auto out = ut::log::debug() << "Checking receiver:";
         out.AddNote() << "Platform: " << rcvr.GetPlatform()->GetName();
      }

      // Set aCheckMaskingFactor (5th argument) to false to disable antenna masking limit check.
      // Note the "horizon" check should already be disabled by the "mCheckMasking" attribute
      // which is disabled via the constructor.  We will do a separate horizon check below here.
      // We will keep the original checks, and capture the states in "lOneWayInteractionValue".
      //
      // Note that the "BeginOneWayInteraction" method checks many constraints and returns
      // with the first failure.  It does not check them all.  We are skipping the horizon check here
      // so we need to do another horizon with the radio's custom rules and reset the aResult as needed.
      //
      // If we get a failure in BeginOneWayInteraction, we will unset any mCheckedStatus for receiver horizon
      // check since we are overriding BeginOneWayInteraction's method for a valid receiver horizon check.
      unsigned int lOneWayInteractionValue = aResult.BeginOneWayInteraction(mXmtrPtr.get(), &rcvr, true, true, false);

      if (lOneWayInteractionValue != 0)
      {
         // Failed a check other than horizon check to get here, but we did not truly do
         // any horizon checks, so remove a possible horizon-checked-status before we move on.
         // Because we have a failure there is nothing to do but fix the log data here.
         aResult.mCheckedStatus &= ~(WsfEM_Interaction::cRCVR_HORIZON_MASKING);
      }
      else // Passed checks but we really did not check our horizon, so check horizon here.
      {
         // Local variables for geometry data for this TR pair
         WsfGeoPoint lLowPoint;
         WsfGeoPoint lHighPoint;
         double      lWaterRange;

         double lLat1;
         double lLon1;
         double lAlt1;
         double lLat2;
         double lLon2;
         double lAlt2;

         // The aResult.mCheckedStatus flag already shows horizon checked by BeginOneWayInteraction
         // but we will really check this class horizon constraints within this block, so just keep it as flagged.
         mAntennaPtr->GetLocationLLA(lLat1, lLon1, lAlt1);
         rcvr.GetAntenna()->GetLocationLLA(lLat2, lLon2, lAlt2);

         bool lLowPointIsXMTR = false;

         // Setting high and low points before SetUnderwaterRadioValues just to simplify math and tests.
         if (lAlt1 < lAlt2) // Alt1 is XMTR; Alt2 is RCVR
         {
            lLowPoint.SetLocationLLA(lLat1, lLon1, lAlt1);
            lHighPoint.SetLocationLLA(lLat2, lLon2, lAlt2);
            lLowPointIsXMTR = true;
         }
         else
         {
            lHighPoint.SetLocationLLA(lLat1, lLon1, lAlt1);
            lLowPoint.SetLocationLLA(lLat2, lLon2, lAlt2);
            lLowPointIsXMTR = false;
         }

         // Apply depth constraint regardless of communication type
         if (lLowPoint.GetAlt() < mMaxCommunicationDepth)
         {
            lCanSendTo = false;

            if (lLowPointIsXMTR)
            {
               aResult.mCheckedStatus |= WsfEM_Interaction::cXMTR_ALTITUDE_LIMITS;
               aResult.mFailedStatus |= WsfEM_Interaction::cXMTR_ALTITUDE_LIMITS;
            }
            else
            {
               aResult.mCheckedStatus |= WsfEM_Interaction::cRCVR_ALTITUDE_LIMITS;
               aResult.mFailedStatus |= WsfEM_Interaction::cRCVR_ALTITUDE_LIMITS;
            }
         }
         else if (mIsVLFcomm) // VLF assumed to be unlimited horizon
         {
            lCanSendTo = true; // Let VLF send without additional horizon or power checks.
         }
         else // is not VLF; So checks other constraints for this radio type
         {
            double lSlantRange;
            double lAirRange;
            double lWaterLatDeg;
            double lWaterLongDeg;
            double lGrazingAngleRadians;

            // This call only checks simple smooth earth horizon constraint.  No terrain.
            lOneWayInteractionValue = SetSubmarineRadioValues(aSimTime,
                                                              lLowPoint,
                                                              lHighPoint,
                                                              lSlantRange,
                                                              lWaterRange,
                                                              lAirRange,
                                                              lWaterLatDeg,
                                                              lWaterLongDeg,
                                                              lGrazingAngleRadians,
                                                              aResult);

            // Treat angle constraint like a horizon limit.
            if ((lGrazingAngleRadians < mMinimumHorizonAngle) && (lWaterRange > .001))
            {
               aResult.mFailedStatus |= WsfEM_Interaction::cRCVR_HORIZON_MASKING;
               lOneWayInteractionValue = aResult.mFailedStatus;
               lCanSendTo              = false;
               if (GetComm()->DebugEnabled())
               {
                  auto out = ut::log::debug() << "Failed MinimumHorizonAngle check:";
                  out.AddNote() << "Platform:" << rcvr.GetPlatform()->GetName();
                  out.AddNote() << "Minimum Horizon Angle: " << mMinimumHorizonAngle * UtMath::cDEG_PER_RAD << " deg";
                  out.AddNote() << "Grazing Angle: " << lGrazingAngleRadians * UtMath::cDEG_PER_RAD << " deg";
               }
            }

            // Treat underwater range constraint like a horizon limit.
            if (lWaterRange > mAllowedWaterPathRange)
            {
               aResult.mFailedStatus |= WsfEM_Interaction::cRCVR_HORIZON_MASKING;
               lOneWayInteractionValue = aResult.mFailedStatus;
               lCanSendTo              = false;
               if (GetComm()->DebugEnabled())
               {
                  auto logDebug = ut::log::debug() << "Failed 'max_underwater_range_filter' check.";
                  logDebug.AddNote() << "Platform: " << rcvr.GetPlatform()->GetName();
                  logDebug.AddNote() << "Maximum Underwater Range: " << mAllowedWaterPathRange << " m";
                  logDebug.AddNote() << "Current Underwater Range: " << lWaterRange << " m";
               }
            }

            // After adjusting the horizon checks, move on as in original routine.
            // VLF comm to behave like a perfect comm to avoid built-in geometry issues.
            // if (lOneWayInteractionValue == 0  && !mIsVLFcomm)
            if (lOneWayInteractionValue == 0)
            {
               if (GetComm()->DebugEnabled())
               {
                  auto out = ut::log::debug() << "Found receiver.";
                  out.AddNote() << "Platform: " << rcvr.GetPlatform()->GetName();
               }

               if (mXmtrPtr->GetPower() > 1.0E-30) // See comment in WsfRadioXmtr ctor
               {
                  aResult.SetTransmitterBeamPosition();
                  aResult.SetReceiverBeamPosition();

                  // Use full range for ComputeRF_OneWayPower without modification.  The extra water range
                  // will be negligible, then apply the large water attenuation factor.
                  aResult.ComputeRF_OneWayPower();
                  // aResult.ComputeRF_OneWayPower() sets values using the slant-range path.
                  // Because the water leg will be very short, and because the air-path attenuation is
                  // very much less than the water-path attenuation, then the attenuation over the
                  // slant range can be expected to be nearly the same as the air leg alone.
                  // The following parameters will need to be adjusted:
                  //   aResult.mPowerDensityAtTarget, aResult.mAbsorptionFactor, and aResult.mRcvdPower.
                  // Fold in the water-path length attenuation to adjust the values.
                  double lWaterPathAttenuationDB = mWaterAttenuation * lWaterRange;
                  double lAttenuationFactor      = pow(10.0, -0.1 * lWaterPathAttenuationDB);
                  aResult.mPowerDensityAtTarget *= lAttenuationFactor;
                  aResult.mAbsorptionFactor *= lAttenuationFactor;
                  aResult.mRcvdPower *= lAttenuationFactor;

                  // Let components update the results.
                  // (Note: this is where jamming would be done... not tested or sure if this
                  //        is truly practical)
                  Component::AttemptToTransmit(*GetComm(), aSimTime, aResult);
                  Component::AttemptToReceive(*aRcvrPtr, aSimTime, aResult);

                  aResult.mSignalToNoise =
                     rcvr.ComputeSignalToNoise(aResult.mRcvdPower, aResult.mClutterPower, aResult.mInterferencePower);

                  aResult.mCheckedStatus = Result::cSIGNAL_LEVEL;
                  if ((aResult.mSignalToNoise >= rcvr.GetDetectionThreshold()) &&
                      (aResult.mInterferenceFactor < 0.5)) // TODO_JAJ How should we really do this?
                  {
                     lCanSendTo = true; // Signal level success
                  }
                  else
                  {
                     aResult.mFailedStatus |= Result::cSIGNAL_LEVEL;
                  }
               }
               else // gets here if user does not set power on transmitter
               {
                  lCanSendTo = true;
                  // Since this is a new radio type, this case is likely to be an
                  // oversight, so alert the user.
                  if (GetComm()->DebugEnabled())
                  {
                     auto out = ut::log::warning() << "No power assigned to transmitter.";
                     out.AddNote() << "Comm Name: " << GetComm()->GetName();
                     out.AddNote() << "Platform: " << GetPlatform()->GetName();
                  }
               }

               // Check for terrain masking if we have made it this far...
               if (lCanSendTo)
               {
                  aResult.mCheckedStatus |= WsfEM_Interaction::cRCVR_TERRAIN_MASKING;
                  // Only check terrain when not VLF and range over underwater path filter
                  if (!mIsVLFcomm && (lSlantRange > mAllowedWaterPathRange))
                  {
                     // Use aMaxRange = 0.0 and aRadiusScale = 1.0 in method below.
                     // If underwater, set alt to zero as close approximation
                     double lCheckAlt1 = std::max(lAlt1, 0.0);
                     double lCheckAlt2 = std::max(lAlt2, 0.0);

                     bool lIsMaskedByTerrain =
                        GetSimulation()
                           ->GetTerrainInterface()
                           ->MaskedByTerrain(lLat1, lLon1, lCheckAlt1, lLat2, lLon2, lCheckAlt2, 0.0, 1.0);

                     if (lIsMaskedByTerrain)
                     {
                        aResult.mFailedStatus |= WsfEM_Interaction::cRCVR_TERRAIN_MASKING;
                        lCanSendTo = false;
                     }
                  }
               }
            } //  End lOneWayInteractionValue block
         }    // end else not  VLF
      }

      if (GetComm()->DebugEnabled())
      {
         auto out = ut::log::debug() << "Comm Debug Info:";
         aResult.Print(out);
      }
   }
   return lCanSendTo;
}

// =================================================================================================
// protected
unsigned int ComponentHW_Subsurface::SetSubmarineRadioValues(double       aSimTime,
                                                             WsfGeoPoint& aLowPoint1,
                                                             WsfGeoPoint& aHighPoint2,
                                                             double&      aSlantrange,
                                                             double&      aWaterRange,
                                                             double&      aAirRange,
                                                             double&      aWaterLatDeg,
                                                             double&      aWaterLongDeg,
                                                             double&      aGrazingAngleRadians,
                                                             Result&      aResult)
{
   unsigned int lFailedStatus = 0;

   double lLowRadioLat;
   double lLowRadioLong;
   double lLowRadioAlt;
   double lHighRadioLat;
   double lHighRadioLong;
   double lHighRadioAlt;
   aLowPoint1.GetLocationLLA(lLowRadioLat, lLowRadioLong, lLowRadioAlt);
   aHighPoint2.GetLocationLLA(lHighRadioLat, lHighRadioLong, lHighRadioAlt);
   aSlantrange = aHighPoint2.GetDistanceFrom(aLowPoint1);

   // Method intended to filter out interactions at ranges that are too large.
   // Water transmission distance is very small. We can put altitude at zero
   // for the calculation and accept number that is very close and isn't off by
   // more than similar accuracy accepted by other earth-horizon assumptions.
   double lAlt1 = std::max(lLowRadioAlt, 0.0);
   double lAlt2 = std::max(lHighRadioAlt, 0.0);
   // Note: To compute ranges we note that the slant range to the horizon for
   //       altitude 'h' and an (effective) earth radius 'R' is:
   //
   //       D = sqrt((R + h)^2 - R^2)
   //         = sqrt(R^2 + 2Rh + h^2 - R^2)
   //         = sqrt(h^2 + 2Rh)

   // Calculate the slant range from each point to their respective horizon.
   double horizon1Range = 0.0;
   double horizon2Range = 0.0;

   if (lAlt1 > 0.)
   {
      horizon1Range = sqrt((lAlt1 * lAlt1) + (2.0 * UtSphericalEarth::cEARTH_RADIUS * lAlt1));
   }
   if (lAlt2 > 0.)
   {
      horizon2Range = sqrt((lAlt2 * lAlt2) + (2.0 * UtSphericalEarth::cEARTH_RADIUS * lAlt2));
   }

   auto out = ut::log::debug();
   if (GetComm()->DebugEnabled())
   {
      out << "ComponentHW_Subsurface's SubmarineRadioValues:";
      out.AddNote() << "Horizon-1 Range: " << horizon1Range;
      out.AddNote() << "Horizon-2 Range: " << horizon2Range;
      out.AddNote() << "Slant Range: " << aSlantrange;
   }

   // Allow sub to be over the horizon by distance mAllowedWaterPathRange so as to avoid
   // filtering out cases with short range underwater communications.
   if (aSlantrange > (horizon1Range + horizon2Range + mAllowedWaterPathRange))
   {
      aResult.mFailedStatus |= WsfEM_Interaction::cRCVR_HORIZON_MASKING;
      lFailedStatus = aResult.mFailedStatus;
   }

   if (aSlantrange == 0.0) // same point
   {
      aWaterRange          = 0.0;
      aAirRange            = 0.0;
      aWaterLatDeg         = aLowPoint1.GetLat();
      aWaterLongDeg        = aLowPoint1.GetLon();
      aGrazingAngleRadians = UtMath::cPI_OVER_2;
   }
   else
   {
      double lLowRadioradius  = UtSphericalEarth::cEARTH_RADIUS + lLowRadioAlt;
      double lHighRadioRadius = UtSphericalEarth::cEARTH_RADIUS + lHighRadioAlt;

      double lHdgLowToHigh;
      double lDistanceOverearth;

      UtSphericalEarth::GreatCircleHeadingAndDistance(lLowRadioLat,
                                                      lLowRadioLong,
                                                      lHighRadioLat,
                                                      lHighRadioLong,
                                                      lHdgLowToHigh,
                                                      lDistanceOverearth);

      // Angle C here is the angle at center of earth between lines up to the two platforms.
      double lCosC = (lLowRadioradius * lLowRadioradius + lHighRadioRadius * lHighRadioRadius - aSlantrange * aSlantrange) /
                     (2.0 * lLowRadioradius * lHighRadioRadius);
      lCosC = std::max(-1.0, std::min(lCosC, 1.0));
      // double lAngleC = acos(lCosC);

      // Angle A here is the angle at low point between lines to earth center and the high point.
      double lCosA = (lLowRadioradius * lLowRadioradius + aSlantrange * aSlantrange - lHighRadioRadius * lHighRadioRadius) /
                     (2.0 * lLowRadioradius * aSlantrange);
      double lAngleA = acos(lCosA);

      // Let x-angle be angle at earth surface between lines to low point and earth center
      // Let ThetaS-angle be the interior earth arc from receiver to the surface of the earth
      double lSinX         = lLowRadioradius * sin(lAngleA) / UtSphericalEarth::cEARTH_RADIUS;
      lSinX                = std::max(-1.0, std::min(lSinX, 1.0));
      double lXangle       = asin(lSinX);
      aGrazingAngleRadians = UtMath::cPI_OVER_2 - lXangle;

      double lThetaS = UtMath::cPI - lXangle - lAngleA;
      if (lThetaS < 0.0)
      {
         lThetaS = 0.0;
      } // roundoff from subtraction can make small negative number.
      double lSurfaceDistX = lThetaS * UtSphericalEarth::cEARTH_RADIUS;

      UtSphericalEarth::ExtrapolateGreatCirclePosition(lLowRadioLat,
                                                       lLowRadioLong,
                                                       lHdgLowToHigh,
                                                       lSurfaceDistX,
                                                       aWaterLatDeg,
                                                       aWaterLongDeg);

      aWaterRange = lLowRadioradius * sin(lThetaS) / lSinX;
      aAirRange   = aSlantrange - aWaterRange;
      if (GetComm()->DebugEnabled())
      {
         out.AddNote() << "Grazing Angle: " << aGrazingAngleRadians * UtMath::cDEG_PER_RAD << " deg";
         out.AddNote() << "WaterLat: " << aWaterLatDeg;
         out.AddNote() << "WaterLong: " << aWaterLongDeg;
         out.AddNote() << "WATER RANGE: " << aWaterRange;
         out.AddNote() << "AIR RANGE: " << aAirRange;
      }
   }

   return lFailedStatus;

} // end SetUnderwaterRadioValues

} // namespace comm
} // namespace wsf
