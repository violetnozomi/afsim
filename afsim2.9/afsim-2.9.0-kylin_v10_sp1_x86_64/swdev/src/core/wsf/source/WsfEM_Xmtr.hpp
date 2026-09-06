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

#ifndef WSFEM_XMTR_HPP
#define WSFEM_XMTR_HPP

#include "wsf_export.h"

#include <map>
#include <vector>

class WsfAntennaPattern;
#include "WsfComponent.hpp"
#include "WsfComponentList.hpp"
class WsfEM_Antenna;
class WsfEM_Interaction;
class WsfEM_Rcvr;
#include "WsfEM_Types.hpp"
class WsfEM_Xmtr;
#include "WsfEM_XmtrRcvr.hpp"
class WsfPlatform;
#include "WsfStringId.hpp"

//! An abstract class that defines extension component that can be added to a transmitter.
//! Such a component must derive from this class and must implement the abstract methods as required by
//! WsfComponent. It may also implement any of the optional methods defined by this class or by
//! WsfComponent.
using WsfEM_XmtrComponent = WsfComponentT<WsfEM_Xmtr>;

//! A transmitter of electromagnetic radiation.
//!
//! This can be used to implement the transmitters in radars, RF interferers or
//! RF communications.
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
class WSF_EXPORT WsfEM_Xmtr : public WsfEM_XmtrRcvr
{
public:
   //! @name Definitions to support the component list on the transmitter.
   //@{
   //! Even though this isn't a component, it must have a role in order to use component factories.
   using Component     = WsfEM_XmtrComponent;
   using ComponentList = WsfComponentListT<WsfEM_XmtrComponent>;
   //@}

   //! Type definition for the frequency dependent power
   using PowerList = std::vector<std::pair<double, double>>;

   //! XmtrFunction defines the intended purpose of the transmitter.
   enum XmtrFunction
   {
      cXF_UNDEFINED, //!< Undefined
      cXF_COMM,      //!< Communications
      cXF_SENSOR,    //!< Sensing
      cXF_INTERFERER //!< Interfering system
   };

   WsfEM_Xmtr(XmtrFunction aFunction, WsfEM_Antenna* aAntennaPtr = nullptr);
   WsfEM_Xmtr(const WsfEM_Xmtr& aSrc, WsfEM_Antenna* aAntennaPtr = nullptr);
   ~WsfEM_Xmtr() override;
   WsfEM_Xmtr& operator=(const WsfEM_Xmtr&) = delete;

   //! Required to display error messages in WsfComponentFactoryList.
   std::string GetName() const { return "transmitter"; }

   //! @name Component list methods.
   //!@{
   ComponentList&       GetComponents() { return mComponents; }
   const ComponentList& GetComponents() const { return mComponents; }
   //@}

   const char* GetScriptClassName() const override { return "WsfEM_Xmtr"; }

   void Activate();
   void Deactivate();

   virtual bool AllowInteractionWith(WsfEM_Rcvr* aRcvrPtr);

   virtual double ComputeRadiatedPower(double  aTargetAz,
                                       double  aTargetEl,
                                       double  aEBS_Az,
                                       double  aEBS_El,
                                       double  aFrequency,
                                       double& aAntennaGain);

   bool Initialize(WsfSimulation& aSimulation) override;

   bool ProcessInput(UtInput& aInput) override;

   virtual bool ProcessInputBlock(UtInput& aInput);

   bool AllowZeroFrequencyInput() const { return mAllowZeroFrequencyInput; }
   void AllowZeroFrequencyInput(bool val) { mAllowZeroFrequencyInput = val; }

   //! Get the receiver that is linked to this transmitter.
   //! @see SetLinkedReceiver for more information.
   WsfEM_Rcvr* GetLinkedReceiver() const { return mLinkedRcvrPtr; }

   //! Get the number of alternate frequencies specified.
   //! @return The number of alternate frequencies specified
   int GetAlternateFrequencyCount() const { return static_cast<int>(mAlternateFrequencyList.size()); }

   //! Get the frequencies container. This is for External Services.
   using FrequencyList = std::map<size_t, double>;
   const FrequencyList& GetAlternateFrequencies() { return mAlternateFrequencyList; }

   int GetCurrentAlternateFrequencyId() const { return mCurrentAltFreqId; }

   double GetAlternateFrequency(unsigned int aId) const;

   bool ExplicitFrequencyList() const { return mExplicitFrequencyList; }

   //! Get the intended function of the transmitter.
   //! @returns The intended function of the transmitter.
   XmtrFunction GetFunction() const { return mFunction; }

   //! @name Power Functions
   //! Returns peak or average power as indicated by name,
   //! or in the case of GetPower, conditionally returns
   //! either peak or average power based on user preference.
   //@{
   double GetPeakPower(double aFrequency = 0.0) const;
   double GetAveragePower(double aFrequency = 0.0) const;
   double GetPower(double aFrequency = 0.0) const;
   //@}

   //! Get the pulse compression ratio.
   //! @return Pulse compression ratio (absolute ratio, NOT dB).
   double GetPulseCompressionRatio() const { return mPulseCompressionRatio; }

   //! Get the number of PRFs specified.
   //! @return The number of PRFs specified
   int GetPulseRepetitionFrequencyCount() const { return static_cast<int>(mPulseRepetitionFrequencies.size() - 1); }

   //! Get the pulse repetition frequency
   //! @return Pulse repetition frequency (Hertz).  If 0.0 then it is a continuous wave transmitter.
   double GetPulseRepetitionFrequency(size_t aIndex = 0) const;

   //! Get the pulse repetition frequency list
   //! @return Pulse repetition frequency list (Hertz).
   std::vector<double> GetPulseRepetitionFrequencies() const;

   //! Get the number of PRFs specified.
   //! @return The number of PRFs specified
   int GetPulseRepetitionIntervalCount() const { return static_cast<int>(mPulseRepetitionIntervals.size() - 1); }

   //! Get the pulse repetition interval (1 / PRF).
   //! @return Pulse repetition interval (seconds). If 0.0 then it is a continuous wave transmitter.
   double GetPulseRepetitionInterval(size_t aIndex = 0) const;

   //! Get the pulse repetition interval list
   //! @return Pulse repetition interval list (1 / PRF).
   std::vector<double> GetPulseRepetitionIntervals() const;

   //! Get the pulse width
   //! @return Pulse width (seconds). If 0.0 then it is a continuous wave transmitter.
   double GetPulseWidth() const { return mPulseWidth; }

   //! Get the duty-cycle
   //! @return Duty-cycle no-units. If 1.0 then average power is equal to peak power.
   double GetDutyCycle() const { return mDutyCycle; }

   //! Get if the transmitter reports and uses peak power instead of average power.
   //! @return True if using peak power as indicated by the user, false by default.
   //! @note This method does not report true for using peak power if the duty cycle
   //! is set to 1.0, and the average power is effectively equivalent to peak power.
   bool IsUsingPeakPower() const { return mUsePeakPower; }

   void SetLinkedReceiver(WsfEM_Rcvr* aRcvrPtr);

   void SetBandwidth(double aBandwidth) override;

   void SetFrequency(double aFrequency) override;

   void SelectAlternateFrequency(unsigned int aId);

   bool UseDefaultFrequency() { return mUseDefaultFrequency; }
   void UseDefaultFrequency(bool aUseDefaultFrequency) { mUseDefaultFrequency = aUseDefaultFrequency; }

   void SetPolarization(WsfEM_Types::Polarization aPolarization) override;

   void SetPower(double aPower);
   bool SetPower(double aPower, double aFrequency);

   void SetPulseCompressionRatio(double aPulseCompressionRatio);

   void SetPulseRepetitionFrequency(double aPulseRepetitionFrequency, size_t aPRF_Num = 1);

   void SetPulseRepetitionInterval(double aPulseRepetitionInterval, size_t aPRF_Num = 1);

   void SetPulseWidth(double aPulseWidth);

   void SetDutyCycle(double aDutyCycle);

   //! @name Passive listener list maintenance.
   //@{
   virtual bool AddListener(WsfEM_Rcvr* aRcvrPtr);

   virtual bool RemoveListener(WsfEM_Rcvr* aRcvrPtr);

   virtual void NotifyListeners(double aSimTime, WsfEM_Interaction& aResult);

   //! Return true if any passive listeners have been attached.
   virtual bool HasListeners() const { return (!mListeners.empty()); }

   virtual bool AddChangeListener(WsfEM_Rcvr* aRcvrPtr);

   virtual bool RemoveChangeListener(WsfEM_Rcvr* aRcvrPtr);

   virtual void NotifyChangeListeners(double aSimTime, size_t aTargetIndex);

   //! Return true if any passive listeners have been attached.
   virtual bool HasChangeListeners() const { return (!mListeners.empty()); }
   //@}

   //! @name Transmission End Time maintenance.
   //! The transmission end time is primarily used by the passive type sensors to maintain detection of a
   //! long intermittent transmission.
   //@{
   //! Get the simulation time when the current transmission is to be completed.
   //! @returns The simulation time when the current transmission is to be completed.
   //! The value will be less than the current simulation time if no transmission is active.
   double GetTransmissionEndTime() const { return mTransmissionEndTime; }

   //! Set the simulation time when the current transmission is to be completed.
   void SetTransmissionEndTime(double aTransmissionEndTime) { mTransmissionEndTime = aTransmissionEndTime; }

   //@}

private:
   //! The list of extension components.
   ComponentList mComponents;

   //! Pointer to the EM manager when active.  May be null if inactive
   WsfEM_Manager* mEM_ManagerPtr;

   //! The receiver that is linked to this transmitter.
   WsfEM_Rcvr* mLinkedRcvrPtr;

   //! List of the alternate frequencies sorted by Id for preference order.
   FrequencyList mAlternateFrequencyList;

   PowerList           mPowerList;
   double              mPulseCompressionRatio;
   std::vector<double> mPulseRepetitionFrequencies; //! 1-based indexing, 0 index is the average PRF
   std::vector<double> mPulseRepetitionIntervals;   //! 1-based indexing, 0 index is the average PRI
   double              mPulseWidth;
   double              mDutyCycle;

   //! The simulation time when the current transmission will complete.
   //! (used by RF comm devices to indicate when the current transmission will end).
   double mTransmissionEndTime;

   using Listeners = std::vector<WsfEM_Rcvr*>;
   //! The list of receivers that want to be informed when this transmitter emits.
   Listeners mListeners;
   //! The list of receivers that want to be informed when this transmitter changes parameters.
   Listeners mChangeListeners;

   XmtrFunction mFunction;

   //! The current frequency Id that is being used.
   unsigned int mCurrentAltFreqId;

   //! Check as to whether a 'frequency_channel' or 'frequency_list...'
   //! was input for the frequency as opposed to a 'alternate_frequency' inputs.
   bool mExplicitFrequencyList;

   //! If set the transmitter will use the 'frequency' input or first frequency
   // in the frequency list and ignore the alternate list.
   bool mUseDefaultFrequency;

   bool mAllowZeroFrequencyInput;

   //! If set, calls to GetPower will return peak power instead of average power.
   //! By default, average power is used unless specified by the user.
   bool mUsePeakPower;
};

#endif
