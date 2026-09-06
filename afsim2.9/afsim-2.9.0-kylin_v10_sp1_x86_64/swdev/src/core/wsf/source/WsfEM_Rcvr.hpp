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

#ifndef WSFEM_RCVR_HPP
#define WSFEM_RCVR_HPP

#include "wsf_export.h"

#include <string>
#include <vector>

class UtInput;
class WsfAntennaPattern;
class WsfArticulatedPart;
#include "WsfComponent.hpp"
#include "WsfComponentList.hpp"
class WsfEM_Antenna;
class WsfEM_Interaction;
class WsfEM_Rcvr;
#include "WsfEM_Types.hpp"
class WsfEM_Xmtr;
#include "WsfEM_XmtrRcvr.hpp"
class WsfPlatform;

//! An abstract class that defines extension component that can be added to a receiver.
//! Such a component must derive from this class and must implement the abstract methods as required by
//! WsfComponent. It may also implement any of the optional methods defined by this class or by
//! WsfComponent.
class WSF_EXPORT WsfEM_RcvrComponent : public WsfComponentT<WsfEM_Rcvr>
{
public:
   //! This class supports the follow methods defined in WsfComponentT
   //! - ProcessInput
   //! - PreInitialize

   //! @name Receiver-specific component methods.
   //@{
   virtual void AddInteractor(WsfEM_Xmtr* aXmtrPtr);
   virtual void RemoveInteractor(WsfEM_Xmtr* aXmtrPtr);
   //@}
};

//! A receiver of electromagnetic radiation.
//!
//! This can be used to implement the receiver radar receivers, passive sensors or
//! RF communications receivers.
//!
//! To use one of these within a system, the following protocol must be observed
//!
//! - Set the attributes of the object (using ProcessInput or the Set methods)
//! - Ensure that a pointer to a valid WsfEM_Antenna object has been provided via
//! either the (copy) constructor or SetAntenna() method.
//! - Invoke the Initialize() method when all of the attributes have been defined.
//! (This is typically done within the Initialize() method of the containing system).
//! - The Activate() method must be called when the system is turned on.
//! (This is typically done within the TurnOn() method of the containing system).
//! - The Deactivate() method must be called when the system is turned off.
//! (This is typically done within the TurnOff() method of the containing system).
class WSF_EXPORT WsfEM_Rcvr : public WsfEM_XmtrRcvr
{
public:
   //! @name Definitions to support the component list on the receiver.
   //@{
   //! Even though this isn't a component, it must have a role in order to use component factories.

   using Component     = WsfEM_RcvrComponent;
   using ComponentList = WsfComponentListT<WsfEM_RcvrComponent>;
   //@}

   //! RcvrFunction defines the intended purpose of the receiver.
   enum RcvrFunction
   {
      cRF_UNDEFINED,      //!< Undefined
      cRF_COMM,           //!< Communications
      cRF_SENSOR,         //!< Active or Semi-Active Sensing (narrowband or matched frequency sensing, e.g. radar)
      cRF_PASSIVE_SENSOR, //!< Passive Sensing (wideband sensing, etc.)
      cRF_INTERFERER      //!< Interfering system
   };

   WsfEM_Rcvr(RcvrFunction aFunction, WsfEM_Antenna* aAntennaPtr = nullptr);

   WsfEM_Rcvr(const WsfEM_Rcvr& aSrc, WsfEM_Antenna* aAntennaPtr = nullptr);

   ~WsfEM_Rcvr() override;

   //! Required to display error messages in WsfComponentFactoryList.
   std::string GetName() const { return "receiver"; }

   //! @name Component list methods.
   //!@{
   ComponentList&       GetComponents() { return mComponents; }
   const ComponentList& GetComponents() const { return mComponents; }
   //@}

   const char* GetScriptClassName() const override { return "WsfEM_Rcvr"; }

   void Activate();

   virtual void Deactivate();

   virtual double ComputeReceivedPower(double                    aSourceAz,
                                       double                    aSourceEl,
                                       double                    aEBS_Az,
                                       double                    aEBS_El,
                                       double                    aReceivedPowerDensity,
                                       WsfEM_Types::Polarization aPolarization,
                                       double                    aFrequency,
                                       double&                   aAntennaGain);

   virtual double ComputeSignalToNoise(double aReceivedSignalPower, double aClutterPower, double aReceivedInterferencePower);

   bool Initialize(WsfSimulation& aSimulation) override;

   bool ProcessInput(UtInput& aInput) override;

   virtual bool ProcessInputBlock(UtInput& aInput);

   double GetBandwidthEffect(double aSignalFreq, double aSignalBW) const;

   //! Get the minimum detectable signal-to-noise ratio
   //! @return the minimum detection signal-to-noise (absolute ratio, NOT dB)
   double GetDetectionThreshold() const { return mDetectionThreshold; }

   //! Get the intended function of the receiver.
   //! @returns The intended function of the receiver.
   RcvrFunction GetFunction() const { return mFunction; }

   //! Get the receiver bandwidth.
   //! @return The receiver bandwidth (Hz).
   double GetInstantaneousBandwidth() const { return mInstantaneousBandwidth; }

   //! Get the noise power level
   //! @return Noise power (Watts)
   double GetNoisePower() const { return mNoisePower; }

   //! Return the source of the noise power
   //! @return true if noise power was defined in the input file
   bool IsNoisePowerExplicit() const { return mExplicitNoisePower; }

   //! Get the noise figure
   //! @return Noise figure (Linear)
   double GetNoiseFigure() const { return mNoiseFigure; }

   //! Get the noise power level
   //! @return The current noise multiplier  (absolute ratio, NOT dB)
   //! @note This is the multiplier that is used in SNR calculations.
   double GetNoiseMultiplier() const { return mNoiseMultiplier; }

   //! Set the noise multiplier
   //! @param aNoiseMultiplier The noise multiplier (absolute ratio, NOT dB)
   void SetNoiseMultiplier(double aNoiseMultiplier) { mNoiseMultiplier = aNoiseMultiplier; }

   double GetPolarizationEffect(WsfEM_Types::Polarization aPolarization) const;

   void SetAntennaOhmicLoss(double aAntennaOhmicLoss);

   double GetAntennaOhmicLoss() { return mAntennaOhmicLoss; }

   //! Set the threshold required for detection.
   //! @param aDetectionThreshold The minimum detectable signal-to-noise (absolute ratio, NOT dB)
   void SetDetectionThreshold(double aDetectionThreshold) { mDetectionThreshold = aDetectionThreshold; }

   void SetBandwidth(double aBandwidth) override;

   void SetFrequency(double aFrequency) override;

   void SetInstantaneousBandwidth(double aInstantaneousBandwidth);

   void SetNoiseFigure(double aNoiseFigure);

   void SetNoisePower(double aNoisePower);

   void SetPolarization(WsfEM_Types::Polarization aPolarization) override;

   void SetPolarizationEffect(WsfEM_Types::Polarization aPolarization, double aFraction);

   void SetReceiveLineLoss(double aReceiveLineLoss);

   double GetReceiveLineLoss() const { return mReceiveLineLoss; }

   //! @name Interaction methods.
   //@{

   virtual bool CanInteractWith(WsfEM_Xmtr* aXmtrPtr);

   virtual void UpdateInteractions(WsfEM_Xmtr* aXmtrPtr);

   virtual bool AddInteractor(WsfEM_Xmtr* aXmtrPtr);

   virtual bool RemoveInteractor(WsfEM_Xmtr* aXmtrPtr);

   //! Get the number of potentially interacting communications transmitters
   size_t GetCommInteractorCount() const { return mCommInteractors.size(); }

   //! Get the i'th potentially interacting communications transmitter.
   //! @param aIndex The index of the desired entry in the rage [0 .. GetCommInteractorCount() - 1].
   //! @note The index should not be retained as it can change if AddInteractor() or RemoveInteractor() is called.
   WsfEM_Xmtr* GetCommInteractorEntry(unsigned int aIndex) const { return mCommInteractors[aIndex]; }

   //! Get the number of potentially interacting interference transmitters
   size_t GetInterferenceInteractorCount() const { return mInterferenceInteractors.size(); }

   //! Get the i'th potentially interacting interference transmitter.
   //! @param aIndex The index of the desired entry in the rage [0 .. GetInterferenceInteractorCount() - 1].
   //! @note The index should not be retained as it can change if AddInteractor() or RemoveInteractor() is called.
   WsfEM_Xmtr* GetInterferenceInteractorEntry(size_t aIndex) const { return mInterferenceInteractors[aIndex]; }

   //! Get the number of potentially interacting sensor transmitters
   size_t GetSensorInteractorCount() const { return mSensorInteractors.size(); }

   //! Get the i'th potentially interacting sensor transmitter.
   //! @param aIndex The index of the desired entry in the rage [0 .. GetSensorInteractorCount() - 1].
   //! @note The index should not be retained as it can change if AddInteractor() or RemoveInteractor() is called.
   WsfEM_Xmtr* GetSensorInteractorEntry(size_t aIndex) const { return mSensorInteractors[aIndex]; }

   //! Get the number of potentially interacting transmitters of any class.
   //! @note This is provided only for convenience of those that need to process all classes of transmitters.
   //! If you only need a specific class (comm, sensor or interferer), then use the appropriate class-specified methods.
   unsigned int GetInteractorCount() const { return mTotalInteractors; }

   //! Get the i'th potentially interacting transmitter of any class.
   //! @param aIndex The index of the desired entry in the rage [0 .. GetInteractorCount() - 1].
   //! @note The index should not be retained as it can change if AddInteractor() or RemoveInteractor() is called.
   //! @note This is provided only for convenience of those that need to process all classes of transmitters.
   //! If you only need a specific class (comm, sensor or interferer), then use the appropriate class-specified methods.
   WsfEM_Xmtr* GetInteractorEntry(unsigned int aIndex) const;

   virtual void EmitterActiveCallback(double aSimTime, WsfEM_Interaction& aResult);
   virtual void SignalChangeCallback(double aSimTime, size_t aTargetIndex);
   //@}

   //! @name Internal utility functions.
   //@{
   void UpdateNoisePower(double aPulseWidth = 0.0);

   static double ComputeSystemNoiseTemperature(double aElevation,
                                               double aAntennaOhmicLoss,
                                               double aReceiveLineLoss,
                                               double aNoiseFigure,
                                               double aFrequency);
   //@}

   //! Check for transmitter masking, includes terrain check flag also
   //! Transmitter flag utilized primarily for bistatic interactions.
   bool CheckXmtrMasking() { return mCheckXmtrMasking && CheckMasking(); }

protected:
   void UpdateIndices();

   using InteractorList = std::vector<WsfEM_Xmtr*>;
   //! @name Lists of transmitters that can potentially interact with this receiver.
   //! This is used by interferers and passive sensors to speed up processing.
   //@{
   InteractorList mCommInteractors;
   InteractorList mSensorInteractors;
   InteractorList mInterferenceInteractors;
   //@}

private:
   void UpdatePolarizationEffects();

   //! The list of extension components for the receiver.
   ComponentList mComponents;

   //! Pointer to the EM manager when active.  May be null if inactive
   WsfEM_Manager* mEM_ManagerPtr;

   //! @name The following are required to support GetInteractorCount and GetInteractorEntry.
   //@{
   unsigned int mTotalInteractors;
   unsigned int mCommBaseIndex;
   unsigned int mSensorBaseIndex;
   unsigned int mInterferenceBaseIndex;
   //@}

   RcvrFunction mFunction;
   double       mPolarizationEffects[WsfEM_Types::cPOL_COUNT];
   double       mExplicitPolarizationEffects[WsfEM_Types::cPOL_COUNT];
   double       mInstantaneousBandwidth;
   double       mNoisePower;
   double       mNoiseFigure;
   double       mNoiseMultiplier;
   double       mAntennaOhmicLoss;
   double       mReceiveLineLoss;
   double       mDetectionThreshold;
   bool         mExplicitInstantaneousBandwidth;
   bool         mExplicitNoisePower;

   bool mCheckXmtrMasking;
};

#endif
