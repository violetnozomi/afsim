// ****************************************************************************
// CUI
//
// The Advanced Framework for Simulation, Integration, and Modeling (AFSIM)
//
// Copyright 2018 Infoscitex, a DCS Company. All rights reserved.
//
// The use, dissemination or disclosure of data in this file is subject to
// limitation or restriction. See accompanying README and LICENSE for details.
// ****************************************************************************

// References:
//
// 1) "Direct-Detection LADAR Systems"; SPIE Press Tutorial Text in Optical Engineering,
//    Volume TT85; Richard D. Richmond and Stephen C. Cain.
// 2) "Laser Communications in Space", Stephen G. Lambert & William L. Casey, Artech House, 1995.

#include "WsfLASER_XmtrComponent.hpp"

#include <math.h>

#include "UtInput.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtStringIdLiteral.hpp"

// =================================================================================================
// Definition for the nested class WsfLASER_XmtrComponent
// =================================================================================================

// =================================================================================================
WsfLASER_XmtrComponent::WsfLASER_XmtrComponent()
   : WsfEM_XmtrComponent()
   , mApertureDiameter(0.0)
   , mOpticsTransmissionFactor(1.0)
   , mBeamwidth(0.0)
   , mWavefrontError(0.0)
   , mWavefrontTransmissionFactor(1.0)
   , mTransmitAntennaGain(0.0)
   , mAntennaGain(0.0)
   , mObscurationTruncationFactor(1.0)
{
}

// =================================================================================================
WsfComponent* WsfLASER_XmtrComponent::CloneComponent() const
{
   return new WsfLASER_XmtrComponent(*this);
}

// =================================================================================================
WsfStringId WsfLASER_XmtrComponent::GetComponentName() const
{
   return UtStringIdLiteral("em_component_laser_xmtr");
}

// =================================================================================================
const int* WsfLASER_XmtrComponent::GetComponentRoles() const
{
   static const int roles[] = {cWSF_COMPONENT_LASER_XMTR, 0};
   return roles;
}

// =================================================================================================
void* WsfLASER_XmtrComponent::QueryInterface(int aRole)
{
   if (aRole == cWSF_COMPONENT_LASER_XMTR)
   {
      return this;
   }
   return nullptr;
}

//! The default earth radius multiplier should be 1.0 (optical).
// =================================================================================================
void WsfLASER_XmtrComponent::ComponentParentChanged(WsfEM_Xmtr* aXmtr)
{
   GetComponentParent()->SetEarthRadiusMultiplier(1.0);
}

// =================================================================================================
// virtual
bool WsfLASER_XmtrComponent::Initialize(double aSimTime)
{
   bool ok = true;

   // We check our derived members first along with any special dependencies on the base class before
   // we initialize the base class. The ensures error messages are a little more specific.
   WsfEM_Xmtr& xmtr = *GetComponentParent();

   if (xmtr.GetFrequency() == 0.0) // GetWavelength() returns non-zero if frequency is zero, so test GetFrequency() instead...
   {
      ut::log::error() << "Transmitter 'wavelength' must be specified.";
      ok = false;
   }
   if (xmtr.GetPulseWidth() <= 0.0)
   {
      ut::log::error() << "Transmitter 'pulse_width' must be specified.";
      ok = false;
      xmtr.SetPulseRepetitionInterval(0.0); // Prevent extraneous message in WsfEM_Xmtr::Initialize();
   }
   if ((mApertureDiameter == 0.0) && (mBeamwidth == 0.0))
   {
      ut::log::error() << "Either transmitter 'aperture_diameter' or 'beamwidth' must be specified.";
      ok = false;
   }
   else if (mApertureDiameter == 0.0) // and beamwidth was specified:
   {
      // Assume standard circular aperture
      // Full angle as in (2), equation 3.79
      mApertureDiameter = 4.0 * xmtr.GetWavelength() / mBeamwidth / UtMath::cPI;
   }
   else if (mBeamwidth == 0.0) // and aperture_diameter was specified:
   {
      // Assume standard circular aperture
      // Full angle as in (2), equation 3.79
      mBeamwidth = 4.0 * xmtr.GetWavelength() / mApertureDiameter / UtMath::cPI;
   }

   ok &= WsfEM_XmtrComponent::Initialize(aSimTime);

   if (ok)
   {
      // Compute antenna gain.
      // The formulation of antenna gain follows ref. (2), eq. 3.83.
      mTransmitAntennaGain =
         pow(UtMath::cPI * mApertureDiameter / xmtr.GetWavelength(), 2.0); // substituting for area in eq. 3.83
      mObscurationTruncationFactor = mOpticsTransmissionFactor * mWavefrontTransmissionFactor;
      mAntennaGain                 = mTransmitAntennaGain * mObscurationTruncationFactor;

      // Compute wavefront transmission factor from wavefront error.
      if (mWavefrontError > 0.0) // if zero, mWavefrontTransmissionFactor may have been set.
      {
         // following derives from (2), eq. 3.102, specifying wavefront error as a fraction of a wave.
         mWavefrontTransmissionFactor = exp(-pow(UtMath::cTWO_PI * mWavefrontError, 2.0));
      }
   }
   return ok;
}

// =================================================================================================
bool WsfLASER_XmtrComponent::ProcessInput(UtInput& aInput)
{
   bool        myCommand = true;
   std::string command(aInput.GetCommand());

   if (command == "aperture_diameter")
   {
      aInput.ReadValueOfType(mApertureDiameter, UtInput::cLENGTH);
      aInput.ValueGreater(mApertureDiameter, 0.0);
   }
   else if (command == "optics_transmission_factor")
   {
      aInput.ReadValue(mOpticsTransmissionFactor);
      aInput.ValueGreater(mOpticsTransmissionFactor, 0.0);
      aInput.ValueLessOrEqual(mOpticsTransmissionFactor, 1.0);
   }
   else if (command == "optics_loss") // Lasercom convention
   {
      aInput.ReadValueOfType(mOpticsTransmissionFactor, UtInput::cRATIO);
      aInput.ValueGreater(mOpticsTransmissionFactor, 0.0);
      aInput.ValueLessOrEqual(mOpticsTransmissionFactor, 1.0);
   }
   else if ((command == "beamwidth") || (command == "beam_divergence_angle")) // Old form
   {
      aInput.ReadValueOfType(mBeamwidth, UtInput::cANGLE);
      aInput.ValueGreater(mBeamwidth, 0.0);
      aInput.ValueLess(mBeamwidth, UtMath::cPI);
   }
   else if (command == "wavefront_transmission_factor")
   {
      aInput.ReadValue(mWavefrontTransmissionFactor);
      aInput.ValueInClosedRange(mWavefrontTransmissionFactor, 0.0, 1.0);

      // reset value of wavefront_error in case both are set.
      mWavefrontError = 0.0;
   }
   else if (command == "wavefront_loss") // Lasercom convention
   {
      aInput.ReadValueOfType(mWavefrontTransmissionFactor, UtInput::cRATIO);
      aInput.ValueInClosedRange(mWavefrontTransmissionFactor, 0.0, 1.0);

      // reset value of wavefront_error in case both are set.
      mWavefrontError = 0.0;
   }
   else if (command == "wavefront_error")
   {
      aInput.ReadValue(mWavefrontError); // in fractions of a wave.
      aInput.ValueGreaterOrEqual(mWavefrontError, 0.0);
   }
   else
   {
      myCommand = WsfEM_XmtrComponent::ProcessInput(aInput);
   }
   return myCommand;
}

//! Get the aperture area, assumed circular.
double WsfLASER_XmtrComponent::GetApertureArea() const
{
   return UtMath::cPI * pow(mApertureDiameter / 2.0, 2.0);
}
