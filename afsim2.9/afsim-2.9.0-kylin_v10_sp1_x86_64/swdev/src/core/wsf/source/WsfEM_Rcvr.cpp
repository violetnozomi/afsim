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

#include "WsfEM_Rcvr.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <string>

#include "UtInput.hpp"
#include "UtInputBlock.hpp"
#include "UtLog.hpp"
#include "UtMath.hpp"
#include "UtSphericalEarth.hpp"
#include "WsfComponentFactoryList.hpp"
#include "WsfEM_Antenna.hpp"
#include "WsfEM_Manager.hpp"
#include "WsfEM_Util.hpp"
#include "WsfEM_Xmtr.hpp"
#include "WsfPlatform.hpp"
#include "WsfSensorMode.hpp"
#include "WsfSimulation.hpp"

//! Since WsfEM_Rcvr does not derive from WsfComponent, this template declaration allows it to "act" like a
//! component only within this translation unit by giving it a role, which it must have in order to use component factories.
template<>
struct WsfComponentRole<WsfEM_Rcvr> : std::integral_constant<int, cWSF_COMPONENT_EM_RCVR>
{
};

// =================================================================================================
//! Called by the parent method of the same name when an interacting transmitter is added.
//! This is called AFTER entry has been added to the interactor list.
//! It will not be called if the entry is already in the interactor list.
// virtual
void WsfEM_RcvrComponent::AddInteractor(WsfEM_Xmtr* aXmtrPtr) {}

// =================================================================================================
//! Called by the parent method of the same name when an interacting transmitter is removed.
//! This is called AFTER entry has been removed to the interactor list.
//! It will not be called if the entry is not in the interactor list.
// virtual
void WsfEM_RcvrComponent::RemoveInteractor(WsfEM_Xmtr* aXmtrPtr) {}

// =================================================================================================
//! Construct a receiver.
//! @param aFunction The primary function of the transmitter.
//! @param aAntennaPtr Pointer to the antenna associated with the receiver. If one isn't
//! provided or if it is zero then call must be made to SetAntenna() prior to calling Initialize().
//! @note The caller retains ownership of the antenna as it may be shared with other
//! receivers or transmitters.
WsfEM_Rcvr::WsfEM_Rcvr(RcvrFunction aFunction, WsfEM_Antenna* aAntennaPtr /* = 0 */)
   : WsfEM_XmtrRcvr(aAntennaPtr)
   , mCommInteractors()
   , mSensorInteractors()
   , mInterferenceInteractors()
   , mComponents()
   , mEM_ManagerPtr(nullptr)
   , mTotalInteractors(0)
   , mCommBaseIndex(0)
   , mSensorBaseIndex(0)
   , mInterferenceBaseIndex(0)
   , mFunction(aFunction)
   , mInstantaneousBandwidth(0.0)
   , mNoisePower(0.0)
   , mNoiseFigure(1.0)
   , mNoiseMultiplier(1.0)
   , mAntennaOhmicLoss(0.0)
   , mReceiveLineLoss(0.0)
   , mDetectionThreshold(pow(10.0, 3.0 / 10.0))
   , // 3 dB above noise level
   mExplicitInstantaneousBandwidth(false)
   , mExplicitNoisePower(false)
   , mCheckXmtrMasking(true)
{
   // Newly created components will have me as a parent.
   mComponents.SetParentOfComponents(this);

   // attenuation_model and propagation_model are disabled by default for receivers.
   SetAttenuationInputEnabled(false);
   SetPropagationInputEnabled(false);

   for (unsigned int i = 0; i < WsfEM_Types::cPOL_COUNT; ++i)
   {
      mPolarizationEffects[i]         = 1.0;
      mExplicitPolarizationEffects[i] = -1.0;
   }
}

// =================================================================================================
//! Copy construct a receiver.
//! @param aSrc The receiver from which to copy.
//! @param aAntennaPtr Pointer to the antenna associated with the receiver. If one isn't
//! provided or if it is zero then call must be made to SetAntenna() prior to calling Initialize().
//! @note The caller retains ownership of the antenna as it may be shared with other
//! receivers or transmitters.
WsfEM_Rcvr::WsfEM_Rcvr(const WsfEM_Rcvr& aSrc, WsfEM_Antenna* aAntennaPtr /* = 0 */)
   : WsfEM_XmtrRcvr(aSrc, aAntennaPtr)
   , mCommInteractors(aSrc.mCommInteractors)
   , mSensorInteractors(aSrc.mSensorInteractors)
   , mInterferenceInteractors(aSrc.mInterferenceInteractors)
   , mComponents(aSrc.mComponents)
   , mEM_ManagerPtr(nullptr)
   , mTotalInteractors(aSrc.mTotalInteractors)
   , mCommBaseIndex(aSrc.mCommBaseIndex)
   , mSensorBaseIndex(aSrc.mSensorBaseIndex)
   , mInterferenceBaseIndex(aSrc.mInterferenceBaseIndex)
   , mFunction(aSrc.mFunction)
   , mInstantaneousBandwidth(aSrc.mInstantaneousBandwidth)
   , mNoisePower(aSrc.mNoisePower)
   , mNoiseFigure(aSrc.mNoiseFigure)
   , mNoiseMultiplier(aSrc.mNoiseMultiplier)
   , mAntennaOhmicLoss(aSrc.mAntennaOhmicLoss)
   , mReceiveLineLoss(aSrc.mReceiveLineLoss)
   , mDetectionThreshold(aSrc.mDetectionThreshold)
   , mExplicitInstantaneousBandwidth(aSrc.mExplicitInstantaneousBandwidth)
   , mExplicitNoisePower(aSrc.mExplicitNoisePower)
   , mCheckXmtrMasking(aSrc.mCheckXmtrMasking)
{
   // Newly created components will have me as a parent.
   mComponents.SetParentOfComponents(this);

   for (unsigned int i = 0; i < WsfEM_Types::cPOL_COUNT; ++i)
   {
      mPolarizationEffects[i]         = aSrc.mPolarizationEffects[i];
      mExplicitPolarizationEffects[i] = aSrc.mExplicitPolarizationEffects[i];
   }
}

// =================================================================================================
// virtual
WsfEM_Rcvr::~WsfEM_Rcvr()
{
   if (mEM_ManagerPtr != nullptr)
   {
      mEM_ManagerPtr->DeactivateRcvr(this);
   }
}

// =================================================================================================
//! Indicate the receiver is 'turned-on'.
//!
//! This method adds the receiver from the list of active receivers within the simulation.
void WsfEM_Rcvr::Activate()
{
   mEM_ManagerPtr = GetEM_Manager();
   if (mEM_ManagerPtr != nullptr)
   {
      mEM_ManagerPtr->ActivateRcvr(this);
   }
}

// =================================================================================================
//! Indicate that the receiver is 'turned-off'.
//! This method removes the receiver from the list of active receivers within the simulation
//! and clears the interactor list.
void WsfEM_Rcvr::Deactivate()
{
   if (mEM_ManagerPtr != nullptr)
   {
      mEM_ManagerPtr->DeactivateRcvr(this);
      mEM_ManagerPtr = nullptr;
   }
   mCommInteractors.clear();
   mSensorInteractors.clear();
   mInterferenceInteractors.clear();
   UpdateIndices();
}

// =================================================================================================
//! Can this receiver potentially interact with (receive radiation from) the specified
//! transmitter.
//!
//! This method should answer the question, 'given the current frequency and bandwidth,
//! could this receiver process information from the specified transmitter'.  This method
//! must NOT consider geometry.
// virtual
bool WsfEM_Rcvr::CanInteractWith(WsfEM_Xmtr* aXmtrPtr)
{
   bool canInteractWith = true;

   // The default implementation determines if there is any overlap of the frequency
   // spectrum of the receiver with the transmitter.

   double rcvrFreqLo = GetFrequency() - 0.5 * GetBandwidth();
   double rcvrFreqHi = rcvrFreqLo + GetBandwidth();
   double xmtrFreqLo = aXmtrPtr->GetFrequency() - 0.5 * aXmtrPtr->GetBandwidth();
   double xmtrFreqHi = xmtrFreqLo + aXmtrPtr->GetBandwidth();
   if ((xmtrFreqLo > rcvrFreqHi) || (xmtrFreqHi < rcvrFreqLo))
   {
      canInteractWith = false;
   }
   return canInteractWith;
}

// =================================================================================================
//! Compute the received power from an emission direction, taking into account the polarization of
//! of the incoming signal for antenna gain. Does NOT take into account bandwidth mismatch between
//! the receiver and signal.
//! @param aSourceAz             [input] Source azimuth with respect to the beam (radians)
//! @param aSourceEl             [input] Source elevation with respect to the beam (radians)
//! @param aEBS_Az               [input] Beam azimuth with respect to the antenna (radians)
//! @param aEBS_El               [input] Beam elevation with respect to the antenna (radians)
//! @param aPolarization         [input] Polarization of the signal being received.
//! @param aReceivedPowerDensity [input] Power density of the signal at the face of the antenna (w/m^2).
//! @param aFrequency            [input] Frequency of the signal being received (Hertz).
//! @param aAntennaGain          [output] Computed antenna gain (absolute - not dB).
//! @return The received power in watts.
//!
//! @note This method uses the receiver frequency as the center frequency. If one wants to use
//!       the transmitter frequency instead, for a passive sensing or wideband passive system, please override
//!       this method with your own method.
//! @note This method does not apply polarization effects because the polarization of the signal
//!       may have changed due to reflection. The application of these effects is left
//!       to the caller.
//! @note This method does not apply bandwidth effects because the bandwidth of the signal
//!       may have changed due to reflection. The application of these effects is left
//!       to the caller.
// virtual
double WsfEM_Rcvr::ComputeReceivedPower(double                    aSourceAz,
                                        double                    aSourceEl,
                                        double                    aEBS_Az,
                                        double                    aEBS_El,
                                        double                    aReceivedPowerDensity,
                                        WsfEM_Types::Polarization aPolarization,
                                        double                    aFrequency,
                                        double&                   aAntennaGain)
{
   // Compute the antenna gain.
   aAntennaGain = GetAntennaGain(aPolarization, mFrequency, aSourceAz, aSourceEl, aEBS_Az, aEBS_El);

   double wavelength    = UtMath::cLIGHT_SPEED / mFrequency;
   double temp1         = wavelength * wavelength / UtMath::cFOUR_PI;
   double receivedPower = aReceivedPowerDensity * temp1 * aAntennaGain / mInternalLoss;
   return receivedPower;
}

// =================================================================================================
//! Compute the signal-to-noise ratio.
//! @param aReceivedSignalPower [input] The power of the signal to be detected.
//! @param aClutterPower        [input] The received clutter power.
//! @param aReceivedInterferencePower [input] The power of the signal that is attempting to interfere with the detection.
//! @returns The computed signal-to-noise ratio (absolute - not dB)
//! @note The input powers are at the output of the antenna terminals as determined by
//! ComputeReceivedPower.
// virtual
double WsfEM_Rcvr::ComputeSignalToNoise(double aReceivedSignalPower, double aClutterPower, double aReceivedInterferencePower)
{
   double signalToNoise =
      aReceivedSignalPower / (aClutterPower + aReceivedInterferencePower + (mNoisePower * mNoiseMultiplier));
   return signalToNoise;
}

// =================================================================================================
//! Initialize the receiver.
//! @returns 'true' if successful or false if some error was encountered.
// virtual
bool WsfEM_Rcvr::Initialize(WsfSimulation& aSimulation)
{
   // Initialize the base class
   bool ok = WsfEM_XmtrRcvr::Initialize(aSimulation);

   if (mFrequency == 0.0)
   {
      ut::log::error() << "Receiver frequency not specified.";
      ok = false;
   }

   UpdateNoisePower(); // Make sure the noise power is valid.
   UpdatePolarizationEffects();

   double simTime(aSimulation.GetSimTime());

   // Allow component factory to inject components and check dependencies.
   WsfComponentFactoryList<WsfEM_Rcvr>::PreInitialize(aSimulation.GetScenario(), simTime, *this);

   // Initialize components
   for (auto* componentPtr : mComponents)
   {
      ok &= componentPtr->Initialize(simTime);
   }

   return ok;
}

// =================================================================================================
//! Process input from a generic source.
//! @see WsfObject::ProcessInput
// virtual
bool WsfEM_Rcvr::ProcessInput(UtInput& aInput)
{
   bool myCommand = true;

   std::string command(aInput.GetCommand());
   if (command == "detection_threshold")
   {
      double detectionThreshold;
      aInput.ReadValueOfType(detectionThreshold, UtInput::cRATIO);
      aInput.ValueGreater(detectionThreshold, 0.0);
      SetDetectionThreshold(detectionThreshold);
   }
   else if ((command == "instantaneous_bandwidth") || (command == "analysis_bandwidth"))
   {
      double instantaneousBandwidth;
      aInput.ReadValueOfType(instantaneousBandwidth, UtInput::cFREQUENCY);
      aInput.ValueGreaterOrEqual(instantaneousBandwidth, 0.0);
      SetInstantaneousBandwidth(instantaneousBandwidth);
   }
   else if (command == "noise_power")
   {
      double noisePower;
      aInput.ReadValueOfType(noisePower, UtInput::cPOWER_DB);
      SetNoisePower(noisePower);
   }
   else if (command == "polarization_effect")
   {
      WsfEM_Types::Polarization polarization;
      std::string               polarizationStr;
      aInput.ReadValue(polarizationStr);
      if (!WsfEM_Util::StringToEnum(polarization, polarizationStr))
      {
         throw UtInput::BadValue(aInput, "Invalid polarization: " + polarizationStr);
      }
      double fraction;
      aInput.ReadValue(fraction);
      aInput.ValueInClosedRange(fraction, 0.0, 1.0);
      SetPolarizationEffect(polarization, fraction);
   }
   else if (command == "noise_figure")
   {
      double noiseFigure;
      aInput.ReadValueOfType(noiseFigure, UtInput::cRATIO);
      aInput.ValueGreaterOrEqual(noiseFigure, 1.0);
      SetNoiseFigure(noiseFigure);
   }
   else if (command == "antenna_ohmic_loss")
   {
      double antennaOhmicLoss;
      aInput.ReadValueOfType(antennaOhmicLoss, UtInput::cRATIO);
      if (antennaOhmicLoss != 0.0) // A value of zero is like not specifying a value
      {
         aInput.ValueGreaterOrEqual(antennaOhmicLoss, 1.0);
      }
      SetAntennaOhmicLoss(antennaOhmicLoss);
   }
   else if (command == "receive_line_loss")
   {
      double receiveLineLoss;
      aInput.ReadValueOfType(receiveLineLoss, UtInput::cRATIO);
      if (receiveLineLoss != 0.0) // A value of zero is like not specifying a value
      {
         aInput.ValueGreaterOrEqual(receiveLineLoss, 1.0);
      }
      SetReceiveLineLoss(receiveLineLoss);
   }
   else if (command == "check_transmitter_masking")
   {
      aInput.ReadValue(mCheckXmtrMasking);
   }
   else if (mComponents.ProcessComponentInput(aInput))
   {
      // First try components already attached. If the input was not recognized by one of them then
      // try the component factories, which may result in the dynamic creation of a new component.
   }
   else if (WsfComponentFactoryList<WsfEM_Rcvr>::ProcessInput(WsfScenario::FromInput(aInput), aInput, *this))
   {
      // If not processed by an attached component try the component factories to see if a component
      // should be dynamically attached.
   }
   else
   {
      myCommand = WsfEM_XmtrRcvr::ProcessInput(aInput);
   }
   return myCommand;
}

// =================================================================================================
//! A convenience method to check for and process a 'receiver/end_receiver' block.
// virtual
bool WsfEM_Rcvr::ProcessInputBlock(UtInput& aInput)
{
   bool myCommand = false;
   if (aInput.GetCommand() == "receiver")
   {
      myCommand = true;
      UtInputBlock inputBlock(aInput);
      while (inputBlock.ReadCommand())
      {
         if (!ProcessInput(aInput))
         {
            throw UtInput::UnknownCommand(aInput);
         }
      }
   }
   return myCommand;
}

// =================================================================================================
//! Get the effect of mis-matched frequency bandwidths between the receiver and supplied signal.
//! The caller should use this to effectively reduce the transmitter power by the ratio of the amount
//! of transmitter bandwidth that is within the receiver bandwidth and the total transmitter bandwidth.
//!
//! @param aSignalFreq The frequency of the signal being received.
//! @param aSignalBW   The bandwidth of the signal being received.
//! @return The fraction in the range [0..1] of the signal will be receivable.
double WsfEM_Rcvr::GetBandwidthEffect(double aSignalFreq, double aSignalBW) const
{
   double effBWRatio = 1.0;

   double rcvrBW = mBandwidth; // placeholder for temporarily modifying mBandwidth for the calculation

   // Determine if the frequency bandwidth of the receiver is covered by the transmitter.
   double signalFreqLo = aSignalFreq - (0.5 * aSignalBW);
   double signalFreqHi = aSignalFreq + (0.5 * aSignalBW);

   // If the receiver bandwidth is not set, the transmitter power cannot be calculated.  In this case,
   // if the receiver frequency is within the transmitter bandwidth, use the transmitter bandwidth
   if (rcvrBW == 0.0)
   {
      if ((signalFreqLo <= mFrequency) && (signalFreqHi >= mFrequency))
      {
         rcvrBW = aSignalBW;
      }
      else
      {
         effBWRatio = 0.0;
         return effBWRatio;
      }
   }

   // Recalculate the receivers bandwidth
   double rcvrFreqLo = mFrequency - (0.5 * rcvrBW);
   double rcvrFreqHi = mFrequency + (0.5 * rcvrBW);

   // Check to make sure signal does not reside outside the receivers
   // newly calculated bandwidth if changed above
   if ((signalFreqLo > rcvrFreqHi) || (signalFreqHi < rcvrFreqLo))
   {
      effBWRatio = 0.0;
      return effBWRatio;
   }

   // If the transmitter bandwidth is zero then we just assume all of the transmitter power is available.
   if (aSignalBW > 0.0)
   {
      double inbandFreqLo    = std::max(rcvrFreqLo, signalFreqLo);
      double inbandFreqHi    = std::min(rcvrFreqHi, signalFreqHi);
      double inbandBandwidth = std::max(inbandFreqHi - inbandFreqLo, 0.0); // 'max' check only for safety
      effBWRatio             = std::min((inbandBandwidth / aSignalBW), 1.0);
   }
   return effBWRatio;
}

// =================================================================================================
//! Get effect of receiving a signal of a specified polarization.
//! This method returns the fraction of the incoming signal that can be received as a function
//! of its polarization. For example, if the receiver is defined to process horizontally polarized signals
//! and it receives a horizontally polarized signal then the fraction will be 1.0. However if it receives
//! a vertically polarized signal then the fraction will be 0.0;
//!
//! @param aPolarization The polarization of the signal being received.
//! @return The fraction in the range [0..1] of the signal will be receivable.
double WsfEM_Rcvr::GetPolarizationEffect(WsfEM_Types::Polarization aPolarization) const
{
   double fraction = 1.0;
   if (mPolarization != WsfEM_Types::cPOL_DEFAULT)
   {
      if ((aPolarization >= 0) && (aPolarization < WsfEM_Types::cPOL_COUNT))
      {
         fraction = mPolarizationEffects[aPolarization];
      }
   }
   return fraction;
}

// =================================================================================================
//! Set the antenna ohmic loss.
//! This is used to determine the system noise temperature and the receiver noise.
//! @param aAntennaOhmicLoss The antenna ohmic loss factor [1..).
//! @note A value of 0 may be given, which effectively indicates the value is 'not defined'.
//! If both antenna ohmic loss and receive line loss are 'not defined' then the system noise
//! temperature is assumed to be 290 degK and the noise figure is just a multiplier. If either
//! or both values are given, the system noise temperature will be computed, with 'not defined'
//! values being given the value of 1.
void WsfEM_Rcvr::SetAntennaOhmicLoss(double aAntennaOhmicLoss)
{
   mAntennaOhmicLoss = aAntennaOhmicLoss;
   UpdateNoisePower();
}

// =================================================================================================
//! Set the receiver operating bandwidth.
//! @param aBandwidth The receiver operating bandwidth (Hz).
// virtual
void WsfEM_Rcvr::SetBandwidth(double aBandwidth)
{
   mBandwidth         = aBandwidth;
   mExplicitBandwidth = true;

   // Explicit bandwidth provided, check the instantaneous bandwidth
   // and set if not explicitly set
   if (!mExplicitInstantaneousBandwidth)
   {
      mInstantaneousBandwidth = mBandwidth;
   }

   // Calling UpdateNoisePower() will cause the noise power to be updated if no explicit
   // value was specified and the bandwidth was used to compute the noise power.
   UpdateNoisePower();

   if (mEM_ManagerPtr != nullptr)
   {
      mEM_ManagerPtr->UpdateRcvr(this);
   }
}

// =================================================================================================
//! Set the operating frequency.
//! @param aFrequency The operating frequency (Hz)
// virtual
void WsfEM_Rcvr::SetFrequency(double aFrequency)
{
   mFrequency = aFrequency;
   if (mEM_ManagerPtr != nullptr)
   {
      mEM_ManagerPtr->UpdateRcvr(this);
   }
}

// =================================================================================================
//! Set the receiver instantaneous bandwidth.
//! @param aInstantaneousBandwidth The receiver instantaneous bandwidth (Hz).
void WsfEM_Rcvr::SetInstantaneousBandwidth(double aInstantaneousBandwidth)
{
   mInstantaneousBandwidth         = aInstantaneousBandwidth;
   mExplicitInstantaneousBandwidth = true;

   // Explicit instantaneous bandwidth provided, check the bandwidth
   // and set if not explicitly set
   if (!mExplicitBandwidth)
   {
      mBandwidth = mInstantaneousBandwidth;
   }

   // Calling UpdateNoisePower() will cause the noise power to be updated if no explicit
   // value was specified and the instantaneous bandwidth was used to compute the noise power.
   UpdateNoisePower();
   if (mEM_ManagerPtr != nullptr)
   {
      mEM_ManagerPtr->UpdateRcvr(this);
   }
}

// =================================================================================================
//! Set the receiver noise figure.
//! This is used to determine the system noise temperature and the receiver noise.
//! @param aNoiseFigure The receiver noise figure (absolute, not dB).
//! @note If either the antenna ohmic loss or receive line loss is specified, the system noise
//! temperature, Ts, will be computed and the noise power will be computed as k * Ts * B.
//! If neither is specified, the noise power will be computed as k * T0 * B * Fn.
void WsfEM_Rcvr::SetNoiseFigure(double aNoiseFigure)
{
   mNoiseFigure = aNoiseFigure;
   UpdateNoisePower();
}

// =================================================================================================
//! Set the noise power level
//! @param aNoisePower Noise power level (Watts)
void WsfEM_Rcvr::SetNoisePower(double aNoisePower)
{
   mNoisePower         = aNoisePower;
   mExplicitNoisePower = true;

   // Specifying a noise power <= 0 causes the noise power to revert to the standard
   // value of k * T0 * B
   if (mNoisePower <= 0.0)
   {
      mExplicitNoisePower = false;
      UpdateNoisePower();
   }
}

// =================================================================================================
//! Set the polarization of the receive antenna.
//! @param aPolarization The polarization.
// virtual
void WsfEM_Rcvr::SetPolarization(WsfEM_Types::Polarization aPolarization)
{
   if ((aPolarization >= WsfEM_Types::cPOL_DEFAULT) && (aPolarization < WsfEM_Types::cPOL_COUNT))
   {
      mPolarization = aPolarization;
      UpdatePolarizationEffects();
   }
}

// =================================================================================================
//! Set the polarization effect for a specified polarization.
//! @param aPolarization The polarization for which the factor applies.
//! @param aFraction     The effect for the specified polarization in the range [0..1]
void WsfEM_Rcvr::SetPolarizationEffect(WsfEM_Types::Polarization aPolarization, double aFraction)
{
   if ((aPolarization >= WsfEM_Types::cPOL_DEFAULT) && (aPolarization < WsfEM_Types::cPOL_COUNT))
   {
      mPolarizationEffects[aPolarization]         = aFraction;
      mExplicitPolarizationEffects[aPolarization] = aFraction;
      UpdatePolarizationEffects();
   }
}

// =================================================================================================
//! Set the receive line loss.
//! This is used to determine the system noise temperature and the receiver noise.
//! @param aReceiveLineLoss The line loss factor [1..).
//! @note A value of 0 may be given, which effectively indicates the value is 'not defined'.
//! If both antenna ohmic loss and receive line loss are 'not defined' then the system noise
//! temperature is assumed to be 290 degK and the noise figure is just a multiplier. If either
//! or both values are given, the system noise temperature will be computed, with 'not defined'
//! values being given the value of 1.
void WsfEM_Rcvr::SetReceiveLineLoss(double aReceiveLineLoss)
{
   mReceiveLineLoss = aReceiveLineLoss;
   UpdateNoisePower();
}

// =================================================================================================
//! Add the specified transmitter to the list of potential interactors.
//! @param aXmtrPtr The transmitter to be added.
bool WsfEM_Rcvr::AddInteractor(WsfEM_Xmtr* aXmtrPtr)
{
   bool updated = false;

   switch (aXmtrPtr->GetFunction())
   {
   case WsfEM_Xmtr::cXF_COMM:
   {
      if (std::find(mCommInteractors.begin(), mCommInteractors.end(), aXmtrPtr) == mCommInteractors.end())
      {
         mCommInteractors.emplace_back(aXmtrPtr);
         updated = true;
      }
   }
   break;
   case WsfEM_Xmtr::cXF_INTERFERER:
   {
      if (std::find(mInterferenceInteractors.begin(), mInterferenceInteractors.end(), aXmtrPtr) ==
          mInterferenceInteractors.end())
      {
         mInterferenceInteractors.emplace_back(aXmtrPtr);
         updated = true;
      }
   }
   break;
   case WsfEM_Xmtr::cXF_SENSOR:
   {
      if (std::find(mSensorInteractors.begin(), mSensorInteractors.end(), aXmtrPtr) == mSensorInteractors.end())
      {
         mSensorInteractors.emplace_back(aXmtrPtr);
         updated = true;
      }
   }
   break;
   default:
      break;
   }
   UpdateIndices(); // Update data for GetInteractorCount/Entry

   // Inform any interested component.
   if (updated && mComponents.HasComponents())
   {
      for (auto* componentPtr : mComponents)
      {
         componentPtr->AddInteractor(aXmtrPtr);
      }
   }

   return updated;
}

// =================================================================================================
//! Remove the specified transmitter from the list of potential interactors.
//! This method is typically called by WsfEM_Manager whenever a transmitter is deactivated.
//! @param aXmtrPtr The transmitter to be removed.
bool WsfEM_Rcvr::RemoveInteractor(WsfEM_Xmtr* aXmtrPtr)
{
   bool updated = false;

   switch (aXmtrPtr->GetFunction())
   {
   case WsfEM_Xmtr::cXF_COMM:
   {
      InteractorList::iterator iter = std::find(mCommInteractors.begin(), mCommInteractors.end(), aXmtrPtr);
      if (iter != mCommInteractors.end())
      {
         mCommInteractors.erase(iter);
         updated = true;
      }
   }
   break;
   case WsfEM_Xmtr::cXF_INTERFERER:
   {
      InteractorList::iterator iter =
         std::find(mInterferenceInteractors.begin(), mInterferenceInteractors.end(), aXmtrPtr);
      if (iter != mInterferenceInteractors.end())
      {
         mInterferenceInteractors.erase(iter);
         updated = true;
      }
   }
   break;
   case WsfEM_Xmtr::cXF_SENSOR:
   {
      InteractorList::iterator iter = std::find(mSensorInteractors.begin(), mSensorInteractors.end(), aXmtrPtr);
      if (iter != mSensorInteractors.end())
      {
         mSensorInteractors.erase(iter);
         updated = true;
      }
   }
   break;
   default:
      break;
   }
   UpdateIndices(); // Update data for GetInteractorCount/Entry

   // Inform any interested component.
   if (updated && mComponents.HasComponents())
   {
      for (auto* componentPtr : mComponents)
      {
         componentPtr->RemoveInteractor(aXmtrPtr);
      }
   }

   return updated;
}

// =================================================================================================
//! Update the potential interaction list for the associated transmitter.
//!
//! This method is typically called by WsfEM_Manager to maintain the list of
//! transmitters that can potentially interact with this receiver. This uses
//! the CanInteractWith() method to determine if the receiver can interact with
//! the transmitter.
//! @param aXmtrPtr The transmitter to be evaluated.
void WsfEM_Rcvr::UpdateInteractions(WsfEM_Xmtr* aXmtrPtr)
{
   if (CanInteractWith(aXmtrPtr) && aXmtrPtr->AllowInteractionWith(this))
   {
      // This receiver can now potentially interact with the specified transmitter.
      // If the transmitter is not currently in the list then add it to the list.
      AddInteractor(aXmtrPtr);
   }
   else
   {
      // This receiver cannot potentially interact with the specified transmitter.
      // If the transmitter is currently in the list then remove it from the list.
      RemoveInteractor(aXmtrPtr);
   }
}

// =================================================================================================
//! Get the i'th potentially interacting transmitter of any class.
//! @param aIndex The index of the desired entry in the rage [0 .. GetInteractorCount() - 1].
//! @note The index should not be retained as it can change if AddInteractor() or RemoveInteractor() is called.
//! @note This is provided only for convenience of those that need to process all classes of transmitters.
//! If you only need a specific class (comm, sensor or interferer), then use the appropriate class-specified methods.
WsfEM_Xmtr* WsfEM_Rcvr::GetInteractorEntry(unsigned int aIndex) const
{
   // NOTE: The processing order must not be changed without also changing UpdateIndices.
   // The order below will result in processing comm entries first, sensor entries second and interferer entries last.
   WsfEM_Xmtr* xmtrPtr = nullptr;
   if (aIndex < mSensorBaseIndex)
   {
      xmtrPtr = mCommInteractors[aIndex - mCommBaseIndex];
   }
   else if (aIndex < mInterferenceBaseIndex)
   {
      xmtrPtr = mSensorInteractors[aIndex - mSensorBaseIndex];
   }
   else if (aIndex < mTotalInteractors)
   {
      xmtrPtr = mInterferenceInteractors[aIndex - mInterferenceBaseIndex];
   }
   assert(xmtrPtr != nullptr);
   return xmtrPtr;
}

// =================================================================================================
//! A transmitter to which this receiver is listening is emitting.
//!
//! @param aSimTime The current simulation time.
//! @param aResult  The interaction object representing the transmission. aResult.GetTransmitter()
//!                 returns the emitting transmitter.
//!
//! @note See WsfEM_Xmtr::AddListener for a discussion of listeners.
void WsfEM_Rcvr::EmitterActiveCallback(double aSimTime, WsfEM_Interaction& aResult)
{
   // The base class does nothing.
}

// =================================================================================================
//! A targets transmitter to which this receiver is listening has changed requiring a target update.
//!
//! @param aSimTime      The current simulation time.
//! @param aTargetIndex  The target representing the transmission change.
//!
//! @note See WsfEM_Xmtr::AddListener for a discussion of listeners.
void WsfEM_Rcvr::SignalChangeCallback(double aSimTime, size_t aTargetIndex)
{
   // The base class does nothing.
}

// =================================================================================================
//! Update the noise power from other values if one hasn't been explicitly provided.
//!
//! If a noise power has not been explicitly provided by SetNoisePower() then this
//! routine will calculate the noise power based on:
//!
//! noise_power = k * Ts * B
//!
//! - k   is the Boltzmann constant
//! - Ts  is the system noise temperature (nominally 290 degrees Kelvin)
//! - B   is the bandwidth in Hertz.
//!
//! @param aPulseWidth If the noise power must be calculated and if the instantaneous
//! bandwidth or bandwidth hasn't been defined AND these values are less than zero,
//! then the instantaneous bandwidth and bandwidth will be assumed to be (1 / this value).
void WsfEM_Rcvr::UpdateNoisePower(double aPulseWidth /* = 0.0 */)
{
   // Calculate the noise power if not explicitly specified
   if (!mExplicitNoisePower)
   {
      if (mExplicitInstantaneousBandwidth && (mInstantaneousBandwidth > 0.0))
      {
         // Explicit instantaneous bandwidth provided, check the bandwidth
         // and set if not explicitly set
         if ((!mExplicitBandwidth))
         {
            mBandwidth = mInstantaneousBandwidth;
         }
      }
      else if (mExplicitBandwidth && (mBandwidth > 0.0))
      {
         // Explicit bandwidth provided, check the instantaneous bandwidth
         // and set if not explicitly set
         if ((!mExplicitInstantaneousBandwidth))
         {
            mInstantaneousBandwidth = mBandwidth;
         }
      }
      else if (aPulseWidth > 0.0)
      {
         // Instantaneous bandwidth and bandwidth are not explicitly specified or are both zero
         // and pulse width is non-zero.  Use the reciprocal of the pulse width as the
         // instantaneous bandwidth and bandwidth.
         mInstantaneousBandwidth = 1.0 / aPulseWidth;
         mBandwidth              = mInstantaneousBandwidth;
      }

      // If a bandwidth is available then compute the noise power from 'N = k * Ts * B'
      if (mInstantaneousBandwidth > 0.0)
      {
         double noiseFigure = mNoiseFigure;
         if (noiseFigure <= 0.0)
         {
            noiseFigure = 1.0;
         }
         if ((mAntennaOhmicLoss <= 0.0) && (mReceiveLineLoss <= 0.0))
         {
            // Neither 'antenna_ohmic_loss' nor 'receive_line_loss' was specified.
            // Compute the noise power and 'N = k * T0 * B * Fn'
            mNoisePower = UtMath::cBOLTZMANN_CONSTANT * 290.0 * mInstantaneousBandwidth * noiseFigure;
         }
         else
         {
            // Either antenna_ohmic_loss or receive_line_loss was specified.
            // Compute the system temperature using the algorithm defined in Blake.

            // Determine the nominal elevation angle for computing the system temperature.
            double elevation = 0.0;
            if ((mAntennaPtr != nullptr) && (mAntennaPtr->GetArticulatedPart() != nullptr))
            {
               WsfArticulatedPart* partPtr = mAntennaPtr->GetArticulatedPart();
               // Add the beam tilt, antenna tilt, the articulated part tilt and the articulated part pitch
               // together to get the default elevation angle. Depending on the type of system being modeled
               // (physical azimuth scan, fixed pitch, multi-beam, etc.), the result should give the nominal
               // elevation angle.
               elevation = GetBeamTilt() + mAntennaPtr->GetPitch() + partPtr->GetTilt() + partPtr->GetPitch();

               // If the antenna tracks or scans in the vertical direction then use the mid-point of its limits.
               WsfArticulatedPart::SlewMode slewMode = partPtr->GetSlewMode();
               WsfEM_Antenna::ScanMode      scanMode = mAntennaPtr->GetScanMode();
               if (slewMode & WsfArticulatedPart::cSLEW_EL)
               {
                  elevation = 0.5 * (partPtr->GetMinElSlew() + partPtr->GetMaxElSlew());
               }
               else if (scanMode & WsfEM_Antenna::cSCAN_EL)
               {
                  elevation =
                     0.5 * (mAntennaPtr->GetMinimumElevationScanLimit() + mAntennaPtr->GetMaximumElevationScanLimit());
               }
            }

            double antennaOhmicLoss = mAntennaOhmicLoss;
            double receiveLineLoss  = mReceiveLineLoss;
            if (antennaOhmicLoss <= 0.0)
            {
               antennaOhmicLoss = 1.0;
            }
            if (receiveLineLoss <= 0.0)
            {
               receiveLineLoss = 1.0;
            }
            double systemTemperature =
               ComputeSystemNoiseTemperature(elevation, antennaOhmicLoss, receiveLineLoss, noiseFigure, mFrequency);
            // N = k * Ts * B;
            mNoisePower = UtMath::cBOLTZMANN_CONSTANT * systemTemperature * mInstantaneousBandwidth;
         }
      }
      else
      {
         mNoisePower = 1.0E-16; // -160 dB
      }
   }
}

// =================================================================================================
//! Compute the system noise temperature in a receiver.
//!
//! This method computes the system noise temperature as defined by:
//!
//! Ref 1: "Radar Range Performance", Lamont V. Blake, 1986, Artech House, Inc., Chapter 4.
//!
//! @param aElevation              Elevation angle above the horizontal (radians).
//! @param aAntennaOhmicLossFactor The antenna ohmic loss factor (>= 1.0).
//!                                (See the note below concerning loss factors).
//! @param aReceiveLineLossFactor  The line loss factor between the antenna and the receiver (>= 1.0).
//!                                (See the note below concerning loss factors).
//! @param aNoiseFigure            The receiver noise figure. This should be greater than 1.
//! @param aFrequency              The operating frequency (Hertz)
//!
//! @returns The system noise temperature (in deg K);
//!
//! @note The term 'loss factor' means the reciprocal of the gain where gain is computed as the ratio of
//! 'output power' / 'input power'. Thus, a loss factor should always be greater than or equal to one!
// static
double WsfEM_Rcvr::ComputeSystemNoiseTemperature(double aElevation,
                                                 double aAntennaOhmicLossFactor,
                                                 double aReceiveLineLossFactor,
                                                 double aNoiseFigure,
                                                 double aFrequency)
{
   // Ambient temperature in deg-K.
   static const double temamb = 290.0;

   // Angle are in radians.
   static const double angtab[7] = {0.0000, 0.0175, 0.0349, 0.0873, 0.1745, 0.5236, 1.5708};

   // Frequencies are in MHz
   static const double frq[26] = {0.0,    10.0,   100.0,   200.0,   300.0,   400.0,   500.0,   600.0,  700.0,
                                  800.0,  900.0,  1000.0,  2000.0,  3000.0,  4000.0,  5000.0,  6000.0, 7000.0,
                                  8000.0, 9000.0, 10000.0, 12000.0, 16000.0, 20000.0, 22000.0, 30000.0};

   static const double tantab[26][7] = {
      //     0        1        2        5       10       30       90
      {10.0E6, 10.0E6, 10.0E6, 10.0E6, 10.0E6, 10.0E6, 10.0E6}, // 0 MHz
      {1.0E6, 1.0E6, 1.0E6, 1.0E6, 1.0E6, 1.0E6, 1.0E6},        // 10 MHz
      {3.0E3, 3.0E3, 3.0E3, 3.0E3, 3.0E3, 3.0E3, 3.0E3},        // 100 MHz
      {545.0, 545.0, 545.0, 545.0, 545.0, 545.0, 545.0},        // 200 MHz
      {225.0, 225.0, 225.0, 225.0, 225.0, 225.0, 225.0},        // 300 MHz
      {150.0, 150.0, 150.0, 150.0, 150.0, 150.0, 150.0},        // 400 MHz
      {120.0, 110.0, 110.0, 110.0, 110.0, 110.0, 110.0},        // 500 MHz
      {100.0, 90.0, 82.0, 72.0, 71.0, 70.0, 70.0},              // 600 MHz
      {93.0, 75.0, 69.1, 55.0, 51.0, 45.7, 45.7},               // 700 MHz
      {90.0, 70.0, 60.0, 48.0, 42.0, 35.5, 35.0},               // 800 MHz
      {90.0, 68.0, 54.0, 41.0, 33.0, 29.0, 28.0},               // 900 MHz
      {89.1, 65.0, 52.0, 38.0, 30.0, 24.0, 23.0},               // 1000 MHz
      {95.5, 63.0, 46.0, 27.0, 18.0, 11.0, 9.0},                // 2000 MHz
      {100.0, 63.5, 46.0, 26.0, 17.0, 9.0, 7.0},                // 3000 MHz
      {104.7, 66.1, 48.0, 26.0, 16.0, 8.3, 6.2},                // 4000 MHz
      {110.0, 68.0, 50.0, 27.0, 16.0, 8.0, 6.0},                // 5000 MHz
      {114.8, 70.5, 50.0, 28.0, 17.0, 8.0, 6.0},                // 6000 MHz
      {120.2, 72.0, 52.0, 29.0, 17.5, 8.1, 6.0},                // 7000 MHz
      {126.0, 75.0, 53.0, 30.0, 18.0, 8.2, 6.0},                // 8000 MHz
      {130.0, 80.0, 58.0, 31.0, 18.5, 8.5, 6.0},                // 9000 MHz
      {135.0, 85.0, 61.0, 32.0, 19.0, 9.0, 6.2},                // 10000 MHz
      {160.0, 100.0, 70.0, 39.0, 21.5, 10.0, 6.5},              // 12000 MHz
      {230.0, 150.0, 125.0, 64.0, 45.0, 16.0, 9.0},             // 16000 MHz
      {280.0, 250.0, 220.0, 140.0, 90.0, 40.0, 20.0},           // 20000 MHz
      {280.0, 280.0, 275.0, 210.0, 150.0, 70.0, 40.0},          // 22000 MHz
      {275.0, 240.0, 190.0, 110.0, 70.0, 29.0, 17.0}            // 30000 MHz
   };

   double elang  = aElevation;
   double aloss  = aAntennaOhmicLossFactor;
   double rloss  = aReceiveLineLossFactor;
   double rxnf   = aNoiseFigure;
   double freqmh = aFrequency * 1.0E-6;

   // Determine two frequency factors for use in computing atmospheric attenuation.

   int    hi     = 25;
   double frqfac = 1.0;
   int    ifreq  = hi - 1;
   if (freqmh <= frq[hi])
   {
      int lo = 0;
      while ((lo + 1) < hi)
      {
         int mid = (lo + hi) / 2;
         if (freqmh < frq[mid])
         {
            hi = mid;
         }
         else
         {
            lo = mid;
         }
      }
      ifreq  = lo;
      frqfac = (freqmh - frq[lo]) / (frq[hi] - frq[lo]);
   }

   int    iang   = 0;
   double angfac = 0.0;
   if (elang < 0.0)
   {
      iang   = 0;
      angfac = 0.0;
   }
   else if (elang >= angtab[6])
   {
      iang   = 6;
      angfac = 1.0;
   }
   else
   {
      iang = 0;
      while (iang <= 5)
      {
         int iap1 = iang + 1;
         if (elang < angtab[iap1])
         {
            angfac = (elang - angtab[iang]) / (angtab[iap1] - angtab[iang]);
            break;
         }
         ++iang;
      }
   }

   // Calculate the antenna noise temperature for a lossless antenna by using the antenna table
   // and linear interpolation in frequency and logarithmic interpolation in noise temperature.

   int ifp1 = ifreq + 1;
   int iap1 = iang + 1;

   double tanllg = (frqfac * log10(tantab[ifp1][iang])) + ((1.0 - frqfac) * log10(tantab[ifreq][iang]));

   double tanrlg = (frqfac * log10(tantab[ifp1][iap1])) + ((1.0 - frqfac) * log10(tantab[ifreq][iap1]));

   double tanplg = (angfac * tanrlg) + ((1.0 - angfac) * tanllg);
   double tantp  = pow(10.0, tanplg);

   // Adjust the antenna noise temperature for a non-lossless antenna and for a standard ground
   // contribution using Equation 4.76a of Reference 1, page 172.

   double tant = (0.876 * tantp - 254.0) / aloss + temamb;

   // Calculate the noise contribution due to the receive transmission line. The transmission
   // line thermal temperature is assumed to be the ambient thermal temperature.

   double ttxl = temamb * (rloss - 1.0);

   // Compute the noise contribution due to the receiver per Equation 4.28 of Reference 1, page 147.

   double teff = temamb * (rxnf - 1.0);

   // Compute the total system noise temperature per Equation 4.41 of Reference 1, page 152.

   double tsubs = tant + ttxl + (rloss * teff);
   return tsubs;
}

// =================================================================================================
//! Update indices used by GetInteractorCount() and GetInteractorEntry().
//! This method must be called whenever the interactor lists are modified.
// private
void WsfEM_Rcvr::UpdateIndices()
{
   // NOTE: The order in which the indices are computed must not be changed without also changing GetInteractorEntry.
   // The order below will result in processing comm entries first, sensor entries second and interferer entries last.
   mCommBaseIndex         = 0;
   mSensorBaseIndex       = mCommBaseIndex + static_cast<unsigned int>(mCommInteractors.size());
   mInterferenceBaseIndex = mSensorBaseIndex + static_cast<unsigned int>(mSensorInteractors.size());
   mTotalInteractors      = mInterferenceBaseIndex + static_cast<unsigned int>(mInterferenceInteractors.size());
}

// =================================================================================================
//! Update the polarization effects table.
//! This method must be called whenever the signal polarization changes or the effects table changes.
// private
void WsfEM_Rcvr::UpdatePolarizationEffects()
{
   // Determine the default effects for the current polarization.

   double defaultEffects[WsfEM_Types::cPOL_COUNT];
   if (mPolarization == WsfEM_Types::cPOL_HORIZONTAL)
   {
      defaultEffects[WsfEM_Types::cPOL_DEFAULT]        = 1.0;
      defaultEffects[WsfEM_Types::cPOL_HORIZONTAL]     = 1.0;
      defaultEffects[WsfEM_Types::cPOL_VERTICAL]       = 0.0;
      defaultEffects[WsfEM_Types::cPOL_SLANT_45]       = 0.5;
      defaultEffects[WsfEM_Types::cPOL_SLANT_135]      = 0.5;
      defaultEffects[WsfEM_Types::cPOL_LEFT_CIRCULAR]  = 0.5;
      defaultEffects[WsfEM_Types::cPOL_RIGHT_CIRCULAR] = 0.5;
   }
   else if (mPolarization == WsfEM_Types::cPOL_VERTICAL)
   {
      defaultEffects[WsfEM_Types::cPOL_DEFAULT]        = 1.0;
      defaultEffects[WsfEM_Types::cPOL_HORIZONTAL]     = 0.0;
      defaultEffects[WsfEM_Types::cPOL_VERTICAL]       = 1.0;
      defaultEffects[WsfEM_Types::cPOL_SLANT_45]       = 0.5;
      defaultEffects[WsfEM_Types::cPOL_SLANT_135]      = 0.5;
      defaultEffects[WsfEM_Types::cPOL_LEFT_CIRCULAR]  = 0.5;
      defaultEffects[WsfEM_Types::cPOL_RIGHT_CIRCULAR] = 0.5;
   }
   else if (mPolarization == WsfEM_Types::cPOL_SLANT_45)
   {
      defaultEffects[WsfEM_Types::cPOL_DEFAULT]        = 1.0;
      defaultEffects[WsfEM_Types::cPOL_HORIZONTAL]     = 0.5;
      defaultEffects[WsfEM_Types::cPOL_VERTICAL]       = 0.5;
      defaultEffects[WsfEM_Types::cPOL_SLANT_45]       = 1.0;
      defaultEffects[WsfEM_Types::cPOL_SLANT_135]      = 0.0;
      defaultEffects[WsfEM_Types::cPOL_LEFT_CIRCULAR]  = 0.5;
      defaultEffects[WsfEM_Types::cPOL_RIGHT_CIRCULAR] = 0.5;
   }
   else if (mPolarization == WsfEM_Types::cPOL_SLANT_135)
   {
      defaultEffects[WsfEM_Types::cPOL_DEFAULT]        = 1.0;
      defaultEffects[WsfEM_Types::cPOL_HORIZONTAL]     = 0.5;
      defaultEffects[WsfEM_Types::cPOL_VERTICAL]       = 0.5;
      defaultEffects[WsfEM_Types::cPOL_SLANT_45]       = 0.0;
      defaultEffects[WsfEM_Types::cPOL_SLANT_135]      = 1.0;
      defaultEffects[WsfEM_Types::cPOL_LEFT_CIRCULAR]  = 0.5;
      defaultEffects[WsfEM_Types::cPOL_RIGHT_CIRCULAR] = 0.5;
   }
   else if (mPolarization == WsfEM_Types::cPOL_LEFT_CIRCULAR)
   {
      defaultEffects[WsfEM_Types::cPOL_DEFAULT]        = 1.0;
      defaultEffects[WsfEM_Types::cPOL_HORIZONTAL]     = 0.5;
      defaultEffects[WsfEM_Types::cPOL_VERTICAL]       = 0.5;
      defaultEffects[WsfEM_Types::cPOL_SLANT_45]       = 0.5;
      defaultEffects[WsfEM_Types::cPOL_SLANT_135]      = 0.5;
      defaultEffects[WsfEM_Types::cPOL_LEFT_CIRCULAR]  = 1.0;
      defaultEffects[WsfEM_Types::cPOL_RIGHT_CIRCULAR] = 0.0;
   }
   else if (mPolarization == WsfEM_Types::cPOL_RIGHT_CIRCULAR)
   {
      defaultEffects[WsfEM_Types::cPOL_DEFAULT]        = 1.0;
      defaultEffects[WsfEM_Types::cPOL_HORIZONTAL]     = 0.5;
      defaultEffects[WsfEM_Types::cPOL_VERTICAL]       = 0.5;
      defaultEffects[WsfEM_Types::cPOL_SLANT_45]       = 0.5;
      defaultEffects[WsfEM_Types::cPOL_SLANT_135]      = 0.5;
      defaultEffects[WsfEM_Types::cPOL_LEFT_CIRCULAR]  = 0.0;
      defaultEffects[WsfEM_Types::cPOL_RIGHT_CIRCULAR] = 1.0;
   }
   else
   {
      defaultEffects[WsfEM_Types::cPOL_DEFAULT]        = 1.0;
      defaultEffects[WsfEM_Types::cPOL_HORIZONTAL]     = 1.0;
      defaultEffects[WsfEM_Types::cPOL_VERTICAL]       = 1.0;
      defaultEffects[WsfEM_Types::cPOL_SLANT_45]       = 1.0;
      defaultEffects[WsfEM_Types::cPOL_SLANT_135]      = 1.0;
      defaultEffects[WsfEM_Types::cPOL_LEFT_CIRCULAR]  = 1.0;
      defaultEffects[WsfEM_Types::cPOL_RIGHT_CIRCULAR] = 1.0;
   }

   // For each polarization use the explicit effect if specified, otherwise use the default effect.

   for (unsigned int i = 0; i < WsfEM_Types::cPOL_COUNT; ++i)
   {
      mPolarizationEffects[i] = defaultEffects[i];
      if (mUsedExplicitPolarization[i])
      {
         mPolarizationEffects[i] = 1.0;
      }
      else if (mExplicitPolarizationEffects[i] >= 0.0)
      {
         mPolarizationEffects[i] = mExplicitPolarizationEffects[i];
      }
   }
}
